#ifndef __NV_RAM_H_
#define __NV_RAM_H_
#include "../../Core.h"
class NV_RAM {
    public:
        NV_RAM(I2C_HandleTypeDef *instance, uint8_t address, uint16_t pages, uint16_t pagesize, uint16_t offset=0);
        void read16(uint16_t address, uint16_t *data, uint16_t size);
        void write16(uint16_t address, uint16_t *data, uint16_t size);
        
    private:
        I2C_HandleTypeDef *_instance;
        uint8_t _address;
        uint8_t _i2c_state;
        uint16_t _pages;
        uint16_t _pageSize;
        uint16_t _offset;
        uint16_t *_data;
        uint16_t _size;
        uint32_t _timeout;
        uint8_t  _status;
        
        enum States {
            I2C_OK,
            I2C_ERR_HAL,
            I2C_ERR_TO_S,
            I2C_ERR_TO_W,
            NIL,
            START_READ16,
            WAIT_COMPL_READ16,
            START_WRITE16,
            WAIT_COMPL_WRITE16
        } _state;
       
};
#endif