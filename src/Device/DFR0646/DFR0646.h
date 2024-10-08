#ifndef APP_DFR0646_H
#define APP_DFR0646_H

#include "../../Hardware/I2C/I2C.h"

class DFR0646 {
	public:
		DFR0646(I2C *pInstance, uint8_t DevAddress);
		void printLed(char* wsk, uint8_t dot_point);
	private:
		I2C *_pInstance;
		uint8_t _DevAddress;
};

#endif