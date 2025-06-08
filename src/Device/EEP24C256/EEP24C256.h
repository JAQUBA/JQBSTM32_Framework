/*
 * JQBSTM32 Framework - EEP24C256.h Header
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "../../Hardware/I2C/I2C.h"
#ifdef __I2C_H_

#ifndef __EEP24C256_H_
#define __EEP24C256_H_

#include "../../Interface/IExternalMemory.h"

class EEP24C256 : public IExternalMemory {
    public:
        EEP24C256(I2C *pInstance,
            uint16_t DevAddress = 0xA0,
            uint32_t BaseAddress = 0x00
        );
        void readFromMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) override;

        void writeToMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) override;
        
    protected:
        I2C             *_pInstance;
        uint16_t        _DevAddress;
        uint32_t        _BaseAddress;
        uint16_t        _MemAddSize = ADDRESS_SIZE_16BIT;
};
#endif
#endif
