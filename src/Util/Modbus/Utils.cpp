#include "Utils.h"

#include <string.h>

uint32_t ModbusUtils::registersToUint32(const uint16_t *registers, bool wordSwap) {
    const uint16_t high = wordSwap ? registers[1] : registers[0];
    const uint16_t low = wordSwap ? registers[0] : registers[1];
    return ((uint32_t)high << 16U) | low;
}

int32_t ModbusUtils::registersToInt32(const uint16_t *registers, bool wordSwap) {
    return (int32_t)registersToUint32(registers, wordSwap);
}

float ModbusUtils::registersToFloat(const uint16_t *registers, bool wordSwap) {
    const uint32_t raw = registersToUint32(registers, wordSwap);
    float value;
    memcpy(&value, &raw, sizeof(value));
    return value;
}

double ModbusUtils::registersToDouble(const uint16_t *registers, bool wordSwap) {
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

void ModbusUtils::uint32ToRegisters(uint32_t value, uint16_t *registers, bool wordSwap) {
    registers[wordSwap ? 1 : 0] = (uint16_t)(value >> 16U);
    registers[wordSwap ? 0 : 1] = (uint16_t)value;
}

void ModbusUtils::floatToRegisters(float value, uint16_t *registers, bool wordSwap) {
    uint32_t raw;
    memcpy(&raw, &value, sizeof(raw));
    uint32ToRegisters(raw, registers, wordSwap);
}

void ModbusUtils::doubleToRegisters(double value, uint16_t *registers, bool wordSwap) {
    uint64_t raw;
    memcpy(&raw, &value, sizeof(raw));
    for (uint8_t i = 0U; i < 4U; i++) {
        const uint8_t shift = (uint8_t)(48U - i * 16U);
        const uint16_t word = (uint16_t)(raw >> shift);
        registers[wordSwap ? 3U - i : i] = word;
    }
}

void ModbusUtils::writeBits(uint16_t *words, uint16_t bitOffset, uint8_t bitCount, uint32_t value) {
    for (uint8_t bit = 0U; bit < bitCount; bit++) {
        const uint16_t targetBit = bitOffset + bit;
        const uint16_t wordIndex = targetBit / 16U;
        const uint8_t wordBit = targetBit % 16U;
        const uint16_t mask = (uint16_t)(1U << wordBit);
        if ((value >> bit) & 1U) words[wordIndex] |= mask;
        else words[wordIndex] &= (uint16_t)~mask;
    }
}
