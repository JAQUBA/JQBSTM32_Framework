/*
 * JQBSTM32 Framework - DFR0646.h Header
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
#include "../../Hardware/I2C/I2C.h"
#ifdef __I2C_H_

#ifndef __DFR0646_H_
#define __DFR0646_H_

class DFR0646 {
	public:
		DFR0646(I2C *pInstance, uint8_t DevAddress);
		void printLed(char* wsk, uint8_t dot_point);
	private:
		I2C *_pInstance;
		uint8_t _DevAddress;
};

#endif
#endif
