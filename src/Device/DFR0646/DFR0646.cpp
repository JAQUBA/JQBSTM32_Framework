/*
 * JQBSTM32 Framework - DFR0646.cpp Implementation
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
#include "DFR0646.h"
#ifdef __DFR0646_H_

#include "fl_bit.h"

#define CMD_HT16K33_ON          0x01  /**< Turn ON  */
#define CMD_HT16K33_OSCOFF      0x20  /**< Turn OFF the clock */
#define CMD_HT16K33_OSCON       0x21  /**< Turn ON the clock */
#define CMD_HT16K33_DFR0646OFF  0x80  /**< Turn OFF the DFR0646 */
#define CMD_HT16K33_DFR0646ON   0x81  /**< Turn ON the DFR0646*/
#define CMD_HT16K33_FLASHTWOS   0x87  /**< Set the mode to flash at 0.5Hz*/
#define CMD_HT16K33_FLASHONES   0x85  /**< Set the mode to flash at 1Hz*/
#define CMD_HT16K33_FLASHHALFS  0x83  /**< Set the mode to flash at 2Hz*/
#define CMD_HT16K33_STOPFLASH   0x81  /**< Flash end*/
#define CMD_HT16K33_BRIGHTNESS  0xE0

#include "font/def_font.h"

DFR0646::DFR0646(I2C *pInstance, uint8_t DevAddress) {
	_pInstance = pInstance;
	_DevAddress = DevAddress;

	uint8_t cmd;

	cmd=CMD_HT16K33_OSCON;
	_pInstance->transmit(_DevAddress, &cmd, 1);

	cmd=CMD_HT16K33_ON;
	_pInstance->transmit(_DevAddress, &cmd, 1);

	cmd=CMD_HT16K33_DFR0646ON;
	_pInstance->transmit(_DevAddress, &cmd, 1);

	cmd=CMD_HT16K33_BRIGHTNESS | 15;
	_pInstance->transmit(_DevAddress, &cmd, 1);
}
void DFR0646::printLed(char* wsk, uint8_t dot_point) {
	uint8_t buf[16];

    char cacheData1=' ',cacheData2=' ',cacheData3=' ',cacheData4=' ', cacheData5=' ',cacheData6=' ',cacheData7=' ',cacheData8=' ',cacheData9='0';
    sscanf(wsk,"%c%c%c%c%c%c%c%c%c",&cacheData1,&cacheData2,&cacheData3,&cacheData4,&cacheData5,&cacheData6,&cacheData7,&cacheData8,&cacheData9);

	buf[0] = 0;
	buf[1] = TAB2[cacheData1-32];
	if (bfb_iss(&dot_point, 7)) buf[1]=buf[1] | 128;
	buf[2] = 2;
	buf[3] = TAB2[cacheData2-32];
	if (bfb_iss(&dot_point, 6)) buf[3]=buf[3] | 128;
	buf[4] = 4;
	buf[5] = TAB2[cacheData3-32];
	if (bfb_iss(&dot_point, 5)) buf[5]=buf[5] | 128;
	buf[6] = 6;
	buf[7] = TAB2[cacheData4-32];
	if (bfb_iss(&dot_point, 4)) buf[7]=buf[7] | 128;
	buf[8] = 8;
	buf[9] = TAB2[cacheData5-32];
	if (bfb_iss(&dot_point, 3)) buf[9]=buf[9] | 128;
	buf[10] = 10;
	buf[11] = TAB2[cacheData6-32];
	if (bfb_iss(&dot_point, 2)) buf[11]=buf[11] | 128;
	buf[12] = 12;
	buf[13] = TAB2[cacheData7-32];
	if (bfb_iss(&dot_point, 1)) buf[13]=buf[13] | 128;
	buf[14] = 14;
	buf[15] = TAB2[cacheData8-32];
	if (bfb_iss(&dot_point, 0)) buf[15]=buf[15] | 128;

	_pInstance->transmit(_DevAddress, buf, 16);
}
#endif
