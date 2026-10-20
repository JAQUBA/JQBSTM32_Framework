/*
 * JQBSTM32 Framework - Modbus Master Implementation
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * Implementation of Modbus RTU protocol according to official
 * Modbus specification (www.modbus.org).
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

#include "ModbusMaster.h"
#include "../Math/CRC16.h"

#define MODBUS_MASTER_POLL_MS 20U

ModbusMaster::ModbusMaster(UART *bus) : _bus(bus) {
    addTaskMain(taskCallback {
        if (_state == WAITING && static_cast<int32_t>(millis() - _timeoutAt) >= 0) {
            _statistics.timeouts++;
            _finish(false, 0U, nullptr, 0U);
        }
    }, MODBUS_MASTER_POLL_MS);
}
bool ModbusMaster::isBusy() const {
    return _state != IDLE;
}
bool ModbusMaster::readHoldingRegisters(uint8_t slaveId, uint16_t address, uint16_t count,
                                         ResponseCallback callback, uint32_t timeoutMs) {
    if (_state != IDLE || _bus == nullptr || count == 0U || count > MaxReadRegisters) return false;

    uint8_t frame[8];
    frame[0] = slaveId;
    frame[1] = (uint8_t)ModbusFunction::FUNC_3;
    writeU16BE(frame + 2U, address);
    writeU16BE(frame + 4U, count);
    const uint16_t len = appendCrc(frame, 6U);

    _expectedSlaveId = slaveId;
    _expectedFunction = (uint8_t)ModbusFunction::FUNC_3;
    _expectedCount = count;
    _callback = callback;
    _timeoutAt = millis() + timeoutMs;
    _responseLength = 0U;
    _state = WAITING;

    _bus->transmit(frame, len);
    return true;
}
bool ModbusMaster::writeHoldingRegisters(uint8_t slaveId, uint16_t address, const uint16_t *registers,
                                         uint16_t count, WriteCallback callback, uint32_t timeoutMs) {
    if (_state != IDLE || _bus == nullptr || slaveId == 0U || slaveId > 247U ||
        registers == nullptr || count == 0U || count > 123U || callback == nullptr) return false;

    uint8_t frame[255];
    frame[0] = slaveId;
    frame[1] = static_cast<uint8_t>(FUNC_16);
    writeU16BE(frame + 2U, address);
    writeU16BE(frame + 4U, count);
    frame[6] = static_cast<uint8_t>(count * 2U);
    for (uint16_t index = 0U; index < count; index++) {
        writeU16BE(frame + 7U + index * 2U, registers[index]);
    }
    const uint16_t length = appendCrc(frame, static_cast<uint16_t>(7U + count * 2U));
    _expectedSlaveId = slaveId;
    _expectedFunction = static_cast<uint8_t>(FUNC_16);
    _expectedAddress = address;
    _expectedCount = count;
    _callback = [callback](bool success, uint8_t exception, uint16_t*, uint16_t) {
        callback(success, exception);
    };
    _timeoutAt = millis() + timeoutMs;
    _responseLength = 0U;
    _state = WAITING;
    _bus->transmit(frame, length);
    return true;
}
bool ModbusMaster::readHoldingRegistersAsUint32(uint8_t slaveId, uint16_t address, Uint32Callback callback,
                                                 bool wordSwap, uint32_t timeoutMs) {
    return readHoldingRegisters(slaveId, address, 2U,
        [callback, wordSwap](bool success, uint8_t exception, uint16_t *registers, uint16_t count) {
            if (callback) callback(success, exception, success && count == 2U ? registersToUint32(registers, wordSwap) : 0U);
        }, timeoutMs);
}
bool ModbusMaster::readHoldingRegistersAsInt32(uint8_t slaveId, uint16_t address, Int32Callback callback,
                                                bool wordSwap, uint32_t timeoutMs) {
    return readHoldingRegisters(slaveId, address, 2U,
        [callback, wordSwap](bool success, uint8_t exception, uint16_t *registers, uint16_t count) {
            if (callback) callback(success, exception, success && count == 2U ? registersToInt32(registers, wordSwap) : 0);
        }, timeoutMs);
}
bool ModbusMaster::readHoldingRegistersAsFloat(uint8_t slaveId, uint16_t address, FloatCallback callback,
                                                bool wordSwap, uint32_t timeoutMs) {
    return readHoldingRegisters(slaveId, address, 2U,
        [callback, wordSwap](bool success, uint8_t exception, uint16_t *registers, uint16_t count) {
            if (callback) callback(success, exception, success && count == 2U ? registersToFloat(registers, wordSwap) : 0.0f);
        }, timeoutMs);
}
bool ModbusMaster::readHoldingRegistersAsDouble(uint8_t slaveId, uint16_t address, DoubleCallback callback,
                                                 bool wordSwap, uint32_t timeoutMs) {
    return readHoldingRegisters(slaveId, address, 4U,
        [callback, wordSwap](bool success, uint8_t exception, uint16_t *registers, uint16_t count) {
            if (callback) callback(success, exception, success && count == 4U ? registersToDouble(registers, wordSwap) : 0.0);
        }, timeoutMs);
}
void ModbusMaster::onReceive(uint8_t *data, uint16_t length) {

    if (_state != WAITING || data == nullptr || length == 0U) return;

    if (length > sizeof(_responseBuffer) - _responseLength) {
        _statistics.malformedFrames++;
        _finish(false, 0U, nullptr, 0U);
        return;
    }

    memcpy(_responseBuffer + _responseLength, data, length);
    _responseLength = (uint16_t)(_responseLength + length);
    if (_responseLength < 2U) return;
    const bool exceptionResponse = _responseBuffer[1] == static_cast<uint8_t>(_expectedFunction | 0x80U);
    const uint16_t expectedLength = exceptionResponse ? 5U :
        (_expectedFunction == FUNC_16 ? 8U : static_cast<uint16_t>(5U + _expectedCount * 2U));
    if (_responseLength < expectedLength) return;
    if (_responseLength != expectedLength) {
        _statistics.malformedFrames++;
        _finish(false, 0U, nullptr, 0U);
        return;
    }

    if (_responseBuffer[0] != _expectedSlaveId) {
        _statistics.malformedFrames++;
        _finish(false, 0U, nullptr, 0U);
        return;
    }
    if (!validateCrc(_responseBuffer, _responseLength)) {
        _statistics.crcErrors++;
        _finish(false, 0U, nullptr, 0U);
        return;
    }

    if (exceptionResponse) {
        _statistics.exceptions++;
        _finish(false, _responseBuffer[2], nullptr, 0U);
        return;
    }
    if (_responseBuffer[1] != _expectedFunction) {
        _statistics.malformedFrames++;
        _finish(false, 0U, nullptr, 0U);
        return;
    }
    if (_expectedFunction == FUNC_16) {
        if (readU16BE(_responseBuffer + 2U) != _expectedAddress ||
            readU16BE(_responseBuffer + 4U) != _expectedCount) {
            _statistics.malformedFrames++;
            _finish(false, 0U, nullptr, 0U);
            return;
        }
        _statistics.requests++;
        _finish(true, 0U, nullptr, 0U);
        return;
    }
    if (_responseBuffer[2] != (uint8_t)(_expectedCount * 2U)) {
        _statistics.malformedFrames++;
        _finish(false, 0U, nullptr, 0U);
        return;
    }

    uint16_t registers[MaxReadRegisters];
    for (uint16_t index = 0U; index < _expectedCount; index++) {
        registers[index] = readU16BE(_responseBuffer + 3U + index * 2U);
    }
    _statistics.requests++;
    _finish(true, 0U, registers, _expectedCount);
}
void ModbusMaster::_finish(bool success, uint8_t exception, uint16_t *registers, uint16_t count) {
    _state = IDLE;
    ResponseCallback callback = _callback;
    _callback = nullptr;
    if (callback) callback(success, exception, registers, count);
}
