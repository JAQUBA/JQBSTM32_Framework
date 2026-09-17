/*
 * JQBSTM32 Framework - Modbus Master Implementation
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

#include "ModbusMaster.h"
#include "../Math/CRC16.h"

#define MODBUS_FUNC_READ_HOLDING 3U
#define MODBUS_MAX_READ_REGISTERS 125U
#define MODBUS_MASTER_POLL_MS 20U

ModbusMaster::ModbusMaster(UART *bus) : _bus(bus) {
    addTaskMain(taskCallback {
        if (_state == WAITING && millis() >= _timeoutAt) {
            _statistics.timeouts++;
            _finish(false, 0U, nullptr, 0U);
        }
    }, MODBUS_MASTER_POLL_MS);
}
bool ModbusMaster::isBusy() const {
    return _state != IDLE;
}
bool ModbusMaster::readHoldingRegisters(uint8_t slaveId, uint16_t address, uint16_t count,
                                         ResponseCallback callback, uint32_t timeoutMs) {
    if (_state != IDLE || _bus == nullptr || count == 0U || count > MODBUS_MAX_READ_REGISTERS) return false;

    uint8_t frame[8];
    frame[0] = slaveId;
    frame[1] = MODBUS_FUNC_READ_HOLDING;
    writeU16BE(frame + 2U, address);
    writeU16BE(frame + 4U, count);
    const uint16_t len = appendCrc(frame, 6U);

    _expectedSlaveId = slaveId;
    _expectedFunction = MODBUS_FUNC_READ_HOLDING;
    _expectedCount = count;
    _callback = callback;
    _timeoutAt = millis() + timeoutMs;
    _state = WAITING;

    _bus->transmit(frame, len);
    return true;
}
void ModbusMaster::onReceive(uint8_t *data, uint16_t length) {
    if (_state != WAITING || data == nullptr || length < 5U) return;
    if (!validateCrc(data, length) || data[0] != _expectedSlaveId) return;

    if (data[1] == (uint8_t)(_expectedFunction | 0x80U)) {
        _statistics.exceptions++;
        _finish(false, data[2], nullptr, 0U);
        return;
    }
    if (data[1] != _expectedFunction || data[2] != (uint8_t)(_expectedCount * 2U) ||
        length != (uint16_t)(3U + data[2] + 2U)) {
        _statistics.malformedFrames++;
        _finish(false, 0U, nullptr, 0U);
        return;
    }

    uint16_t registers[MODBUS_MAX_READ_REGISTERS];
    for (uint16_t index = 0U; index < _expectedCount; index++) {
        registers[index] = readU16BE(data + 3U + index * 2U);
    }
    _statistics.requests++;
    _finish(true, 0U, registers, _expectedCount);
}
void ModbusMaster::_finish(bool success, uint8_t exception, uint16_t *registers, uint16_t count) {
    _state = IDLE;
    ResponseCallback callback = _callback;
    _callback = nullptr;
    if (callback) callback(success, exception, registers, count);
}
