# JQBSTM32 Framework

![Version](https://img.shields.io/badge/version-v1.0.0-brightgreen)
![License](https://img.shields.io/badge/license-GPL--3.0--or--later-blue)
![Platform](https://img.shields.io/badge/platform-STM32-green)
![Framework](https://img.shields.io/badge/framework-PlatformIO-orange)
![Build](https://img.shields.io/badge/build-passing-brightgreen)
![Quality](https://img.shields.io/badge/code%20quality-A+-brightgreen)

## Description

**JQBSTM32 Framework v1.0** is an advanced, production-ready library designed for rapid and efficient development of applications for STM32 microcontrollers. The framework provides comprehensive tools for hardware resource management, peripheral device handling, and code optimization.

### ✨ Key Features
- **🎯 Stable API** - Ready for use in production projects
- **⚡ High Performance** - Optimized algorithms and memory management
- **🔧 Modularity** - Use only the components you need
- **📚 Complete Documentation** - Detailed examples and API reference
- **🛡️ GPL v3.0 Compliance** - Legally verified implementation
- **🔄 Easy Integration** - Works with STM32CubeMX and PlatformIO

## Features

### 🔧 Hardware Abstraction Layer (HAL)
- **Analog/ADC** - Analog-to-digital converter handling with calibration
- **I2C** - I2C communication with multi-device support
- **SPI** - SPI interface for high-speed communication
- **UART** - Serial communication with RS232/RS485 support
- **Timer/PWM** - Timer management and PWM generation
- **GPIO** - I/O port control
- **OneWire** - OneWire protocol for temperature sensors
- **USB CDC** - USB communication as virtual serial port
- **CAN/FDCAN** - CAN bus for automotive systems
- **Flash Memory** - Internal flash memory management

### 📟 Supported Devices
- **DS18B20** - OneWire digital temperature sensors
- **EEP24C04/EEP24C256** - I2C EEPROM memories
- **FM25V05** - SPI FRAM memory
- **PCF8583** - I2C real-time clock
- **SED1520** - LCD display controller
- **DFR0646** - Display module
- **TMC2209** - Stepper motor driver

### 🛠️ Utility Tools
- **Modbus** - Modbus RTU/ASCII communication protocol
- **RegisterBank** - Register bank system for configuration data
- **MemoryBlock** - Memory block management
- **Menu System** - Menu system for user interfaces
- **PID Controller** - PID controller
- **Math Utils** - Mathematical functions
- **Scheduler** - Advanced task scheduler

## Requirements

### Hardware
- **Microcontrollers**: STM32 (almost all series)
- **Tools**: STM32CubeMX for pin configuration

### Software
- **Platform**: PlatformIO
- **IDE**: Visual Studio Code with PlatformIO extension
- **Framework**: STM32Cube HAL
- **Compiler**: GCC ARM

### Operating Systems
- Windows 10/11
- macOS 10.15+
- Linux (Ubuntu 18.04+)

## Installation

### 1. Via PlatformIO Registry
```ini
# platformio.ini
[env:your_board]
platform = ststm32
board = your_stm32_board
framework = stm32cube
lib_deps = 
    https://github.com/JAQUBA/JQBSTM32_Framework.git
```

### 2. Local Installation
```bash
git submodule add https://github.com/JAQUBA/JQBSTM32_Framework.git lib/JQBSTM32
git submodule update --init --recursive
```


## Technical Support

### Bug Reports
Issues can be reported through [GitHub Issues](https://github.com/JAQUBA/JQBSTM32_Framework/issues)

### Discussions
Questions and discussions: [GitHub Discussions](https://github.com/JAQUBA/JQBSTM32_Framework/discussions)

### Documentation
Detailed documentation available in the `docs/` folder and on [GitHub Wiki](https://github.com/JAQUBA/JQBSTM32_Framework/wiki)

## Contributing

We welcome contributions! See [CONTRIBUTING.md](CONTRIBUTING.md) to learn how to:
- Report bugs
- Propose new features
- Create pull requests
- Write documentation

## License

This project is licensed under the **GNU General Public License v3.0** or later.
See the [LICENSE](LICENSE) file for detailed information.

### GPL v3.0 Compliance
- All source files contain appropriate copyright headers
- Used standard algorithms are properly documented
- Framework can be freely used, modified and distributed according to GPL terms
- See [ACKNOWLEDGMENTS](ACKNOWLEDGMENTS) file for information about used standards

### Note for Developers
If you use this framework in your project, remember that GPL v3.0 requires:
- Making your application source code available
- Keeping the GPL v3.0 license for the entire project
- Including license and copyright information

More information: https://www.gnu.org/licenses/gpl-3.0.html

## Authors

- **JAQUBA** - Main author and maintainer
  - GitHub: [@JAQUBA](https://github.com/JAQUBA)
  - Email: kjakubowski0492@gmail.com

**JQBSTM32 Framework** - Accelerate your STM32 project development! 🚀
