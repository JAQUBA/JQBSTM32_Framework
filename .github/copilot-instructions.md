# JQBSTM32 Framework – Copilot Instructions

## Project Overview

**JQBSTM32 Framework v1.0.0** is a production-ready C++ library for rapid STM32 microcontroller development. It integrates with **STM32CubeMX** and **PlatformIO** to provide hardware abstraction, cooperative task scheduling, Modbus communication, and peripheral device drivers.

- **License**: GPL-3.0-or-later
- **Platform**: `ststm32` (PlatformIO)
- **Framework**: `stm32cube` (STM32Cube HAL)
- **Language**: C++ with C interop (`extern "C"`)

### Architecture

```
src/
├── Core.h / Core.cpp        — Entry point, global functions (millis, delay, map), HAL callbacks
├── Scheduler.hpp / .cpp     — Cooperative task scheduler (main + interrupt contexts)
├── WProgram.h               — Arduino compatibility layer (typedefs, includes)
├── fl_bit.h                 — Bit manipulation macros (bf_set, bf_clr, bfb_set, etc.)
├── Hardware/                — 13 HAL abstraction modules (GPIO, UART, I2C, SPI, Timer, PWM, Analog, CAN, FDCAN, OneWire, Encoder, FlashMemory, USB_CDC)
├── Interface/               — Base interfaces (IBus, IExternalMemory)
├── Device/                  — 8 device drivers (DS18B20, PCF8583, EEP24C04, EEP24C256, FM25V05, TMC2209, DFR0646, SED1520)
└── Util/                    — Utilities (Modbus, RegisterBank, MemoryBlock, SimpleMenu, Math)

scripts/
├── includeCore.py           — PlatformIO build integration (include paths, custom_using defines)
└── makehex.py               — HEX file generation post-build

examples/
└── blink/                   — Minimal STM32 blink example
```

---

## Coding Conventions

### Language

- **Code identifiers**: English
- **Comments**: English (Doxygen-style `@brief`, `@param`, `@return`, `@details`, `@note`)
- **Respond to prompts in Polish** when the user writes in Polish

### Naming

| Element                | Convention              | Example                                       |
|------------------------|-------------------------|-----------------------------------------------|
| Classes                | `PascalCase`            | `HardwareGPIO`, `RegisterBank`, `SimpleMenu`   |
| Class methods          | `camelCase`             | `transmit()`, `getValue()`, `readFromMemory()` |
| Member variables       | `_camelCase` (prefix _) | `_pHandler`, `_delay`, `_DevAddress`           |
| Static/class members   | `_camelCase` (prefix _) | `_taskNum`, `_UART_instances[]`                |
| Free functions         | `camelCase`             | `millis()`, `delay()`, `setup()`, `loop()`     |
| Callback typedefs      | suffix `_f`             | `taskCallback_f`, `dataCallback_f`, `voidCallback_f` |
| Macros / constants     | `UPPER_SNAKE_CASE`      | `HIGH`, `LOW`, `MAX_GPIO_INTERRUPTS`           |
| Enums                  | `PascalCase` members    | `Mode::INPUT`, `StartType::START_IT`           |
| Task macro             | `TASK(name)`            | `TASK(myTask) { ... }`                         |
| Task lambda            | `taskCallback`          | `taskCallback { ... }`                         |

### Formatting

- **Indentation**: 4 spaces (no tabs)
- **Braces**: K&R for control flow (`if (...) {`), Allman for function/class definitions
- **Include guards**: `#ifndef __HEADER_H_` / `#define __HEADER_H_` / `#endif`
- **Includes**: Framework headers with `"Core.h"`, STM32 HAL with `"main.h"`, standard with `<stdio.h>`
- **Copyright header**: GPL-3.0-or-later header required in every source file

---

## Core System

### Entry Points (User must implement)

```cpp
void init();    // Called by Core constructor — CubeMX peripheral initialization
void setup();   // Called once before main loop — application setup, task registration
void loop();    // Called continuously — prefer tasks over loop code
```

### Task Scheduler

Two execution contexts, cooperative (no preemption):

```cpp
// Periodic task: runs every 100ms in main loop context
addTaskMain(taskCallback {
    // task->delay, task->_delay, task->_id available
}, 100);

// One-shot task: fires once after 500ms
addTaskMain(taskCallback {
    // runs once then auto-removed
}, 500, true);

// Interrupt-context task: runs from SysTick ISR
addTaskInterrupt(taskCallback {
    // must be very short, no blocking
}, 10);

// Named function task
TASK(myTask) {
    // code
}
addTaskMain(myTask, 200);
```

