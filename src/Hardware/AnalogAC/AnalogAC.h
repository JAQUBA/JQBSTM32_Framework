#ifndef _ANALOGAC_H
#define _ANALOGAC_H

#include "Core.h"

class AnalogAC {
	public:
		AnalogAC(uint8_t channelNumber1, uint8_t channelNumber2);
		uint16_t getRaw();
		uint16_t getValue();
		void configureChannel(uint16_t *offset, uint16_t *divider);
	private:
		uint8_t _channelNumber1;
		uint8_t _channelNumber2;
};

#endif