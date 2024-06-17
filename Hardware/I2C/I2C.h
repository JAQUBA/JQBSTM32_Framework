#ifndef __I2C_H_
#define __I2C_H_

#include "../../Core.h"
#include "Interface/IBus.h"

#define I2C_MAX_INSTANCES 2

class I2C : public IBus {
    public:
        I2C(I2C_HandleTypeDef* instance);
        static I2C *getInstance(I2C_HandleTypeDef *instance);

        void txInterrupt();

        void send(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
        void recv(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
    private:
        I2C_HandleTypeDef* _pInstance;

        uint8_t _i2c_state;
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