#include "AnalogAC.h"
#include "math.h"

AnalogAC::AnalogAC(uint8_t channelNumber1, uint8_t channelNumber2) : _channelNumber1(channelNumber1), _channelNumber2(channelNumber2) {}

uint16_t AnalogAC::getRaw() {
    return 0;
}
uint16_t AnalogAC::getValue() {
    uint16_t chan1 = (uint16_t)(((Analog::rawADC[_channelNumber1] * ((*_divider))) >> 14) - (*_offset) );
    uint16_t chan2 = (uint16_t)(((Analog::rawADC[_channelNumber2] * ((*_divider))) >> 14) - (*_offset) );
    // return sqrt((chan1 * chan1 + chan2 * chan2) / 2);
    // return (chan1 + chan2) / 2;
    return chan1;
}
void AnalogAC::configureChannel(uint16_t *offset, uint16_t *divider) {
    _offset = offset;
    _divider = divider;
}