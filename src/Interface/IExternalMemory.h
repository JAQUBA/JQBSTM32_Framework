/*
 * JQBSTM32 Framework - IExternalMemory.h Header
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
#ifndef __IEXTERNALMEMORY_H
#define __IEXTERNALMEMORY_H

//#include "../Interface/IBus.h"

#define ADDRESS_SIZE_8BIT            (0x00000001U)
#define ADDRESS_SIZE_16BIT           (0x00000002U)
#define ADDRESS_SIZE_24BIT           (0x00000003U)
#define ADDRESS_SIZE_32BIT           (0x00000004U)

class IExternalMemory {
    public:
        virtual void readFromMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) = 0;
        virtual void writeToMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) = 0;
    protected:
        // IBus *_pInstance;
};

#endif
