#include "Analog.h"
#ifdef __ANALOG_H_

#include "Hardware/UART/UART.h"
extern UART rs232;

uint16_t Analog::rawADC[8];
uint32_t Analog::avgADC[8];
uint16_t wynik = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    for(uint8_t i = 0; i < hadc->Init.NbrOfConversion; i++) {
        Analog::avgADC[i] += Analog::rawADC[i];
    }
    wynik++;
}

void Analog::init(ADC_HandleTypeDef *pHandler) {
    HAL_ADC_Start_DMA(pHandler, (uint32_t*)rawADC, pHandler->Init.NbrOfConversion);
    
    addTaskInterrupt(taskCallback {
        char buffer[32];
        sprintf(buffer, "%d %d <-> %d\r\n", wynik, (Analog::avgADC[4]/wynik), (Analog::avgADC[5]/wynik));
        rs232.transmit((uint8_t*)buffer, strlen(buffer));
        wynik=0;
        for(uint8_t i = 0; i < 8; i++) {
            Analog::avgADC[i] = 0;
        }
    }, 1000); // 1s
}
Analog::Analog(uint8_t channelNumber) {
    _channelNumber = channelNumber;
}
void Analog::configureChannel(uint16_t *offset, uint16_t *multiplier) {
    _offset = offset;
    _multiplier = multiplier;
}
uint16_t Analog::getValue() {
    int16_t val =(uint16_t)(((rawADC[_channelNumber] * (*_multiplier)) >> 10) - (*_offset));
    if(val < 0) val = 0;
    return val;
}

#endif