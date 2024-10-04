#include "AnalogAC.h"

AnalogAC::AnalogAC(uint8_t channelNumber1, uint8_t channelNumber2) : _channelNumber1(channelNumber1), _channelNumber2(channelNumber2) {}

uint16_t AnalogAC::getRaw() {
    return 0;
}
uint16_t AnalogAC::getValue() {
    return 0;
}
void AnalogAC::configureChannel(uint16_t *offset, uint16_t *divider) {
    return;
}