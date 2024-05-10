#ifndef __I2C_H_
#define __I2C_H_

#include "../../Core.h"

#define I2C_MAX_INSTANCES 2

class I2C {
    public:
        I2C(I2C_HandleTypeDef* instance);
        static I2C *getInstance(I2C_HandleTypeDef *instance);

        void txInterrupt();

        void send(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
        void recv(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
    private:
        I2C_HandleTypeDef* _pInstance;      
};
#endif