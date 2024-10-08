#include "Core.h"
#include "Hardware/OneWire/OneWire.h"

class DS18B20 {
    public:
        DS18B20(OneWire *oneWire);
        void readTemperature();
    private:
        OneWire *oneWire;
};