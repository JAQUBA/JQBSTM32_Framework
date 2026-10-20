/*
 * JQBSTM32 Framework - Modbus Protocol Header
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

#ifndef __MODBUS_H_
#define __MODBUS_H_
#include "../../Core.h"

/**
 * @brief Modbus function codes enumeration
 * @details Standard Modbus function codes supported by the implementation
 */
enum ModbusFunction {
    FUNC_1 = 1,    ///< Read Coils (decimal 1, hex 0x01)
    FUNC_2 = 2,    ///< Read Discrete Inputs (decimal 2, hex 0x02)
    FUNC_3 = 3,    ///< Read Holding Registers (decimal 3, hex 0x03)
    FUNC_4 = 4,    ///< Read Input Registers (decimal 4, hex 0x04)
    FUNC_5 = 5,    ///< Write Single Coil (decimal 5, hex 0x05)
    FUNC_6 = 6,    ///< Write Single Register (decimal 6, hex 0x06)
    FUNC_15 = 15,  ///< Write Multiple Coils (decimal 15, hex 0x0F)
    FUNC_16 = 16   ///< Write Multiple Registers (decimal 16, hex 0x10)
};

/**
 * @brief Modbus exception codes enumeration (shared by master and slave)
 */
enum ModbusException {
    EXCEPTION_ILLEGAL_FUNCTION = 1,
    EXCEPTION_ILLEGAL_ADDRESS = 2,
    EXCEPTION_ILLEGAL_VALUE = 3
};

/**
 * @brief Modbus frame structure
 * @details Structure containing parsed Modbus request/response data
 */
struct ModbusFrame {
    ModbusFunction function; ///< Modbus function code
    uint16_t address;        ///< Starting register address
    uint16_t size;           ///< Number of registers
    uint16_t registers[2000]; ///< Register and coil data array
    uint8_t exception;       ///< Modbus exception code, zero when the request is valid
};

struct ModbusStatistics {
    uint32_t requests;
    uint32_t crcErrors;
    uint32_t malformedFrames;
    uint32_t exceptions;
    uint32_t broadcasts;
    uint32_t timeouts; ///< Master-only: requests that received no response in time
};

/**
 * @brief Base Modbus protocol class
 * @details Holds the statistics and framing helpers shared by master and slave implementations
 */
class Modbus {
    public:
        const ModbusStatistics& statistics() const;
        void resetStatistics();

        static uint16_t readU16BE(const uint8_t *p);
        static void writeU16BE(uint8_t *p, uint16_t value);
        static bool validateCrc(const uint8_t *data, uint16_t length);
        static uint16_t appendCrc(uint8_t *buffer, uint16_t length);

        /**
         * @brief Register <-> multi-word value conversion helpers
         * @details Registers are combined big-endian (first register = most significant word),
         * matching the byte order already used for single 16-bit values by readU16BE/writeU16BE.
         * Pass wordSwap = true for devices that transmit the words in swapped (little-endian) order.
         */
        static uint32_t registersToUint32(const uint16_t *registers, bool wordSwap = false);
        static int32_t registersToInt32(const uint16_t *registers, bool wordSwap = false);
        static float registersToFloat(const uint16_t *registers, bool wordSwap = false);
        static double registersToDouble(const uint16_t *registers, bool wordSwap = false);
        static void uint32ToRegisters(uint32_t value, uint16_t *registers, bool wordSwap = false);
        static void floatToRegisters(float value, uint16_t *registers, bool wordSwap = false);
        static void doubleToRegisters(double value, uint16_t *registers, bool wordSwap = false);
    protected:
        ModbusStatistics _statistics = {};
};

#endif