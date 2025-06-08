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
#ifdef __cplusplus
extern "C" {
#endif
uint8_t 	bcdToDec(uint8_t val);
uint8_t 	decToBcd(uint8_t val);
uint16_t 	CRC16(const uint8_t *wsk, uint16_t len);
#ifdef __cplusplus
}
#endif

#endif
