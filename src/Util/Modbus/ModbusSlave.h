/*
 * JQBSTM32 Framework - Modbus Slave Header
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

#ifndef __MODBUS_SLAVE_H_
#define __MODBUS_SLAVE_H_
#include <initializer_list>
#include "Modbus.h"

/**
 * @brief Modbus Slave implementation
 * @details Implements Modbus slave functionality for responding to master requests
 */
class ModbusSlave : public Modbus {
    public:
        static constexpr uint16_t MAX_READ_COILS = 2000U;
        static constexpr uint16_t MAX_WRITE_COILS = 1968U;
        static constexpr uint16_t MAX_READ_REGISTERS = 125U;
        static constexpr uint16_t MAX_WRITE_REGISTERS = 123U;
        static constexpr uint16_t MAX_RESPONSE_BYTES = 3U + MAX_READ_COILS / 8U + 2U;

        /**
         * @brief Set slave ID
         * @details Sets the slave ID for this Modbus slave instance
         * @param slaveID Pointer to slave ID value
         */
        void setID(uint8_t *slaveID);

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

        /**
         * @brief Bind one handler to multiple function codes
         * @param functions Modbus function codes to bind
         * @param functionPointer Handler function for all specified function codes
         */
        void bind_function(std::initializer_list<ModbusFunction> functions, void(*functionPointer)(ModbusFrame *request));
    private:
        uint8_t *_slaveID = nullptr;
        void (*_functionHandlers[8])(ModbusFrame *request) = {};
        ModbusFrame _frame = {};
        uint8_t _responseBuffer[MAX_RESPONSE_BYTES] = {};
};

#endif
