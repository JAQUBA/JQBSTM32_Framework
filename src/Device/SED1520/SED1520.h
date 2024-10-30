#include "../../Hardware/I2C/I2C.h"
#ifdef __I2C_H_
#ifndef __SED1520_H_
#define __SED1520_H_

#include "Fonts/font6x8.h"

class SED1520 {
    public:
        SED1520(I2C* i2c);
        void clear();
        void print(char const *text, bool fill = false);
        void setCursor(uint8_t y, uint8_t x);
        void flush();
    private:
        I2C *_i2c;
        enum Sector {FIRST, SECOND, ALL};
        enum Control {COMMAND, DATA};
        void addBuffer(uint8_t data, Control control, Sector sector);
        uint8_t _current_row = 0;
        uint8_t _current_column = 0;

        void printChar(const char chr);
        

        std::list<uint8_t> dataQueue;
};

#endif
#endif