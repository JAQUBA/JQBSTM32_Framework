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
    FUNC_3,  ///< Read Holding Registers (0x03)
    FUNC_4,  ///< Read Input Registers (0x04)
    FUNC_6,  ///< Write Single Register (0x06)
    FUNC_10  ///< Write Multiple Registers (0x10)
};

/**
 * @brief Modbus frame structure
 * @details Structure containing parsed Modbus request/response data
 */
struct ModbusFrame {
    ModbusFunction function; ///< Modbus function code
    uint16_t address;        ///< Starting register address
    uint16_t size;           ///< Number of registers
    uint16_t registers[125]; ///< Register data array (max 125 registers)
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
    protected:
        ModbusStatistics _statistics = {};
};

#endif