**Time multipliers** (4th parameter): `MUL_100US`, `MUL_1MS` (default), `MUL_10MS`, `MUL_100MS`, `MUL_1S`

**Time macros**: `TIME(5, SECONDS)`, `TIME(2, MINUTES)`

### Global Functions

```cpp
uint32_t millis();                                           // System uptime in ms
void delay(volatile uint32_t ms);                            // Blocking delay (avoid in tasks)
void delay_us(volatile uint32_t us);                         // Microsecond delay
long map(long x, long in_min, long in_max, long out_min, long out_max);  // Linear interpolation
```

---

## Hardware Abstraction Layer

### Design Pattern

All HAL modules follow:
1. **Singleton**: `getInstance(HAL_HandleTypeDef*)` for multi-instance peripherals
2. **Async state machine**: `IDLE → CHECK_FREE → WORK → WAITING → FINISH → CLEAR`
3. **Operation queue**: `std::queue<operation>` for non-blocking DMA/interrupt I/O
4. **Callback completion**: `dataCallback_f` (data + length), `voidCallback_f` (void)
5. **Conditional compilation**: `#ifdef HAL_xxx_MODULE_ENABLED`

### Module Summary

| Module      | Class           | Key Methods                                                |
|------------|-----------------|-------------------------------------------------------------|
| GPIO       | `HardwareGPIO`  | `setup()`, `set()`, `toggle()`, `readInput()`, `attachInterrupt()` |
| UART       | `UART`          | `transmit()`, `onReceiveHandler()`, `onTransmitHandler()`  |
| I2C        | `I2C`           | `transmit()`, `receive()`, `readFromMemory()`, `writeToMemory()` |
| SPI        | `SPI`           | `transmit()`, `receive()`, `transmitThenReceive()`         |
| Timer      | `Timer`         | `attachInterrupt()`, `setPeriod()`                         |
| PWM        | `PWM`           | `set()`, `get()` (inherits Timer)                          |
| Analog     | `Analog`        | `getRawValue()`, `getFilteredValue()`, `getVoltage()`, `attachInterrupt()` |
| CAN        | `CAN`           | `send()`, `onPacket()`                                     |
| FDCAN      | `FDCAN`         | `send()`, `onPacket()`                                     |
| OneWire    | `OneWire`       | `reset()`, `transmit()`, `receive()`, `completeTransaction()` |
| Encoder    | `Encoder`       | `getValue()`, `setValue()`, `setLimits()`, `getDirection()` |
| FlashMemory| `FlashMemory`   | `readFromMemory()`, `writeToMemory()`, `read()`, `write()` |
| USB_CDC    | `USB_CDC`       | `send()`, `onReceiveHandler()`                             |

### Configurable Instance Limits

```cpp
// Override in platformio.ini build_flags or before including Core.h
#define TIMER_MAX_INSTANCES    // default: TIMERS_AVAILABLE
#define I2C_MAX_INSTANCES   2
#define UART_MAX_INSTANCES  2
#define SPI_MAX_INSTANCES   2
#define ANALOG_MAX_INSTANCES 1
#define ANALOG_MAX_CHANNELS  8
#define MAX_GPIO_INTERRUPTS 16
```

---

## Interfaces

### IBus

```cpp
class IBus { };  // Base interface for all bus communication modules
```

### IExternalMemory

```cpp
class IExternalMemory {
    virtual void readFromMemory(uint32_t addr, uint8_t *pData, uint16_t size) = 0;
    virtual void writeToMemory(uint32_t addr, uint8_t *pData, uint16_t size) = 0;
};
// Address size constants: ADDRESS_SIZE_8BIT, ADDRESS_SIZE_16BIT, ADDRESS_SIZE_24BIT, ADDRESS_SIZE_32BIT
```

---

## Device Drivers

