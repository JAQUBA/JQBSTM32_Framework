/*
 * JQBSTM32 Framework - Modbus Protocol Header
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * Implementation of Modbus RTU protocol according to official
 * Modbus specification (www.modbus.org).
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
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
};

/**
 * @brief Base Modbus protocol class
 * @details Provides common Modbus functionality for both master and slave implementations
 */
class Modbus {
    public:
        /**
         * @brief Process received Modbus data
         * @details Parses received data and calls appropriate function handler
         * @param data Pointer to received data buffer
         * @param length Length of received data
         * @param functionPointer Callback function for processing parsed frame
         */
        void receive(uint8_t* data, uint16_t length, dataCallback_f functionPointer);
        
        /**
         * @brief Bind function handler
         * @details Associates a function code with a handler function
         * @param function Modbus function code to bind
         * @param functionPointer Handler function for the specified function code
         */
        void bind_function(ModbusFunction function, void(*functionPointer)(ModbusFrame *request));
    protected:
        uint8_t *_slaveID; ///< Slave ID pointer (for slave implementations)
};

/**
 * @brief Modbus Slave implementation
 * @details Implements Modbus slave functionality for responding to master requests
 */
class ModbusSlave : public Modbus {
    public:
        /**
         * @brief Set slave ID
         * @details Sets the slave ID for this Modbus slave instance
         * @param slaveID Pointer to slave ID value
         */
        void setID(uint8_t *slaveID);
    private:
};

#endif