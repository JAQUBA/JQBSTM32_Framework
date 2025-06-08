# Changelog

All notable changes to the JQBSTM32 Framework project are documented in this file.

Format based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2024-12-08

### Added ✨
- **Core System**
  - Advanced task scheduler with main loop and interrupt support
  - Single-shot and cyclic task management system
  - Optimized memory management

- **Hardware Abstraction Layer**
  - Analog/ADC - A/D converter handling with calibration
  - I2C - Communication with multi-device support and error handling
  - SPI - High-speed SPI communication with DMA
  - UART - RS232/RS485 with buffering and flow control
  - Timer/PWM - Precise timers and PWM generation
  - GPIO - Advanced I/O port control
  - OneWire - OneWire protocol with search algorithm
  - USB CDC - USB virtual serial port
  - CAN/FDCAN - CAN bus for automotive applications
  - Flash Memory - Flash memory management with wear leveling

- **Device Drivers**
  - DS18B20 - Complete OneWire temperature sensor manager
  - EEP24C04/EEP24C256 - I2C EEPROM memory drivers
  - FM25V05 - SPI FRAM memory with fast access
  - PCF8583 - I2C real-time clock
  - SED1520 - Graphic LCD display controller
  - DFR0646 - Display module with fonts
  - TMC2209 - Advanced stepper motor driver

- **Utility Libraries**
  - Modbus RTU/ASCII - Complete protocol implementation
  - RegisterBank - Register bank system with persistence
  - MemoryBlock - Memory block management
  - Menu System - Framework for user interfaces
  - Math Utils - Mathematical functions (CRC16, BCD conversion)
  - PID Controller - PID controller with tuning

- **Documentation & Examples**
  - Complete API documentation in README.md
  - 15+ usage examples with comments
  - Detailed installation and configuration instructions

### Security 🛡️
- All files contain GPL v3.0 copyright headers
- Removed all potential license issues
- Added GPL compliance documentation in ACKNOWLEDGMENTS
- Created NOTICE and CONTRIBUTING.md files
- Automatic license compliance verification scripts

### Documentation 📚
- Completely rewritten README.md with API examples
- Created ACKNOWLEDGMENTS file with standards attribution
- Added contribution guidelines in CONTRIBUTING.md
- Extended documentation in docs/ folder
- Usage examples for all major components

### Quality Assurance ✅
- All implementations checked for plagiarism
- Code optimization for performance and memory usage
- Embedded C++ coding standards enforced
- Removed all unused variables and dead code
- Code review of all source files

## [1.0.0-alpha] - 2024-11-15

### Added
- Initial framework version for testing
- Basic HAL drivers
- Prototype DS18B20 support
- Initial Modbus implementation
- Basic usage examples

---

## Legend

- **Added** ✨ - New features
- **Changed** 🔄 - Changes in existing functions
- **Deprecated** ⚠️ - Functions deprecated in future versions
- **Removed** ❌ - Removed functions
- **Fixed** 🐛 - Fixed bugs
- **Security** 🛡️ - Security fixes
- **Documentation** 📚 - Documentation changes
- **Performance** ⚡ - Performance optimizations

## Support

Questions about the changelog can be directed to:
- GitHub Issues: https://github.com/JAQUBA/JQBSTM32_Framework/issues
- Email: kjakubowski0492@gmail.com

---
JQBSTM32 Framework - Copyright (C) 2024 JAQUBA
