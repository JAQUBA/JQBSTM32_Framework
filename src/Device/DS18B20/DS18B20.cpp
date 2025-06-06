#include "DS18B20.h"
#include "../../Core.h"
#include "../../Hardware/FlashMemory/FlashMemory.h"

#ifdef __DS18B20_H_

void unpack_rom(uint64_t number, uint8_t *result) {
    result[7] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[6] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[5] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[4] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[3] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[2] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[1] = number & 0x00000000000000FF ; number = number >> 8 ;
    result[0] = number & 0x00000000000000FF ;
}

uint64_t pack_rom(uint8_t *buffer) {
    uint64_t value = 0;
    for(int i = 0; i < 8; i++) {
        value = (value << 8) + buffer[i];
    }
    return value;
}

// DS18B20Manager Implementation
DS18B20Manager::DS18B20Manager(OneWire *oneWire, IExternalMemory *flashMemory) 
    : oneWire(oneWire), flashMemory(flashMemory), sensorCount(0), 
      autoReadEnabled(false), autoReadInterval(1000), currentState(IDLE), currentSensorId(0) {
    
    // Inicjalizacja tabeli czujników
    for(int i = 0; i < DS_MAX_SENSORS; i++) {
        sensors[i].romCode = 0;
        sensors[i].temperature = 0.0f;
        sensors[i].isValid = false;
        sensors[i].lastReadTime = 0;
    }
    
    // Uruchomienie zadania obsługi auto-read
    addTaskMain(taskCallback {
        if(autoReadEnabled) {
            processAutoRead();
        }
    }, autoReadInterval, true);
}

bool DS18B20Manager::addSensor(uint8_t id, uint64_t romCode) {
    if(id >= DS_MAX_SENSORS || !isValidRom(romCode)) return false;
    
    sensors[id].romCode = romCode;
    sensors[id].isValid = true;
    sensors[id].temperature = 0.0f;
    sensors[id].lastReadTime = 0;
    
    if(id >= sensorCount) sensorCount = id + 1;
    return true;
}

bool DS18B20Manager::addSensorFromFlash(uint8_t id, uint32_t flashAddress) {
    if(!flashMemory || id >= DS_MAX_SENSORS) return false;
    
    uint64_t romCode = static_cast<FlashMemory*>(flashMemory)->read(flashAddress);
    return addSensor(id, romCode);
}

void DS18B20Manager::removeSensor(uint8_t id) {
    if(id >= DS_MAX_SENSORS) return;
    
    sensors[id].romCode = 0;
    sensors[id].isValid = false;
    sensors[id].temperature = 0.0f;
    sensors[id].lastReadTime = 0;
    
    // Aktualizuj liczbę czujników
    while(sensorCount > 0 && !sensors[sensorCount-1].isValid) {
        sensorCount--;
    }
}

void DS18B20Manager::readRomFromBus(std::function<void(uint64_t romCode)> callback) {
    if(currentState != IDLE) return;
    
    romCallback = callback;
    currentState = READING_ROM;
    
    oneWire->reset();
    uint8_t cmd = DS18B20_READ_ROM;
    oneWire->transmitThenReceive(&cmd, 1, romBuffer, 8, dataCallback {
        handleRomRead();
    });
}

void DS18B20Manager::startConversion() {
    if(currentState != IDLE) return;
    
    currentState = CONVERTING;
    oneWire->reset();
    uint8_t cmd = DS18B20_SKIP_ROM;
    oneWire->transmit(&cmd, 1);
    cmd = DS18B20_CONVERT_T;
    oneWire->transmit(&cmd, 1, dataCallback {
        addTaskMain(taskCallback {
            handleConversionComplete();
        }, 750, true); // 750ms delay dla konwersji
    });
}

void DS18B20Manager::startConversionForSensor(uint8_t id) {
    if(currentState != IDLE || id >= DS_MAX_SENSORS || !sensors[id].isValid) return;
    
    currentSensorId = id;
    currentState = CONVERTING;
    
    oneWire->reset();
    uint8_t cmd = DS18B20_MATCH_ROM;
    oneWire->transmit(&cmd, 1);
    
    uint8_t rom[8];
    unpack_rom(sensors[id].romCode, rom);
    oneWire->transmit(rom, 8);
    
    cmd = DS18B20_CONVERT_T;
    oneWire->transmit(&cmd, 1, dataCallback {
        addTaskMain(taskCallback {
            handleConversionComplete();
        }, 750, true);
    });
}

void DS18B20Manager::readTemperature(uint8_t id, TemperatureCallback callback) {
    if(currentState != IDLE || id >= DS_MAX_SENSORS || !sensors[id].isValid) {
        if(callback) callback(id, 0.0f, false);
        return;
    }
    
    currentSensorId = id;
    tempCallback = callback;
    currentState = READING_SCRATCHPAD;
    
    oneWire->reset();
    uint8_t cmd = DS18B20_MATCH_ROM;
    oneWire->transmit(&cmd, 1);
    
    uint8_t rom[8];
    unpack_rom(sensors[id].romCode, rom);
    oneWire->transmit(rom, 8);
    
    cmd = DS18B20_READ_SCRATCHPAD;
    oneWire->transmitThenReceive(&cmd, 1, scratchpadBuffer, 9, dataCallback {
        handleScratchpadRead();
    });
}

