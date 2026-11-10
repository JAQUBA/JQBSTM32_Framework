/*
 * JQBSTM32 Framework - DS18B20 Temperature Sensor Driver Implementation
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * Driver for Dallas/Maxim DS18B20 digital temperature sensor.
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <https://www.gnu.org/licenses/>.
 */

#include "DS18B20.h"
#include "../../Core.h"
#include <cstring>
#include <new>

#ifdef __DS18B20_H_

DS18B20::DS18B20(OneWire *oneWire, Resolution resolution) : ow(oneWire), resolution(resolution) {
}

void DS18B20::unpackRom(uint64_t romAddress, uint8_t *result) {
    for(int i = 7; i >= 0; i--) {
        result[i] = romAddress & 0xFF;
        romAddress >>= 8;
    }
}

uint64_t DS18B20::packRom(uint8_t *buffer) {
    uint64_t value = 0;
    for(int i = 0; i < 8; i++) {
        value = (value << 8) | buffer[i];
    }
    return value;
}    

void DS18B20::readTemperature(uint64_t deviceAddress, const std::function<void(float temperature)> &callbackFn) {
    readTemperatureWithStatus(deviceAddress, [callbackFn](bool success, float temperature) {
        if(success && callbackFn) callbackFn(temperature);
    });
}

void DS18B20::readTemperatureWithStatus(uint64_t deviceAddress, const std::function<void(bool, float)> &callbackFn) {
    Request *request = new (std::nothrow) Request;
    if(request == nullptr) {
        if(callbackFn) callbackFn(false, 0.0f);
        return;
    }

    request->type = RequestType::TEMPERATURE;
    request->temperatureStatusCallback = callbackFn;
    unpackRom(deviceAddress, request->rom);
    enqueueRequest(request);
}

void DS18B20::enqueueRequest(Request *request) {
    if(activeRequest == nullptr && !dispatchingCallback && requests.empty()) {
        activeRequest = request;
        startActiveRequest();
        return;
    }
    requests.push(request);
}

void DS18B20::startNextRequest() {
    if(activeRequest != nullptr || requests.empty()) return;
    activeRequest = requests.front();
    requests.pop();
    startActiveRequest();
}

void DS18B20::startActiveRequest() {
    if(ow == nullptr) {
        finishActiveRequest(false);
    } else if(activeRequest->type == RequestType::TEMPERATURE) {
        uint8_t resolutionBits = static_cast<uint8_t>(resolution);
        if(resolutionBits < 9 || resolutionBits > 12 ||
            activeRequest->rom[0] != 0x28 ||
            OneWire::calculateCRC8(activeRequest->rom, 7) != activeRequest->rom[7]) {
            finishActiveRequest(false);
            return;
        }
        startTemperatureRequest();
    } else {
        startRomRequest();
    }
}

void DS18B20::startTemperatureRequest() {
    uint8_t resolutionConfig = static_cast<uint8_t>(
        0x1F | ((static_cast<uint8_t>(resolution) - 9U) << 5)
    );
    uint8_t configBuffer[13] = {
        0x55,
        activeRequest->rom[0], activeRequest->rom[1], activeRequest->rom[2], activeRequest->rom[3],
        activeRequest->rom[4], activeRequest->rom[5], activeRequest->rom[6], activeRequest->rom[7],
        0x4E, 0x4B, 0x46, resolutionConfig
    };

    if(!ow->reset() || !ow->transmit(configBuffer, sizeof(configBuffer), [this](uint8_t *, uint16_t) {
        if(!ow->wasLastOperationSuccessful() || !ow->isDevicePresent()) {
            finishActiveRequest(false);
            return;
        }

        uint8_t convertBuffer[10] = {
            0x55,
            activeRequest->rom[0], activeRequest->rom[1], activeRequest->rom[2], activeRequest->rom[3],
            activeRequest->rom[4], activeRequest->rom[5], activeRequest->rom[6], activeRequest->rom[7],
            0x44
        };
        if(!ow->reset() || !ow->transmit(convertBuffer, sizeof(convertBuffer), [this](uint8_t *, uint16_t) {
            if(!ow->wasLastOperationSuccessful() || !ow->isDevicePresent()) {
                finishActiveRequest(false);
                return;
            }
            addTaskMain([this](taskStruct *) {
                readTemperatureScratchpad();
            }, conversionTimeMs(), true);
        })) {
            finishActiveRequest(false);
        }
    })) {
        finishActiveRequest(false);
    }
}

