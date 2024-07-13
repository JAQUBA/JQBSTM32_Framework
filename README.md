# JQBSTM32_Framework

![Project Status](https://img.shields.io/badge/status-in%20development-yellow)
![License](https://img.shields.io/badge/license-GPL-blue)

## Project Description

JQBSTM32_Framework is a framework designed to facilitate the development of applications for STM32 microcontrollers. It enables quick and efficient project creation, resource management, and code optimization. The framework includes a set of libraries and tools supporting various aspects of STM32 microcontroller programming.

**Note:** The project is still in development.

## Installation

To install JQBSTM32_Framework, follow these steps:

1. **Add as a submodule**

    ```bash
    git submodule add https://github.com/JAQUBA/JQBSTM32_Framework.git
    cd JQBSTM32_Framework
    ```

2. **Install dependencies**

    - Make sure you have the required tools and libraries installed. Detailed instructions can be found in the [Requirements](#requirements) section.

## Usage

To use JQBSTM32_Framework in your project, follow these steps:

1. **Set up the environment**
    - Install Visual Studio Code and the PlatformIO extension.
    - Configure STM32CubeMX to generate the startup code for your project.

2. **Create a new project**
    - Use the provided scripts and project templates to quickly start working.

3. **Directory structure**
    - Hardware resources are located in the `Hardware` directory.
    - Peripheral libraries are located in the `Utils` directory.

4. **STM32CubeMX**: The project is configured using STM32CubeMX. It can be further customized using this tool.

5. **Visual Studio Code and PlatformIO**: Visual Studio Code with the PlatformIO plugin is required to compile and upload the software to the device.

## Requirements

- **Operating System**: Windows, macOS, Linux
- **Tools**:
    - STM32CubeMX
    - Visual Studio Code with the PlatformIO extension
    - ARM Compiler (e.g., arm-none-eabi-gcc)
    - Debugging tools (e.g., OpenOCD, ST-Link)
- **Libraries**:
    - CMSIS
    - STM32 HAL

## Example Project

### Step 1: STM32CubeMX Configuration

1. Launch STM32CubeMX.
2. Select the microcontroller: `STM32F103C8Tx`.
3. Configure peripherals: e.g., `TIM4`.
4. Project configuration:
   - Name: `STM32Blink`.
   - IDE: `EwARM`.
5. Generate the code.

### Step 2: PlatformIO Configuration

1. Create a new project in PlatformIO.
2. Configure the `platformio.ini` file:

    ```ini
    [platformio]
    src_dir = ./
    include_dir = Core/Inc

    [env:genericSTM32F103C8]
    platform = ststm32
    board = genericSTM32F103C8
    framework = stm32cube
    lib_deps = 
        https://github.com/JAQUBA/JQBSTM32_Framework.git
    build_flags = 
        -DTASK_TIMER=htim4
    ```

## Authors

- **JAQUBA** - main author

## Contact

If you have any questions about the project, you can contact via [GitHub profile](https://github.com/JAQUBA).

## License

This project is licensed under the terms of the GNU General Public License v3.0. More information can be found in the LICENSE file available in this repository.
