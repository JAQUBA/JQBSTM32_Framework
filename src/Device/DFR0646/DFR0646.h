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

/**
 * @class DFR0646
 * @brief Driver class for DFR0646 I2C 7-segment LED display module
 * @details This class provides interface for controlling DFR0646 4-digit 7-segment LED display
 *          via I2C communication. The module features HT16K33 controller and supports
 *          numeric display with decimal point control. Commonly used for showing
 *          temperatures, time, or other numeric values.
 * @note Display supports digits 0-9 and some letters, with individual decimal point control
 */
class DFR0646 {
	public:
		/**
		 * @brief Constructor for DFR0646 LED display driver
		 * @param pInstance Pointer to I2C instance for communication
		 * @param DevAddress I2C device address of the display module
		 * @details Initializes DFR0646 driver with specified I2C interface and device address.
		 *          The device address is typically configurable via jumpers on the module.
		 * @note Module typically uses addresses 0x70-0x77 depending on A0,A1,A2 jumper settings
		 */
		DFR0646(I2C *pInstance, uint8_t DevAddress);
		
		/**
		 * @brief Display text with decimal point control on LED display
		 * @param wsk Pointer to character string to display (max 4 characters)
		 * @param dot_point Decimal point bitmask (bit position corresponds to digit position)
		 * @details Displays up to 4 characters on the 7-segment display with optional decimal points.
		 *          Characters are automatically converted to 7-segment patterns. Supports digits 0-9
		 *          and some letters. Decimal points are controlled by individual bits in dot_point parameter.
		 * @note Bit 0 of dot_point controls rightmost decimal point, bit 3 controls leftmost
		 * @note Unsupported characters may display as blank or garbled segments
		 */
		void printLed(char* wsk, uint8_t dot_point);
		
	private:
		I2C *_pInstance;     ///< Pointer to I2C communication interface
		uint8_t _DevAddress; ///< I2C device address of the display module
};

#endif
#endif
