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

uint32_t Modbus::registersToUint32(const uint16_t *registers, bool wordSwap) {
    const uint16_t high = wordSwap ? registers[1] : registers[0];
    const uint16_t low = wordSwap ? registers[0] : registers[1];
    return ((uint32_t)high << 16U) | low;
}
int32_t Modbus::registersToInt32(const uint16_t *registers, bool wordSwap) {
    return (int32_t)registersToUint32(registers, wordSwap);
}
float Modbus::registersToFloat(const uint16_t *registers, bool wordSwap) {
    const uint32_t raw = registersToUint32(registers, wordSwap);
    float value;
    memcpy(&value, &raw, sizeof(value));
    return value;
}
double Modbus::registersToDouble(const uint16_t *registers, bool wordSwap) {
    const uint16_t ordered[4] = {
        registers[wordSwap ? 3 : 0], registers[wordSwap ? 2 : 1],
        registers[wordSwap ? 1 : 2], registers[wordSwap ? 0 : 3]
    };
    uint64_t raw = 0U;
    for (uint8_t i = 0U; i < 4U; i++) raw = (raw << 16U) | ordered[i];
    double value;
    memcpy(&value, &raw, sizeof(value));
    return value;
}
void Modbus::uint32ToRegisters(uint32_t value, uint16_t *registers, bool wordSwap) {
    registers[wordSwap ? 1 : 0] = (uint16_t)(value >> 16U);
    registers[wordSwap ? 0 : 1] = (uint16_t)value;
}
void Modbus::floatToRegisters(float value, uint16_t *registers, bool wordSwap) {
    uint32_t raw;
    memcpy(&raw, &value, sizeof(raw));
    uint32ToRegisters(raw, registers, wordSwap);
}
void Modbus::doubleToRegisters(double value, uint16_t *registers, bool wordSwap) {
    uint64_t raw;
    memcpy(&raw, &value, sizeof(raw));
    for (uint8_t i = 0U; i < 4U; i++) {
        const uint8_t shift = (uint8_t)(48U - i * 16U);
        const uint16_t word = (uint16_t)(raw >> shift);
        registers[wordSwap ? 3 - i : i] = word;
    }
}

const ModbusStatistics& Modbus::statistics() const {
    return _statistics;
}
void Modbus::resetStatistics() {
    _statistics = {};
}
