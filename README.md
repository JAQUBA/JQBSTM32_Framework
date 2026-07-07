# JQBSTM32 Framework

[![Version](https://img.shields.io/badge/version-v1.0.0-brightgreen)](https://github.com/JAQUBA/JQBSTM32_Framework/releases)
[![License](https://img.shields.io/badge/license-LGPL--3.0--or--later-blue)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-STM32-green)](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html)
[![Framework](https://img.shields.io/badge/framework-PlatformIO-orange)](https://platformio.org)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/jaquba/library/JQBSTM32.svg)](https://registry.platformio.org/libraries/jaquba/JQBSTM32)

> **Production-ready C++ framework for rapid STM32 embedded application development with PlatformIO and STM32CubeMX.**

JQBSTM32 Framework v1.0 is a comprehensive, modular library designed for the STM32 family of ARM Cortex-M microcontrollers. It provides hardware abstraction layers, device drivers, and utility libraries so you can focus on your application logic instead of low-level peripheral code.

---

## Table of Contents

- [Key Features](#-key-features)
- [Supported Hardware](#-supported-hardware)
  - [STM32 Families](#stm32-families)
  - [HAL Peripherals](#hardware-abstraction-layer-hal)
  - [Device Drivers](#device-drivers)
  - [Utility Libraries](#utility-libraries)
- [Requirements](#-requirements)
- [Installation](#-installation)
- [Quick Start](#-quick-start)
- [Documentation](#-documentation)
- [Contributing](#-contributing)
- [License](#-license)
- [Authors](#-authors)

---

## ✨ Key Features

| Feature | Description |
|---|---|
| 🎯 **Stable API** | Ready for production projects |
| ⚡ **High Performance** | Optimized algorithms and memory management |
| 🔧 **Modularity** | Use only the components you need |
| 📚 **Documentation** | Detailed examples and API reference |
| 🛡️ **LGPL v3.0** | Use in proprietary projects without GPL obligations |
| 🔄 **Easy Integration** | Works seamlessly with STM32CubeMX and PlatformIO |

---

## 🔩 Supported Hardware

### STM32 Families

Works with virtually all STM32 series supported by STM32Cube HAL:

- **STM32F0 / STM32F1 / STM32F2 / STM32F3 / STM32F4 / STM32F7**
- **STM32G0 / STM32G4**
- **STM32H7**
- **STM32L0 / STM32L1 / STM32L4 / STM32L5**
- **STM32U5 / STM32WB / STM32WL**

### Hardware Abstraction Layer (HAL)

| Module | Description |
|---|---|
| **Analog / ADC** | A/D converter with multi-channel support and calibration |
| **I2C** | I2C bus with multi-device support and error recovery |
| **SPI** | High-speed SPI with DMA support |
| **UART** | RS232/RS485 with buffering and flow control |
| **Timer / PWM** | Precise timers and PWM signal generation |
| **GPIO** | Advanced I/O port control |
| **OneWire** | OneWire protocol with device search algorithm |
| **USB CDC** | USB virtual serial port (VCP) |
| **CAN / FDCAN** | CAN bus for automotive and industrial applications |
| **Flash Memory** | Internal flash management with wear leveling |

### Device Drivers

| Driver | Interface | Description |
|---|---|---|
| **DS18B20** | OneWire | Digital temperature sensor manager |
| **EEP24C04** | I2C | 4 Kbit EEPROM memory |
| **EEP24C256** | I2C | 256 Kbit EEPROM memory |
| **FM25V05** | SPI | 512 Kbit FRAM memory |
| **PCF8583** | I2C | Real-time clock (RTC) |
| **SED1520** | SPI | Graphic LCD display controller |
| **DFR0646** | I2C | Display module with font support |
| **TMC2209** | UART | Stepper motor driver |

### Utility Libraries

| Module | Description |
|---|---|
| **Modbus RTU/ASCII** | Complete Modbus protocol implementation |
| **Scheduler** | Cooperative task scheduler with ISR support |
| **RegisterBank** | Configuration register bank with persistence |
| **MemoryBlock** | Memory block allocator |
| **Menu System** | Hierarchical menu framework for embedded UIs |
| **Math Utils** | CRC16, BCD conversion, fixed-point math |

---

## 📋 Requirements

### Hardware
- Any **STM32** microcontroller (see supported families above)
- **ST-Link** programmer/debugger

### Software
- **[PlatformIO](https://platformio.org)** (recommended) or STM32CubeIDE
- **[STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)** for pin and clock configuration
- **Visual Studio Code** with PlatformIO extension
- **GCC ARM** toolchain (installed automatically by PlatformIO)

### Operating Systems
- Windows 10/11
- macOS 10.15+
- Linux (Ubuntu 18.04+, Debian, Arch, etc.)

---

## 📦 Installation

### Option 1 — PlatformIO Registry (recommended)

Add the library to your `platformio.ini`:

```ini
[env:your_board]
platform = ststm32
board = nucleo_f446re      ; replace with your board
framework = stm32cube
lib_deps =
    jaquba/JQBSTM32
```

### Option 2 — Git submodule

```bash
git submodule add https://github.com/JAQUBA/JQBSTM32_Framework.git lib/JQBSTM32
git submodule update --init --recursive
```

<<<<<<< HEAD
## Quick Start

```cpp
#include "Core.h"
#include "Hardware/GPIO/GPIO.h"

#define LED GPIOC, GPIO_PIN_13

void init() {
    INIT(GPIO);  // Calls MX_GPIO_Init() generated by CubeMX
    GPIO.setup(LED, GPIO_MODE_OUTPUT_PP);
}

void setup() {
    GPIO.set(LED, HIGH);
    addTaskMain(taskCallback {
        GPIO.toggle(LED);
    }, 500);  // Toggle LED every 500ms
}
```

Three functions to implement:
- **`init()`** — CubeMX peripheral initialization (`MX_xxx_Init()`)
- **`setup()`** — Application setup, register tasks
- **`loop()`** — Optional main loop (prefer tasks)

=======
Then reference the library path in `platformio.ini`:
>>>>>>> origin/master

```ini
lib_extra_dirs = lib
```

### Option 3 — Direct URL

```ini
lib_deps =
    https://github.com/JAQUBA/JQBSTM32_Framework.git
```

<<<<<<< HEAD
### Documentation
Detailed documentation available in the `docs/` folder:

| Document | Description |
|----------|-------------|
| [Core & Scheduler](docs/Core.md) | Entry points, task system, global functions, bit macros, HAL callbacks |
| [Hardware Abstraction](docs/Hardware.md) | GPIO, UART, I2C, SPI, Timer, PWM, ADC, CAN, FDCAN, OneWire, Encoder, Flash, USB_CDC |
| [Device Drivers](docs/Devices.md) | DS18B20, PCF8583, EEP24C04, EEP24C256, FM25V05, TMC2209, DFR0646, SED1520 |
| [Utilities & Interfaces](docs/Utilities.md) | Modbus RTU, RegisterBank, MemoryBlock, SimpleMenu, Math, Interfaces |
| [Build System](docs/BuildSystem.md) | PlatformIO integration, CubeMX setup, scripts, bootloader support |

Also see [GitHub Wiki](https://github.com/JAQUBA/JQBSTM32_Framework/wiki)
=======
---
>>>>>>> origin/master

## 🚀 Quick Start

### 1. Configure your STM32 project with STM32CubeMX

Generate initialization code (HAL-based) for your target board.

### 2. Include the framework

```cpp
#include <Core.h>
```

### 3. Example — Blink LED with the Scheduler

```cpp
#include <Core.h>

void setup() {
    // Called once at startup
}

void loop() {
    // Called repeatedly — cooperative scheduler handles task execution
}
```

See the [`examples/`](examples/) directory for more complete usage examples.

---

## 📚 Documentation

| Resource | Link |
|---|---|
| Examples | [`examples/`](examples/) directory |
| Changelog | [`CHANGELOG.md`](CHANGELOG.md) |
| Contributing Guide | [`CONTRIBUTING.md`](CONTRIBUTING.md) |
| GitHub Wiki | [JQBSTM32_Framework Wiki](https://github.com/JAQUBA/JQBSTM32_Framework/wiki) |
| Bug Reports | [GitHub Issues](https://github.com/JAQUBA/JQBSTM32_Framework/issues) |
| Discussions | [GitHub Discussions](https://github.com/JAQUBA/JQBSTM32_Framework/discussions) |

---

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) to learn how to:

- Report bugs
- Propose new features
- Submit pull requests
- Write documentation

---

## 📄 License

This project is licensed under the **GNU Lesser General Public License v3.0 or later**.  
See the [LICENSE](LICENSE) file for details.

**What LGPL v3.0 means for you:**
- ✅ You can use this library in **proprietary/closed-source projects** without any GPL obligations
- ✅ You can freely use, modify, and distribute the library
- ⚠️ Modifications to the library itself must be released under LGPL v3.0
- ℹ️ See [ACKNOWLEDGMENTS](ACKNOWLEDGMENTS) for attribution of included standards

More information: https://www.gnu.org/licenses/lgpl-3.0.html

---

## 👤 Authors

- **JAQUBA** — Main author and maintainer
  - GitHub: [@JAQUBA](https://github.com/JAQUBA)
  - Email: kjakubowski0492@gmail.com

---

*JQBSTM32 Framework — Accelerate your STM32 embedded development* 🚀

