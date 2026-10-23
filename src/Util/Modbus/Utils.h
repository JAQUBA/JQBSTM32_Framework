#ifndef __MODBUS_UTILS_H_
#define __MODBUS_UTILS_H_

#include <stdint.h>

class ModbusUtils {
public:
    static uint32_t registersToUint32(const uint16_t *registers, bool wordSwap = false);
    static int32_t registersToInt32(const uint16_t *registers, bool wordSwap = false);
    static float registersToFloat(const uint16_t *registers, bool wordSwap = false);
    static double registersToDouble(const uint16_t *registers, bool wordSwap = false);
    static void uint32ToRegisters(uint32_t value, uint16_t *registers, bool wordSwap = false);
    static void floatToRegisters(float value, uint16_t *registers, bool wordSwap = false);
    static void doubleToRegisters(double value, uint16_t *registers, bool wordSwap = false);
    static void writeBits(uint16_t *words, uint16_t bitOffset, uint8_t bitCount, uint32_t value);
};

#endif
