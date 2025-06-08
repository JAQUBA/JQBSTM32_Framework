/*
 * JQBSTM32 Framework - def_font.h Header
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

/**
 * @file def_font.h
 * @brief Default font definition for DFR0646 7-segment LED display
 * @details Contains character encoding tables for displaying ASCII characters
 *          on 7-segment LED displays. Maps ASCII values to 7-segment patterns.
 * @author JAQUBA
 * @version 1.0
 * @date 2024
 * 
 * @note Some ASCII characters cannot be properly displayed on 7-segment displays
 * @note Character patterns are optimized for readability on 7-segment format
 */

/**
 * @brief Character encoding table for 7-segment display
 * @details Array mapping ASCII character codes to 7-segment display patterns.
 *          Each entry corresponds to the bit pattern needed to display
 *          the character on a 7-segment LED display.
 * 
 * @note Array index corresponds to ASCII value minus 32 (space character)
 * @note Value 0x00 indicates character cannot be displayed or is blank
 * @note Bit pattern corresponds to 7-segment display segments (a-g)
 * 
 * Supported characters include:
 * - Digits 0-9 (fully supported)
 * - Some letters (limited by 7-segment constraints)
 * - Basic punctuation where possible
 * - Special symbols adapted for 7-segment format
 */
const unsigned char TAB2[] = {
	0x00,//  32
	0x00,//  33
	0x00,//  34
	0x00,//  35
	0x00,//  36
	0x00,//  37
	0x00,//  38
	0x00,//  39
	0x00,//  40
	0x00,//  41
	0x00,//  42
	0x00,//  43
	0x00,//  44
	0x40,//- 45
	0x80,//. 46
	0x00,/// 47
	0x3F,//0 48
	0x06,//1
	0x5B,//2
	0x4F,//3
	0x66,//4
	0x6D,//5
	0x7D,//6
	0x07,//7
	0x7F,//8
	0x6F,//9
	0x00,//:
	0x00,//;
	0x00,//<
	0x00,//=
	0x00,//>
	0xDB,//?
	0x00,//@
	0x77,//A
	0x7C,//B
	0x39,//C
	0x5E,//D
	0x79,//E
	0x71,//F
	0x00,//G
	0x76,//H
	0x00,//I
	0x00,//J
	0x00,//K
	0x38,//L 76
	0x00,//M
	0x00,//N
	0x5C,//O
	0x73,//P 80
	0x00,//Q
	0x00,//R
	0x00,//S
	0x00,//T
	0x3E,//U
	0x00,//V
	0x00,//W
	0x00,//X
	0x00,//Y
	0x00,//Z 90
	0x00,//[
	0x00,//
	0x00,//]
	0x00,//^
	0x08//_ 95
};
