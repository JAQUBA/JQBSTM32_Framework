/*
 * JQBSTM32 Framework - FM25V05.h Header
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
#include "../../Hardware/SPI/SPI.h"
#ifdef __SPI_H_

#ifndef __FM25V05_H_
#define __FM25V05_H_

#include "../../Interface/IExternalMemory.h"

class FM25V05 : public IExternalMemory {
    public:
        FM25V05(SPI *pInstance,
            GPIO_TypeDef* GPIOx,
            uint16_t GPIO_Pin
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
        SPI             *_pInstance;

        GPIO_TypeDef    *_CSPort;
        uint16_t        _CSPin;
    
};
#endif
#endif
