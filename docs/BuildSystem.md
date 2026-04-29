# Build System & Integration

## Overview

The JQBSTM32 Framework integrates with **PlatformIO** and **STM32CubeMX** through custom build scripts. This document covers installation, configuration, and the build pipeline.

---

## Installation

### Via PlatformIO (GitHub)

```ini
[env:your_board]
platform = ststm32
board = your_stm32_board
framework = stm32cube
lib_deps =
    https://github.com/JAQUBA/JQBSTM32_Framework.git
```

### Local (Git Submodule)

```bash
git submodule add https://github.com/JAQUBA/JQBSTM32_Framework.git lib/JQBSTM32
git submodule update --init --recursive
```

### Library Configuration (`library.json`)

```json
{
    "name": "JQBSTM32",
    "version": "1.0.0",
    "license": "GPL-3.0-or-later",
    "frameworks": "stm32cube",
    "platforms": "ststm32",
    "build": {
        "libArchive": false,
        "libLDFMode": "deep+",
        "extraScript": "scripts/includeCore.py"
    }
}
```

Key settings:
- `libArchive: false` — Sources compiled directly (not archived)
- `libLDFMode: "deep+"` — Deep dependency resolution with recursive scanning
- `extraScript` — Runs `includeCore.py` during build

---

## Build Scripts

### `includeCore.py` — PlatformIO Integration

This script runs automatically during the PlatformIO build. It performs:

1. **HAL Configuration Copy**  
   Copies the project's custom `stm32*xx_hal_conf.h` from `Core/Inc/` to the STM32Cube framework package, overwriting the default. This ensures CubeMX-generated HAL settings (enabled modules, clock config) are used.

2. **Include Path Setup**  
   Adds `Core/Inc/` to the compiler's include path so framework code can find project-specific HAL headers.

3. **CubeMX Source Compilation**  
   Registers `Core/Src/*.c` as additional build sources. These are the CubeMX-generated peripheral initialization files.

4. **Feature Selection** (`custom_using`)  
   Processes the `custom_using` option from `platformio.ini` to enable optional framework features:

   | `custom_using` value | Define generated       | Additional actions                    |
   |---------------------|------------------------|---------------------------------------|
   | `FDCAN`              | `-D_JQB_USE_FDCAN`    | —                                     |
   | `CAN`                | `-D_JQB_USE_CAN`      | —                                     |
   | `USB_CDC`            | `-D_JQB_USE_USB_CDC`  | Adds USB_Device include paths & sources |

### Configuration Example

```ini
[env:myboard]
platform = ststm32
board = nucleo_g071rb
framework = stm32cube

lib_deps =
    https://github.com/JAQUBA/JQBSTM32_Framework.git

# Enable CAN and USB CDC
custom_using = CAN, USB_CDC
```

### `makehex.py` — HEX File Generation

Post-build script that converts the ELF binary to Intel HEX format:

```
objcopy -O ihex -R .eeprom firmware.elf firmware.hex
```

Add to `platformio.ini`:
```ini
extra_scripts = post:lib/JQBSTM32/scripts/makehex.py
```

---

## Project Structure Requirements

For the framework to work correctly, the project must follow this structure:

```
project/
├── Core/
│   ├── Inc/           ← CubeMX generated headers (stm32*_hal_conf.h, main.h, etc.)
│   └── Src/           ← CubeMX generated sources (gpio.c, usart.c, etc.)
├── lib/
│   └── JQBSTM32/     ← Framework library
├── src/
│   ├── main.cpp       ← User code: init(), setup(), loop()
│   └── ...
├── platformio.ini
└── *.ioc              ← STM32CubeMX project file
```

### CubeMX Configuration

When configuring the STM32CubeMX project:

1. **Project Manager → Project**:
   - Application Structure: **Advanced**
   - Check: **Do not generate the main()**
   - Toolchain/IDE: **EWARM** (for compatibility with PIO)

2. **Project Manager → Code Generator**:
   - **Generate peripheral initialization as a pair of .c/.h files per peripheral** ✓
   - **Keep User Code when re-generating** ✓

3. **Pinout & Configuration**:
   - Enable desired peripherals (GPIO, USART, I2C, SPI, TIM, ADC, etc.)
   - Configure NVIC interrupts for peripherals used with the framework
   - Enable DMA for bus peripherals (UART, I2C, SPI, ADC)

### Peripheral Init Macro

The framework defines the `INIT()` macro for calling CubeMX-generated init functions:

```cpp
// In init.cpp
void init() {
    INIT(GPIO);          // → MX_GPIO_Init()
    INIT(DMA);           // → MX_DMA_Init()
    INIT(USART1_UART);   // → MX_USART1_UART_Init()
    INIT(I2C2);          // → MX_I2C2_Init()
    INIT(TIM7);          // → MX_TIM7_Init()
    INIT(ADC1);          // → MX_ADC1_Init()
}
```

---

## Configurable Limits

Override these defines in `platformio.ini` `build_flags` or in a header before including `Core.h`:

```ini
build_flags =
    -DUART_MAX_INSTANCES=3
    -DI2C_MAX_INSTANCES=3
    -DSPI_MAX_INSTANCES=2
    -DTIMER_MAX_INSTANCES=8
    -DANALOG_MAX_INSTANCES=2
    -DANALOG_MAX_CHANNELS=16
    -DMAX_GPIO_INTERRUPTS=8
```

---

## Bootloader Support

For projects with a bootloader, use a custom linker script to partition flash:

```ld
MEMORY {
    RAM    (xrw) : ORIGIN = 0x20000000, LENGTH = 36K
    FLASH  (rx)  : ORIGIN = 0x08000000, LENGTH = 122K
    BOOT   (rx)  : ORIGIN = 0x0801E800, LENGTH = 4K
    CONFIG (rx)  : ORIGIN = 0x0801F800, LENGTH = 2K
}
```

### Build Environments

```ini
[env:application]
; source_filter excludes bootloader.c and config.cpp

[env:bootloader]
; source_filter includes only bootloader.c
; custom linker flags for .bootloader_text section

[env:combined]
; Merges both into single binary
```

---

## Blink Example

Minimal working example with the framework:

### `platformio.ini`

```ini
[env:genericSTM32F103C8]
platform = ststm32
board = genericSTM32F103C8
framework = stm32cube
lib_deps =
    https://github.com/JAQUBA/JQBSTM32_Framework.git
```

### `src/main.cpp`

```cpp
#include "Core.h"
#include "Hardware/GPIO/GPIO.h"

#define LED GPIOC, GPIO_PIN_13

void init() {
    INIT(GPIO);  // Calls MX_GPIO_Init() — enables GPIO clocks
    GPIO.setup(LED, GPIO_MODE_OUTPUT_PP);
}

void setup() {
    GPIO.set(LED, HIGH);
    addTaskMain(taskCallback {
        GPIO.toggle(LED);
        task->delay++;  // Gradually slow down
    }, 200);
}
```

### What happens:

1. `init()` — Enables GPIO clocks via `MX_GPIO_Init()`, configures PC13 as push-pull output
2. `setup()` — Turns LED on, registers a 200ms periodic task
3. Task toggles LED every cycle, increasing delay by 1 tick each time
4. `loop()` — Not defined (optional, defaults to empty)