void DS18B20::readTemperatureScratchpad() {
    if(activeRequest == nullptr || activeRequest->type != RequestType::TEMPERATURE) return;

    uint8_t commandBuffer[10] = {
        0x55,
        activeRequest->rom[0], activeRequest->rom[1], activeRequest->rom[2], activeRequest->rom[3],
        activeRequest->rom[4], activeRequest->rom[5], activeRequest->rom[6], activeRequest->rom[7],
        0xBE
    };
    memset(activeRequest->scratchpad, 0xFF, sizeof(activeRequest->scratchpad));

    if(!ow->reset() || !ow->transmitThenReceive(commandBuffer, sizeof(commandBuffer), activeRequest->scratchpad, sizeof(activeRequest->scratchpad),
        [this](uint8_t *data, uint16_t size) {
            bool valid = ow->wasLastOperationSuccessful() && ow->isDevicePresent() &&
                size == sizeof(activeRequest->scratchpad) &&
                OneWire::calculateCRC8(data, 8) == data[8] &&
                (data[4] & 0x60) ==
                    ((static_cast<uint8_t>(resolution) - 9U) << 5);
            if(!valid) {
                finishActiveRequest(false);
                return;
            }

            int16_t rawTemperature = static_cast<int16_t>((static_cast<uint16_t>(data[1]) << 8) | data[0]);
            if(rawTemperature < -880 || rawTemperature > 2000) {
                finishActiveRequest(false);
                return;
            }
            finishActiveRequest(true, rawTemperature / 16.0f);
        })) {
        finishActiveRequest(false);
    }
}

uint32_t DS18B20::conversionTimeMs() const {
    switch(resolution) {
        case Resolution::BITS_9: return 94;
        case Resolution::BITS_10: return 188;
        case Resolution::BITS_11: return 375;
        case Resolution::BITS_12: return 750;
    }
    return 750;
}

void DS18B20::startRomRequest() {
    uint8_t readRomCommand = 0x33;
    memset(activeRequest->rom, 0xFF, sizeof(activeRequest->rom));
    if(!ow->reset() || !ow->transmitThenReceive(&readRomCommand, 1, activeRequest->rom, sizeof(activeRequest->rom),
        [this](uint8_t *data, uint16_t size) {
            bool valid = ow->wasLastOperationSuccessful() && ow->isDevicePresent() &&
                size == sizeof(activeRequest->rom) && data[0] == 0x28 &&
                OneWire::calculateCRC8(data, 7) == data[7];
            finishActiveRequest(valid);
        })) {
        finishActiveRequest(false);
    }
}

void DS18B20::finishActiveRequest(bool success, float temperature) {
    Request *completedRequest = activeRequest;
    if(completedRequest == nullptr) return;
    activeRequest = nullptr;
    dispatchingCallback = true;

    if(completedRequest->type == RequestType::TEMPERATURE) {
        if(completedRequest->temperatureStatusCallback) {
            completedRequest->temperatureStatusCallback(success, temperature);
        }
    } else if(completedRequest->romCallback) {
        uint64_t deviceAddress = success ? packRom(completedRequest->rom) : 0;
        completedRequest->romCallback(deviceAddress, success);
    }

    delete completedRequest;
    dispatchingCallback = false;
    startNextRequest();
}

void DS18B20::readSingleDeviceROM(const std::function<void(uint64_t, bool)> &callbackFn) {
    Request *request = new (std::nothrow) Request;
    if(request == nullptr) {
        if(callbackFn) callbackFn(0, false);
        return;
    }
    request->type = RequestType::ROM;
    request->romCallback = callbackFn;
    enqueueRequest(request);
}

#endif // __DS18B20_H__