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
