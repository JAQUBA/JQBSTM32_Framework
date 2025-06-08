/*
 * JQBSTM32 Framework - SED1520.cpp Implementation
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
#include "SED1520.h"
#ifdef __SED1520_H_

#define SED1520_RESET 0xE2
#define SED1520_DUTY_RATIO 0xA9
#define SED1520_STATIC_DRIVE_OFF 0xA4
#define SED1520_ACD_SELECT 0xA1
#define SED1520_SED1520_ON 0xAF
#define SED1520_PAGE_ADDRESS 0xB8
#define SED1520_COLUMN_ADDRESS 0x13
#define SED1520_SED1520_START_LINE 0xC0
#define SED1520_END 0xEE
#define SED1520_BLANK 0x00

SED1520::SED1520(I2C* i2c) {
	_i2c = i2c;
	uint8_t buf[3] = {0x06, 0x00, 0x00};
	_i2c->transmit(0x4e, buf, 3);
	addBuffer(0xFF, DATA, ALL);
	addBuffer(0xFF, DATA, ALL);
	addBuffer(0xFF, DATA, ALL);
	addBuffer(SED1520_RESET, COMMAND, ALL);
	addBuffer(SED1520_DUTY_RATIO, COMMAND, ALL);
	addBuffer(SED1520_STATIC_DRIVE_OFF, COMMAND, ALL);
	addBuffer(SED1520_ACD_SELECT, COMMAND, ALL);
	addBuffer(SED1520_SED1520_ON, COMMAND, ALL);
	addBuffer(SED1520_COLUMN_ADDRESS, COMMAND, ALL);
    addBuffer(SED1520_SED1520_START_LINE, COMMAND, ALL);
	clear();
}
void SED1520::clear() {
	_current_column = 0;
	_current_row = 0;
	for(int line = 0; line < 4; line++) {
		addBuffer(SED1520_PAGE_ADDRESS + line, COMMAND, ALL);
		for(int a=0; a < 61; a++) {
			addBuffer(SED1520_BLANK, DATA, ALL);
		}
		addBuffer(SED1520_END, COMMAND, ALL);
	}
	flush();
}
void SED1520::print(char const *text, bool fill) {
	addBuffer(SED1520_PAGE_ADDRESS + _current_row, COMMAND, ALL);
	addBuffer(SED1520_COLUMN_ADDRESS + _current_column % 61, COMMAND, ALL);
	
	while(*text) {
		printChar(*text++);
	}
	if(fill) {
		while(_current_column < FONT_MAX_CHAR*6) {
			printChar(' ');
		}
	}
	addBuffer(SED1520_END, COMMAND, ALL);
	flush();
}
void SED1520::printChar(char chr) {
	chr-=32;
	for(uint8_t a = 0; a < 6 ; a++) {
		if(_current_column == 61) {
			addBuffer(SED1520_COLUMN_ADDRESS, COMMAND, SECOND);
		}
		addBuffer(font6x8[(chr * 6) + a], DATA, _current_column < 61 ? FIRST : SECOND);
		_current_column++;
	}
}
void SED1520::setCursor(uint8_t y, uint8_t x) {
	_current_row = y;
	_current_column = x*6;
}
void SED1520::addBuffer(uint8_t data, Control command, Sector sector) {
	dataQueue.push_back(0x02);
	dataQueue.push_back(data);
	dataQueue.push_back((command<<7) | (((sector+1)&0x01)<<6) | (((sector+1)&0x02)<<4));
	dataQueue.push_back(data);
	dataQueue.push_back(0x00);
	dataQueue.push_back(0x00);
}

void SED1520::flush() {
	uint8_t arr[dataQueue.size()];
    std::copy(dataQueue.begin(), dataQueue.end(), arr);
	_i2c->transmit(0x4e, arr, dataQueue.size());
	dataQueue.clear();
}
#endif
