#include "../../Core.h"
#ifdef HAL_ADC_MODULE_ENABLED

#ifndef __ANALOGAC_H_
#define __ANALOGAC_H_

class AnalogAC {
	public:
		AnalogAC(uint8_t channelNumber1, uint8_t channelNumber2);
		uint16_t getRaw();
		uint16_t getValue();
		void configureChannel(uint16_t *offset, uint16_t *divider);
	private:
		uint8_t _channelNumber1;
		uint8_t _channelNumber2;
		uint16_t *_offset; /**< Wskaźnik do offsetu. */
        uint16_t *_divider; /**< Wskaźnik do dzielnika. */
};

#endif
#endif