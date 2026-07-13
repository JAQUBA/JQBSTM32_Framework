# Device Drivers

## Overview

The Device layer provides ready-to-use drivers for popular external ICs. Each driver wraps a Hardware bus module (I2C, SPI, OneWire) and provides a high-level, domain-specific API. Memory devices implement the `IExternalMemory` interface for uniform access.

---

## DS18B20 — OneWire Temperature Sensor

**Header**: `Device/DS18B20/DS18B20.h`  
**Bus**: OneWire  
**Protocol**: Dallas/Maxim 1-Wire

### Constructor

```cpp
DS18B20(OneWire *oneWire);
```

### API

```cpp
// Read temperature from specific sensor by ROM address
void readTemperature(uint64_t deviceAddress,
                     const std::function<void(float)> &callback);

// Read ROM of the only device on the bus (single-device mode)
void readSingleDeviceROM(const std::function<void(uint64_t, bool)> &callback);
```

### Features

- 12-bit resolution (default)
- Multi-sensor support via 64-bit ROM addressing
- Parasite power compatible
- Asynchronous callback-based results

### Usage

```cpp
OneWire ow(new Timer(&htim7), GPIOB, GPIO_PIN_12);
DS18B20 ds(&ow);

// Read temperature (address previously discovered)
ds.readTemperature(sensorAddress, [](float temp) {
    currentTemp = temp;  // Temperature in °C
});

// Discover single sensor ROM
ds.readSingleDeviceROM([](uint64_t rom, bool success) {
    if (success) sensorAddress = rom;
});
```

---

## PCF8583 — I2C RTC + 240B RAM

**Header**: `Device/PCF8583/PCF8583.h`  
**Bus**: I2C  
**Implements**: `IExternalMemory`

### Constructor

```cpp
PCF8583(I2C *pInstance,
        uint16_t DevAddress = 0xA0,
        uint32_t BaseAddress = 0x00);
```

### API (IExternalMemory)

```cpp
void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
```

### Features

- Real-time clock with BCD time registers
- 240 bytes of static RAM (addresses 0x10–0xFF)
- 8-bit addressing
- I2C address: 0xA0 or 0xA2 (hardware selectable)

### Usage

```cpp
I2C sys_i2c(&hi2c2);
PCF8583 rtc(&sys_i2c, 0xA0);

// Read time registers (seconds at 0x02, minutes at 0x03, etc.)
uint8_t timeRegs[4];
rtc.readFromMemory(0x02, timeRegs, 4);
uint8_t seconds = bcdToDec(timeRegs[0]);
uint8_t minutes = bcdToDec(timeRegs[1]);

// Use RAM area for small data storage
uint8_t data[] = {0x42, 0x00};
rtc.writeToMemory(0x10, data, 2);
```

---

## EEP24Cxx — Generic I2C EEPROM (24AA/24LC/24FC Family)

**Header**: `Device/EEP24Cxx/EEP24Cxx.h`  
**Bus**: I2C  
**Implements**: `IExternalMemory`

### Constructor

```cpp
EEP24Cxx(I2C *pInstance,
         const EEP24Cxx::DeviceConfig &config,
         uint16_t DevAddress = 0xA0,
         uint32_t BaseAddress = 0x00);
```

### Built-in Device Configs

```cpp
EEP24Cxx::CONFIG_24AA02
EEP24Cxx::CONFIG_24LC02B
EEP24Cxx::CONFIG_24LC04B
EEP24Cxx::CONFIG_24LC08B
EEP24Cxx::CONFIG_24LC16B
EEP24Cxx::CONFIG_24LC32A
EEP24Cxx::CONFIG_24LC64
EEP24Cxx::CONFIG_24LC128
EEP24Cxx::CONFIG_24LC256
EEP24Cxx::CONFIG_24LC512
```

Driver automatycznie:
- Dzieli zapis na granicach stron EEPROM,
- Dla małych układów (8-bit address + block bits) przełącza bity bloku w adresie urządzenia,
- Wykonuje sekwencyjny odczyt/zapis dla dowolnego zakresu danych.

