/*
 * JQBSTM32 Framework - CRC16 Utilities Header
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
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

#ifndef __CRC16_H_
#define __CRC16_H_
#include <stdint.h>

/**
 * @file CRC16.h
 * @brief CRC16 calculation and BCD conversion utilities
 * @details This header provides functions for CRC16 checksum calculation and
 *          BCD (Binary Coded Decimal) conversion operations commonly used
 *          in communication protocols and data validation.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert BCD (Binary Coded Decimal) value to decimal
 * @param val BCD value to convert (each nibble represents a decimal digit 0-9)
 * @return Decimal equivalent of the BCD value
 * @details Converts a BCD encoded byte to its decimal representation.
 *          Upper nibble represents tens digit, lower nibble represents units digit.
 * @note Input should have valid BCD encoding (each nibble 0-9)
 * @example bcdToDec(0x23) returns 23
 */
uint8_t 	bcdToDec(uint8_t val);

/**
 * @brief Convert decimal value to BCD (Binary Coded Decimal)
 * @param val Decimal value to convert (0-99)
 * @return BCD encoded representation of the decimal value
 * @details Converts a decimal number to BCD format where each nibble
 *          contains a single decimal digit (0-9).
 * @note Input should be in range 0-99 for proper BCD encoding
 * @example decToBcd(23) returns 0x23
 */
uint8_t 	decToBcd(uint8_t val);

/**
 * @brief Calculate CRC16 checksum for data buffer
 * @param wsk Pointer to data buffer to calculate checksum for
 * @param len Length of data buffer in bytes
 * @return 16-bit CRC checksum value
 * @details Calculates CRC16 checksum using standard polynomial for data validation.
 *          Commonly used in communication protocols like Modbus for error detection.
 *          Uses polynomial 0x8005 (CRC-16-IBM/ANSI).
 * @note Returns 0 for empty buffer (len = 0)
 */
uint16_t 	CRC16(const uint8_t *wsk, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
