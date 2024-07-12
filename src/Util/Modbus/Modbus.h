#ifndef __MODBUS_H_
#define __MODBUS_H_
#include "../../Core.h"

enum ModbusFunction {
    FUNC_3,
    FUNC_6,
    FUNC_10
};
struct ModbusFrame {
    ModbusFunction function;
    uint16_t address;
    uint16_t size;
    uint16_t registers[125];
};

class Modbus {
    public:
        void receive(uint8_t* data, uint16_t length, void(*functionPointer)(uint8_t *data, uint16_t size));
        void bind_function(ModbusFunction function, void(*functionPointer)(ModbusFrame *request));
    protected:
        uint8_t *_slaveID;
};

class ModbusSlave : public Modbus {
    public:
        void setID(uint8_t *slaveID);
    private:
};

#endif