void DS18B20Manager::readAllTemperatures(TemperatureCallback callback) {
    // Rozpocznij konwersję dla wszystkich czujników
    startConversion();
    
    // Po konwersji odczytaj temperatury wszystkich czujników
    addTaskMain(taskCallback {
        for(uint8_t i = 0; i < sensorCount; i++) {
            if(sensors[i].isValid) {
                readTemperature(i, callback);
                // Dodaj małe opóźnienie między odczytami
                addTaskMain(taskCallback {}, 50, true);
            }
        }
    }, 800, true); // 800ms po rozpoczęciu konwersji
}

float DS18B20Manager::getTemperature(uint8_t id) {
    if(id >= DS_MAX_SENSORS || !sensors[id].isValid) return 0.0f;
    return sensors[id].temperature;
}

int16_t DS18B20Manager::getRawTemperature(uint8_t id) {
    if(id >= DS_MAX_SENSORS || !sensors[id].isValid) return 0;
    return (int16_t)(sensors[id].temperature * 16.0f);
}

bool DS18B20Manager::isSensorValid(uint8_t id) {
    if(id >= DS_MAX_SENSORS) return false;
    return sensors[id].isValid;
}

uint64_t DS18B20Manager::getSensorRom(uint8_t id) {
    if(id >= DS_MAX_SENSORS || !sensors[id].isValid) return 0;
    return sensors[id].romCode;
}

uint8_t DS18B20Manager::getSensorCount() {
    return sensorCount;
}

void DS18B20Manager::enableAutoRead(uint32_t intervalMs) {
    autoReadInterval = intervalMs;
    autoReadEnabled = true;
}

void DS18B20Manager::disableAutoRead() {
    autoReadEnabled = false;
}

void DS18B20Manager::setAutoReadCallback(TemperatureCallback callback) {
    autoReadCallback = callback;
}

void DS18B20Manager::processAutoRead() {
    static uint8_t currentAutoSensor = 0;
    static bool conversionStarted = false;
    static uint32_t conversionTime = 0;
    
    if(!conversionStarted) {
        // Rozpocznij konwersję
        if(currentAutoSensor < sensorCount && sensors[currentAutoSensor].isValid) {
            startConversionForSensor(currentAutoSensor);
            conversionStarted = true;
            conversionTime = millis();
        } else {
            currentAutoSensor = (currentAutoSensor + 1) % sensorCount;
        }
    } else {        // Sprawdź czy konwersja się zakończyła
        if(millis() - conversionTime >= 750) {
            readTemperature(currentAutoSensor, [this](uint8_t id, float temp, bool success) {
                if(autoReadCallback && success) {
                    autoReadCallback(id, temp, success);
                }
            });
            
            conversionStarted = false;
            currentAutoSensor = (currentAutoSensor + 1) % sensorCount;
        }
    }
}

void DS18B20Manager::handleRomRead() {
    currentState = IDLE;
    if(romCallback) {
        uint64_t rom = pack_rom(romBuffer);
        romCallback(rom);
        romCallback = nullptr;
    }
}

void DS18B20Manager::handleConversionComplete() {
    currentState = IDLE;
}

void DS18B20Manager::handleScratchpadRead() {
    currentState = IDLE;
    
    float temperature = calculateTemperature(scratchpadBuffer);
    sensors[currentSensorId].temperature = temperature;
    sensors[currentSensorId].lastReadTime = millis();
    
    if(tempCallback) {
        tempCallback(currentSensorId, temperature, true);
        tempCallback = nullptr;
    }
}

float DS18B20Manager::calculateTemperature(uint8_t* scratchpad) {
    int16_t rawTemperature = (scratchpad[1] << 8) | scratchpad[0];
    return rawTemperature / 16.0f;
}

bool DS18B20Manager::isValidRom(uint64_t romCode) {
    // Podstawowa walidacja ROM (sprawdź czy nie jest zerem i czy pierwszy bajt to 0x28 dla DS18B20)
    if(romCode == 0) return false;
    uint8_t familyCode = romCode & 0xFF;
    return familyCode == 0x28; // DS18B20 family code
}

// DS18B20 Single Sensor Implementation (kompatybilność wsteczna)
DS18B20::DS18B20(OneWire *oneWire, uint64_t romCode) {
    manager = new DS18B20Manager(oneWire);
    manager->addSensor(0, romCode);
    sensorId = 0;
    ownsManager = true;
}

DS18B20::DS18B20(OneWire *oneWire, IExternalMemory *flashMemory, uint32_t flashAddress) {
    manager = new DS18B20Manager(oneWire, flashMemory);
    manager->addSensorFromFlash(0, flashAddress);
    sensorId = 0;
    ownsManager = true;
}

void DS18B20::startConversion() {
    manager->startConversionForSensor(sensorId);
}

void DS18B20::readTemperature(TemperatureCallback callback) {
    manager->readTemperature(sensorId, callback);
}

float DS18B20::getTemperature() {
    return manager->getTemperature(sensorId);
}

int16_t DS18B20::getRawTemperature() {
    return manager->getRawTemperature(sensorId);
}

bool DS18B20::isValid() {
    return manager->isSensorValid(sensorId);
}

uint64_t DS18B20::getRomCode() {
    return manager->getSensorRom(sensorId);
}

#endif // __DS18B20_H__