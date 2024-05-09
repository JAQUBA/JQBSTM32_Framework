#include "Modbus.h"
#include "../CRC16/CRC16.h"

void (*functionHandlers[4])(ModbusFrame *request) = { };

void ModbusSlave::setID(uint8_t slaveID) {
    this->_slaveID = slaveID;
}
void Modbus::bind_function(ModbusFunction function, void(*functionPointer)(ModbusFrame *request)) {
    functionHandlers[function] = functionPointer;
}
void Modbus::receive(uint8_t* data, uint16_t length, void (*functionPointer)(uint8_t *data, uint16_t size)) {
    if(data[0] != _slaveID) return;
    ModbusFrame *modbusFrame = (ModbusFrame*) malloc(sizeof(ModbusFrame));
    modbusFrame->function = (ModbusFunction) data[1];
    modbusFrame->address = (data[2]<<8) | data[3];
    modbusFrame->size = ((data[4]<<8) | data[5]);

    uint16_t len = 0;
    uint8_t buffer[512];
    buffer[len++] = (uint8_t)_slaveID;
    buffer[len++] = (uint8_t)modbusFrame->function;

    switch((uint8_t)modbusFrame->function) {
        case 3: {
            functionHandlers[ModbusFunction::FUNC_3](modbusFrame);
            buffer[len++] = (uint8_t)(modbusFrame->size*2);
            for (uint16_t i = 0; i < modbusFrame->size; i++) {
                buffer[len++] = (uint8_t)((modbusFrame->registers[i] & 0xFF00) >> 8);
                buffer[len++] = (uint8_t)(modbusFrame->registers[i] & 0x00FF);
            }
            break;
        }
        case 6:{
            functionHandlers[ModbusFunction::FUNC_6](modbusFrame);
            buffer[len++] = (uint8_t)((modbusFrame->address & 0xFF00) >> 8);
            buffer[len++] = (uint8_t)(modbusFrame->address & 0x00FF);
            buffer[len++] = (uint8_t)((modbusFrame->size & 0xFF00) >> 8);
            buffer[len++] = (uint8_t)(modbusFrame->size & 0x00FF);
            
            break;
        }
        default: {
            return;
        }
    }

    uint16_t crc = CRC16(buffer, len);
    buffer[len++] = (uint8_t)(crc & 0x00FF);
    buffer[len++] = (uint8_t)((crc & 0xFF00) >> 8);
    
    functionPointer(buffer, len);
    free(modbusFrame);
}