| Driver     | Bus     | Constructor                                                     |
|-----------|---------|------------------------------------------------------------------|
| DS18B20   | OneWire | `DS18B20(OneWire *ow)`                                          |
| PCF8583   | I2C     | `PCF8583(I2C *i2c, uint16_t addr=0xA0, uint32_t base=0x00)`    |
| EEP24C04  | I2C     | `EEP24C04(I2C *i2c, uint16_t addr=0xA0, uint32_t base=0x00)`   |
| EEP24C256 | I2C     | `EEP24C256(I2C *i2c, uint16_t addr=0xA0, uint32_t base=0x00)`  |
| FM25V05   | SPI     | `FM25V05(SPI *spi, GPIOx, pin)` — CS pin                        |
| TMC2209   | GPIO    | `TMC2209(enablePort/Pin, stepPort/Pin, dirPort/Pin, MS1/MS2, zeroPort/Pin)` |
| DFR0646   | I2C     | `DFR0646(I2C *i2c, uint8_t addr)`                              |
| SED1520   | I2C     | `SED1520(I2C *i2c)`                                            |

All memory devices implement `IExternalMemory`.

---

## Utilities

### Modbus RTU

```cpp
ModbusSlave modbus;
modbus.setID(slaveIdPtr);
modbus.bind_function(ModbusFunction::FUNC_3, handler);  // Read Holdings (0x03)
modbus.bind_function(ModbusFunction::FUNC_6, handler);  // Write Single (0x06)

void handler(ModbusFrame *request) {
    // request->function, request->address, request->size, request->registers[125]
}
```

### RegisterBank

```cpp
// Persistent (flash/EEPROM/FRAM backed)
RegisterBank bank(startAddr, size, new MemoryBlock(&device, memAddr));

// Volatile (RAM only)
RegisterBank bank(startAddr, size);

// Access: getValue/setValue (relative), getRegister/setRegister (absolute)
// Static lookup: RegisterBank::find(absoluteAddr)
// Bulk read: bank.readRegisters(buffer, addr, count)
```

### Bit Macros (`fl_bit.h`)

```cpp
bf_set(flag, mask)   bf_clr(flag, mask)   bf_is(flag, mask)   bf_mask(flag, mask)
bfb_set(ptr, bit)    bfb_clr(ptr, bit)    bfb_tog(ptr, bit)   bfb_iss(ptr, bit)
XOR(A, B)            ABS(X)               TRUE / FALSE
```

### Math

```cpp
uint16_t CRC16(const uint8_t *data, uint16_t len);   // Modbus CRC-16
uint8_t bcdToDec(uint8_t val);                        // BCD → decimal
uint8_t decToBcd(uint8_t val);                        // Decimal → BCD
float random_float(float min, float max);
int random_int(int min, int max);
```

---

## Build System Integration

### PlatformIO Configuration

```ini
[env:myboard]
platform = ststm32
board = your_stm32_board
framework = stm32cube
lib_deps = https://github.com/JAQUBA/JQBSTM32_Framework.git

# Optional: select framework features
custom_using = FDCAN,CAN,USB_CDC
# Generates: -D_JQB_USE_FDCAN, -D_JQB_USE_CAN, -D_JQB_USE_USB_CDC
```

### `includeCore.py` Script

- Copies project HAL config (`stm32*_hal_conf.h`) to framework
- Adds project `Core/Inc/` to include paths
- Builds CubeMX-generated sources from `Core/Src/`
- Processes `custom_using` to add preprocessor defines
- Configures USB middleware if USB_CDC selected

### `makehex.py` Script

- Post-build: converts ELF → Intel HEX via `objcopy`

---

## Important Constraints

- **No heap at runtime** — `malloc` used only during `init()`/`setup()` for one-time allocations; freed in state machine CLEAR state
- **No exceptions** — Error handling via state machines, timeouts (default 4ms for bus ops), status checks
- **Cooperative scheduling** — Tasks must return promptly; never call `delay()` in task callbacks
- **DMA-driven I/O** — All bus operations are asynchronous; use callbacks for completion notification
- **Volatile shared data** — ISR-shared variables (`millis` counter, task timers) must be `volatile`
- **STM32CubeMX dependency** — Peripheral init code must be generated by CubeMX; framework wraps HAL handles
- **Modbus big-endian** — STM32 is little-endian; Modbus frame building handles byte swapping
- **GPL-3.0 compliance** — All source files must include GPL copyright header; contributions must be GPL-compatible
- **Operation queue pattern** — Bus ops are queued; data buffers are `malloc`'d per-operation and `free`'d after completion
- **Performance monitoring** — `getTaskOverruns()`, `getMaxExecutionTime()` track tasks exceeding 10ms
