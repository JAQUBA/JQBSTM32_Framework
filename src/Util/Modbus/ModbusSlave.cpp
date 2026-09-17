/*
 * JQBSTM32 Framework - Modbus Slave Implementation
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

#include "ModbusSlave.h"
#include "../Math/CRC16.h"

#define MODBUS_FUNC_READ_HOLDING 3U
#define MODBUS_FUNC_WRITE_SINGLE 6U
#define MODBUS_FUNC_WRITE_MULTIPLE 16U
#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION 1U
#define MODBUS_EXCEPTION_ILLEGAL_ADDRESS 2U
#define MODBUS_EXCEPTION_ILLEGAL_VALUE 3U

static void sendException(uint8_t slaveId, uint8_t function, uint8_t exception, dataCallback_f functionPointer) {
    uint8_t buffer[5] = {slaveId, (uint8_t)(function | 0x80U), exception};
    uint16_t len = Modbus::appendCrc(buffer, 3U);
    functionPointer(buffer, len);
}

void ModbusSlave::setID(uint8_t *slaveID) {
    this->_slaveID = slaveID;
}
void ModbusSlave::bind_function(ModbusFunction function, void(*functionPointer)(ModbusFrame *request)) {
    if ((uint8_t)function < 4U) {
        _functionHandlers[function] = functionPointer;
    }
}
void ModbusSlave::receive(uint8_t* data, uint16_t length, dataCallback_f functionPointer) {
    if (data == nullptr || functionPointer == nullptr || _slaveID == nullptr || length < 8U) return;
    const bool broadcast = data[0] == 0U;
    if (!broadcast && data[0] != *_slaveID) return;

    const uint8_t function = data[1];
    if (broadcast && function != MODBUS_FUNC_WRITE_SINGLE && function != MODBUS_FUNC_WRITE_MULTIPLE) return;
    _statistics.requests++;
    if (broadcast) _statistics.broadcasts++;
    if (!validateCrc(data, length)) {
        _statistics.crcErrors++;
        return;
    }

    const bool singleWrite = function == MODBUS_FUNC_WRITE_SINGLE;
    const bool multipleWrite = function == MODBUS_FUNC_WRITE_MULTIPLE;
    const bool fixedLength = function == MODBUS_FUNC_READ_HOLDING || singleWrite;
    if ((fixedLength && length != 8U) ||
        (multipleWrite && (length < 11U || data[6] != (uint8_t)(data[5] * 2U) ||
            length != (uint16_t)(9U + data[6]))) ||
        (!fixedLength && !multipleWrite)) {
        _statistics.malformedFrames++;
        if (!broadcast) {
            _statistics.exceptions++;
            sendException(*_slaveID, function, MODBUS_EXCEPTION_ILLEGAL_VALUE, functionPointer);
        }
        return;
    }

    memset(&_frame, 0, sizeof(_frame));
    _frame.function = (ModbusFunction)function;
    _frame.address = readU16BE(data + 2U);
    _frame.size = readU16BE(data + 4U);

    if (function == MODBUS_FUNC_READ_HOLDING &&
        (_frame.size == 0U || _frame.size > MAX_READ_REGISTERS ||
         (uint32_t)_frame.address + _frame.size > 0x10000UL)) {
        _statistics.malformedFrames++;
        if (!broadcast) {
            _statistics.exceptions++;
            sendException(*_slaveID, function, MODBUS_EXCEPTION_ILLEGAL_VALUE, functionPointer);
        }
        return;
    }
    if (multipleWrite && (_frame.size == 0U || _frame.size > MAX_WRITE_REGISTERS ||
        (uint32_t)_frame.address + _frame.size > 0x10000UL)) {
        _statistics.malformedFrames++;
        if (!broadcast) {
            _statistics.exceptions++;
            sendException(*_slaveID, function, MODBUS_EXCEPTION_ILLEGAL_VALUE, functionPointer);
        }
        return;
    }

    const uint8_t handlerIndex = (function == MODBUS_FUNC_READ_HOLDING)
        ? (uint8_t)ModbusFunction::FUNC_3
        : (multipleWrite ? (uint8_t)ModbusFunction::FUNC_10 : (uint8_t)ModbusFunction::FUNC_6);
    if (_functionHandlers[handlerIndex] == nullptr) {
        _statistics.exceptions++;
        if (!broadcast) sendException(*_slaveID, function, MODBUS_EXCEPTION_ILLEGAL_FUNCTION, functionPointer);
        return;
    }

    if (multipleWrite) {
        for (uint16_t index = 0U; index < _frame.size; index++) {
            _frame.registers[index] = readU16BE(data + 7U + index * 2U);
        }
    }

    _functionHandlers[handlerIndex](&_frame);
    if (_frame.exception != 0U) {
        _statistics.exceptions++;
        if (!broadcast) sendException(*_slaveID, function, _frame.exception, functionPointer);
        return;
    }

    if (broadcast) return;

    uint16_t len = 0;
    _responseBuffer[len++] = (uint8_t)*_slaveID;
    _responseBuffer[len++] = function;

    switch(function) {
        case MODBUS_FUNC_READ_HOLDING: {
            _responseBuffer[len++] = (uint8_t)(_frame.size * 2U);
            for (uint16_t i = 0; i < _frame.size; i++) {
                writeU16BE(_responseBuffer + len, _frame.registers[i]);
                len += 2U;
            }
            break;
        }
        case MODBUS_FUNC_WRITE_SINGLE:
        case MODBUS_FUNC_WRITE_MULTIPLE: {
            writeU16BE(_responseBuffer + len, _frame.address);
            len += 2U;
            writeU16BE(_responseBuffer + len, _frame.size);
            len += 2U;
            break;
        }
    }

    len = appendCrc(_responseBuffer, len);
    functionPointer(_responseBuffer, len);
}
