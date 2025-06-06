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