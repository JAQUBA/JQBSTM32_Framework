/*
 * JQBSTM32 Framework - Modbus Master Header
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

#ifndef __MODBUS_MASTER_H_
#define __MODBUS_MASTER_H_
#include "Modbus.h"
#include "../../Hardware/UART/UART.h"

/**
 * @brief Modbus Master implementation
 * @details Polls a single Modbus RTU slave device at a time (Read Holding Registers only)
 * @note Only one request may be in flight; call isBusy() before issuing a new one
 */
class ModbusMaster : public Modbus {
    public:
        using ResponseCallback = std::function<void(bool success, uint8_t exception, uint16_t *registers, uint16_t count)>;

        explicit ModbusMaster(UART *bus);

        bool readHoldingRegisters(uint8_t slaveId, uint16_t address, uint16_t count,
                                   ResponseCallback callback, uint32_t timeoutMs = 200U);

        /**
         * @brief Feed a received frame into the pending request state machine
         * @details Must be wired manually to the bus' onReceiveHandler by the application
         */
        void onReceive(uint8_t *data, uint16_t length);

        bool isBusy() const;
    private:
        static constexpr uint16_t MaxReadRegisters = 125U;
        UART *_bus;
        enum State {IDLE, WAITING} _state = IDLE;
        ResponseCallback _callback;
        uint8_t _expectedSlaveId = 0U;
        uint8_t _expectedFunction = 0U;
        uint16_t _expectedCount = 0U;
        uint32_t _timeoutAt = 0U;
        uint16_t _responseLength = 0U;
        uint8_t _responseBuffer[(MaxReadRegisters * 2U) + 5U] = {};

        void _finish(bool success, uint8_t exception, uint16_t *registers, uint16_t count);
};

#endif
