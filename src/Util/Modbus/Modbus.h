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

enum ModbusFunction {
    FUNC_3,
    FUNC_4,
    FUNC_6,
    FUNC_10
};
struct ModbusFrame {
    ModbusFunction function;
    uint16_t address;
    uint16_t size;
    uint16_t registers[125];
};

class Modbus {
    public:
        void receive(uint8_t* data, uint16_t length, dataCallback_f functionPointer);
        void bind_function(ModbusFunction function, void(*functionPointer)(ModbusFrame *request));
    protected:
        uint8_t *_slaveID;
};

class ModbusSlave : public Modbus {
    public:
        void setID(uint8_t *slaveID);
    private:
};

#endif