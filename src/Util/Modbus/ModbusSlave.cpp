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

// Local aliases for readability; values are the single source of truth in Modbus.h
static constexpr uint8_t MODBUS_FUNC_READ_COILS = ModbusFunction::FUNC_1;
static constexpr uint8_t MODBUS_FUNC_READ_DISCRETE = ModbusFunction::FUNC_2;
static constexpr uint8_t MODBUS_FUNC_READ_HOLDING = ModbusFunction::FUNC_3;
static constexpr uint8_t MODBUS_FUNC_READ_INPUT = ModbusFunction::FUNC_4;
static constexpr uint8_t MODBUS_FUNC_WRITE_SINGLE_COIL = ModbusFunction::FUNC_5;
static constexpr uint8_t MODBUS_FUNC_WRITE_SINGLE = ModbusFunction::FUNC_6;
static constexpr uint8_t MODBUS_FUNC_WRITE_MULTIPLE_COILS = ModbusFunction::FUNC_15;
static constexpr uint8_t MODBUS_FUNC_WRITE_MULTIPLE = ModbusFunction::FUNC_16;
static constexpr uint8_t MODBUS_EXCEPTION_ILLEGAL_FUNCTION = ModbusException::EXCEPTION_ILLEGAL_FUNCTION;
static constexpr uint8_t MODBUS_EXCEPTION_ILLEGAL_VALUE = ModbusException::EXCEPTION_ILLEGAL_VALUE;

static void sendException(uint8_t slaveId, uint8_t function, uint8_t exception, dataCallback_f functionPointer) {
    uint8_t buffer[5] = {slaveId, (uint8_t)(function | 0x80U), exception};
    uint16_t len = Modbus::appendCrc(buffer, 3U);
    functionPointer(buffer, len);
}

static uint8_t functionHandlerIndex(ModbusFunction function) {
    switch (function) {
        case ModbusFunction::FUNC_1: return 0U;
        case ModbusFunction::FUNC_2: return 1U;
        case ModbusFunction::FUNC_3: return 2U;
        case ModbusFunction::FUNC_4: return 3U;
        case ModbusFunction::FUNC_5: return 4U;
        case ModbusFunction::FUNC_6: return 5U;
        case ModbusFunction::FUNC_15: return 6U;
        case ModbusFunction::FUNC_16: return 7U;
        default: return 0xFFU;
    }
}

