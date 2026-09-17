/*
 * JQBSTM32 Framework - Modbus Protocol Implementation
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

#include "Modbus.h"
#include "../Math/CRC16.h"

uint16_t Modbus::readU16BE(const uint8_t *p) {
    return ((uint16_t)p[0] << 8U) | p[1];
}
void Modbus::writeU16BE(uint8_t *p, uint16_t value) {
    p[0] = (uint8_t)(value >> 8U);
    p[1] = (uint8_t)value;
}
bool Modbus::validateCrc(const uint8_t *data, uint16_t length) {
    if (length < 2U) return false;
    // Modbus CRC is transmitted little-endian, unlike register fields
    const uint16_t receivedCrc = (uint16_t)data[length - 2U] | ((uint16_t)data[length - 1U] << 8U);
    return CRC16(data, length - 2U) == receivedCrc;
}
uint16_t Modbus::appendCrc(uint8_t *buffer, uint16_t length) {
    const uint16_t crc = CRC16(buffer, length);
    buffer[length++] = (uint8_t)crc;
    buffer[length++] = (uint8_t)(crc >> 8U);
    return length;
}

const ModbusStatistics& Modbus::statistics() const {
    return _statistics;
}
void Modbus::resetStatistics() {
    _statistics = {};
}
