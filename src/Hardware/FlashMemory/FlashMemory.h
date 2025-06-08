/*
 * JQBSTM32 Framework - FlashMemory.h Header
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
#include "../../Core.h"
#ifdef HAL_FLASH_MODULE_ENABLED

#ifndef __FLASHMEMORY_H_
#define __FLASHMEMORY_H_

#include "../../Interface/IExternalMemory.h"

class FlashMemory : public IExternalMemory{
    public:
        void writeToMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);
        void readFromMemory(uint32_t MemAddress, uint8_t *pData, uint16_t Size);

        uint64_t read(uint32_t MemAddress);
        void write(uint32_t MemAddress, uint64_t Data);
        
    private:
        FLASH_EraseInitTypeDef GenerateFlashEraseStruct(uint32_t Address);
        #if defined(STM32F4) || defined(STM32H7)
        uint32_t GetFlashSector(uint32_t Address);
        #endif
};

#endif
#endif