void ModbusSlave::setID(uint8_t *slaveID) {
    this->_slaveID = slaveID;
}
void ModbusSlave::bind_function(ModbusFunction function, void(*functionPointer)(ModbusFrame *request)) {
    const uint8_t handlerIndex = functionHandlerIndex(function);
    if (handlerIndex < 8U) {
        _functionHandlers[handlerIndex] = functionPointer;
    }
}
void ModbusSlave::bind_function(std::initializer_list<ModbusFunction> functions, void(*functionPointer)(ModbusFrame *request)) {
    for (const ModbusFunction function : functions) {
        bind_function(function, functionPointer);
    }
}
void ModbusSlave::receive(uint8_t* data, uint16_t length, dataCallback_f functionPointer) {
    if (data == nullptr || functionPointer == nullptr || _slaveID == nullptr || length < 8U) return;
    const bool broadcast = data[0] == 0U;
    if (!broadcast && data[0] != *_slaveID) return;

    const uint8_t function = data[1];
    if (broadcast && function != MODBUS_FUNC_WRITE_SINGLE_COIL &&
        function != MODBUS_FUNC_WRITE_SINGLE && function != MODBUS_FUNC_WRITE_MULTIPLE_COILS &&
        function != MODBUS_FUNC_WRITE_MULTIPLE) return;
    _statistics.requests++;
    if (broadcast) _statistics.broadcasts++;
    if (!validateCrc(data, length)) {
        _statistics.crcErrors++;
        return;
    }

    const bool singleWriteCoil = function == MODBUS_FUNC_WRITE_SINGLE_COIL;
    const bool singleWrite = function == MODBUS_FUNC_WRITE_SINGLE;
    const bool multipleWriteCoils = function == MODBUS_FUNC_WRITE_MULTIPLE_COILS;
    const bool multipleWrite = function == MODBUS_FUNC_WRITE_MULTIPLE;
    const bool fixedLength = function == MODBUS_FUNC_READ_COILS ||
        function == MODBUS_FUNC_READ_DISCRETE ||
        function == MODBUS_FUNC_READ_HOLDING || function == MODBUS_FUNC_READ_INPUT ||
        singleWriteCoil || singleWrite;
    if ((fixedLength && length != 8U) ||
        ((multipleWrite || multipleWriteCoils) && (length < 10U ||
            data[6] != (uint8_t)(multipleWriteCoils ? (readU16BE(data + 4U) + 7U) / 8U : readU16BE(data + 4U) * 2U) ||
            length != (uint16_t)(9U + data[6]))) ||
        (!fixedLength && !multipleWrite && !multipleWriteCoils)) {
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

    if ((function == MODBUS_FUNC_READ_COILS || function == MODBUS_FUNC_READ_DISCRETE) &&
        (_frame.size == 0U || _frame.size > MAX_READ_COILS ||
         (uint32_t)_frame.address + _frame.size > 0x10000UL)) {
        _statistics.malformedFrames++;
        if (!broadcast) {
            _statistics.exceptions++;
            sendException(*_slaveID, function, MODBUS_EXCEPTION_ILLEGAL_VALUE, functionPointer);
        }
        return;
    }
    if ((function == MODBUS_FUNC_READ_HOLDING || function == MODBUS_FUNC_READ_INPUT) &&
        (_frame.size == 0U || _frame.size > MAX_READ_REGISTERS ||
        (uint32_t)_frame.address + _frame.size > 0x10000UL)) {
        _statistics.malformedFrames++;
        if (!broadcast) {
            _statistics.exceptions++;
            sendException(*_slaveID, function, MODBUS_EXCEPTION_ILLEGAL_VALUE, functionPointer);
        }
        return;
    }
    if (singleWriteCoil && readU16BE(data + 4U) != 0xFF00U && readU16BE(data + 4U) != 0U) {
        _statistics.malformedFrames++;
        if (!broadcast) {
            _statistics.exceptions++;
            sendException(*_slaveID, function, MODBUS_EXCEPTION_ILLEGAL_VALUE, functionPointer);
        }
        return;
    }
    if (multipleWriteCoils && (_frame.size == 0U || _frame.size > MAX_WRITE_COILS ||
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

    const uint8_t handlerIndex = functionHandlerIndex(_frame.function);
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
    if (singleWriteCoil) {
        _frame.size = 1U;
        _frame.registers[0] = readU16BE(data + 4U) == 0xFF00U ? 1U : 0U;
    }
    if (multipleWriteCoils) {
        for (uint16_t index = 0U; index < _frame.size; index++) {
            _frame.registers[index] = (data[7U + index / 8U] >> (index % 8U)) & 1U;
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
        case MODBUS_FUNC_READ_COILS:
        case MODBUS_FUNC_READ_DISCRETE: {
            _responseBuffer[len++] = (uint8_t)((_frame.size + 7U) / 8U);
            memset(_responseBuffer + len, 0, (_frame.size + 7U) / 8U);
            for (uint16_t i = 0U; i < _frame.size; i++) {
                if (_frame.registers[i] != 0U) _responseBuffer[len + i / 8U] |= 1U << (i % 8U);
            }
            len += (_frame.size + 7U) / 8U;
            break;
        }
        case MODBUS_FUNC_READ_HOLDING:
        case MODBUS_FUNC_READ_INPUT: {
            _responseBuffer[len++] = (uint8_t)(_frame.size * 2U);
            for (uint16_t i = 0; i < _frame.size; i++) {
                writeU16BE(_responseBuffer + len, _frame.registers[i]);
                len += 2U;
            }
            break;
        }
        case MODBUS_FUNC_WRITE_SINGLE_COIL:
        case MODBUS_FUNC_WRITE_SINGLE:
        case MODBUS_FUNC_WRITE_MULTIPLE_COILS:
        case MODBUS_FUNC_WRITE_MULTIPLE: {
            writeU16BE(_responseBuffer + len, _frame.address);
            len += 2U;
            writeU16BE(_responseBuffer + len,
                function == MODBUS_FUNC_WRITE_SINGLE_COIL
                    ? (_frame.registers[0] != 0U ? 0xFF00U : 0U)
                    : _frame.size);
            len += 2U;
            break;
        }
    }

    len = appendCrc(_responseBuffer, len);
    functionPointer(_responseBuffer, len);
}