### Usage (24AA02)

```cpp
I2C sys_i2c(&hi2c2);
EEP24Cxx eeprom(&sys_i2c, EEP24Cxx::CONFIG_24AA02, 0xA0);

uint8_t data[8] = {1,2,3,4,5,6,7,8};
eeprom.writeToMemory(0x00, data, sizeof(data));
```

### Usage (24LC256)

```cpp
I2C sys_i2c(&hi2c2);
EEP24Cxx eeprom(&sys_i2c, EEP24Cxx::CONFIG_24LC256, 0xA0);
```

### Backward Compatibility

Istniejace klasy `EEP24C04` i `EEP24C256` pozostaja dostepne i korzystaja wewnetrznie z `EEP24Cxx`.

---

## EEP24C04 — I2C EEPROM (512 Bytes)

**Header**: `Device/EEP24C04/EEP24C04.h`  
**Bus**: I2C  
**Implements**: `IExternalMemory`

### Constructor

```cpp
EEP24C04(I2C *pInstance,
         uint16_t DevAddress = 0xA0,
         uint32_t BaseAddress = 0x00);
```

### Specifications

| Parameter       | Value              |
|----------------|--------------------|
| Capacity       | 4 Kbit (512 bytes) |
| Page size      | 16 bytes           |
| Address size   | 8-bit              |
| Write time     | 5–10 ms per page   |
| Endurance      | 1M write cycles    |

### API (IExternalMemory)

```cpp
void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
```

### Usage

```cpp
I2C sys_i2c(&hi2c2);
EEP24C04 eeprom(&sys_i2c, 0xA0);

uint8_t buf[16];
eeprom.readFromMemory(0x00, buf, 16);
```

---

## EEP24C256 — I2C EEPROM (32 KB)

**Header**: `Device/EEP24C256/EEP24C256.h`  
**Bus**: I2C  
**Implements**: `IExternalMemory`

### Constructor

```cpp
EEP24C256(I2C *pInstance,
          uint16_t DevAddress = 0xA0,
          uint32_t BaseAddress = 0x00);
```

### Specifications

| Parameter       | Value              |
|----------------|--------------------|
| Capacity       | 256 Kbit (32 KB)   |
| Page size      | 64 bytes           |
| Address size   | 16-bit             |
| Write time     | 5–10 ms per page   |
| Endurance      | 1M write cycles    |

### API (IExternalMemory)

```cpp
void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
```

### Usage

```cpp
I2C sys_i2c(&hi2c2);
EEP24C256 eeprom(&sys_i2c, 0xA0);

// Store configuration block
uint8_t config[64] = { /* ... */ };
eeprom.writeToMemory(0x0000, config, 64);

// Use with RegisterBank for persistent config
RegisterBank params(0, 32, new MemoryBlock(&eeprom, 0x0000));
```

---

## FM25V05 — SPI FRAM (64 KB)

**Header**: `Device/FM25V05/FM25V05.h`  
**Bus**: SPI  
**Implements**: `IExternalMemory`

### Constructor

```cpp
FM25V05(SPI *pInstance, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
```

The GPIO pin serves as the chip select (CS) line.

### Specifications

| Parameter       | Value              |
|----------------|--------------------|
| Capacity       | 512 Kbit (64 KB)   |
| Address size   | 16-bit             |
| Write time     | **Instant** (no delay) |
| Endurance      | 10^14 write cycles |
| Speed          | Up to 20 MHz SPI   |

### API (IExternalMemory)

```cpp
void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
```

### Advantages over EEPROM

- **No write delay** — data available immediately after write
- **Virtually unlimited endurance** — ideal for frequently updated data
- **Byte-level granularity** — no page size restrictions

### Usage

```cpp
SPI spi1(&hspi1);
FM25V05 fram(&spi1, GPIOB, GPIO_PIN_6);

// Instant write — no delay needed
uint8_t data[] = {0x01, 0x02, 0x03};
fram.writeToMemory(0x0000, data, 3);

// Use with RegisterBank for high-frequency persistence
RegisterBank live(100, 64, new MemoryBlock(&fram, 0x0000));
```

---

## TMC2209 — Stepper Motor Driver

**Header**: `Device/TMC2209/TMC2209.h`  
**Bus**: GPIO (direct pin control)

### Constructor

```cpp
TMC2209(GPIO_TypeDef *enablePort, uint16_t enablePin,
        GPIO_TypeDef *stepPort, uint16_t stepPin,
        GPIO_TypeDef *directionPort, uint16_t directionPin,
        GPIO_TypeDef *MS1Port, uint16_t MS1Pin,
        GPIO_TypeDef *MS2Port, uint16_t MS2Pin,
        GPIO_TypeDef *zeroPort, uint16_t zeroPin);
```

### API

```cpp
void step(int16_t steps = 1);     // Execute n steps
void forward();                    // Step forward once
void back();                       // Step backward once
void reverse();                    // Reverse direction
void enable();                     // Enable motor driver
void disable();                    // Disable motor driver
void direction(Direction dir);     // Set direction explicitly
void setLimit(uint32_t limit);     // Set step count limit
```

### Direction Constants

```cpp
#define STEPPER_RIGHT  1   // Clockwise
#define STEPPER_LEFT   0   // Counter-clockwise
```

### Usage

```cpp
TMC2209 stepper(
    GPIOA, GPIO_PIN_0,   // Enable
    GPIOA, GPIO_PIN_1,   // Step
    GPIOA, GPIO_PIN_2,   // Direction
    GPIOA, GPIO_PIN_3,   // MS1
    GPIOA, GPIO_PIN_4,   // MS2
    GPIOA, GPIO_PIN_5    // Zero/home switch
);

stepper.enable();
stepper.direction(STEPPER_RIGHT);
stepper.step(200);  // One full rotation (200 steps @ full-step)
```

---

## DFR0646 — I2C 4-Digit 7-Segment Display

**Header**: `Device/DFR0646/DFR0646.h`  
**Bus**: I2C  
**Controller**: HT16K33

### Constructor

```cpp
DFR0646(I2C *pInstance, uint8_t DevAddress);
```

### API

```cpp
void printLed(char *wsk, uint8_t dot_point);
```

### Parameters

- `wsk`: 4-character string to display (digits 0-9, some letters)
- `dot_point`: Bitmask for decimal point per digit (bit 0 = digit 0, etc.)

### Address Range

I2C addresses: 0x70–0x77 (configurable via hardware jumpers)

### Usage

```cpp
I2C sys_i2c(&hi2c2);
DFR0646 display(&sys_i2c, 0x70);

char text[] = "1234";
display.printLed(text, 0b0100);  // Show "12.34" (dot on digit 2)
```

---

## SED1520 — I2C Graphic LCD Controller (122×32)

**Header**: `Device/SED1520/SED1520.h`  
**Bus**: I2C

### Constructor

```cpp
SED1520(I2C *i2c);
```

### API

```cpp
void clear();                              // Clear display buffer
void print(char const *text, bool fill = false);  // Print text at cursor
void setCursor(uint8_t y, uint8_t x);     // Set cursor position (row, col)
void flush();                              // Write buffer to display
```

### Display Specifications

| Parameter     | Value              |
|--------------|--------------------|
| Resolution   | 122 × 32 pixels    |
| Controllers  | 2 × SED1520 (61×32 each) |
| Font          | Built-in 6×8 pixels |
| Text rows    | 4 rows × 21 characters |

### Usage

```cpp
I2C sys_i2c(&hi2c2);
SED1520 lcd(&sys_i2c);

lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Hello World");
lcd.setCursor(1, 0);
lcd.print("Temperature: 25C");
lcd.flush();
```
