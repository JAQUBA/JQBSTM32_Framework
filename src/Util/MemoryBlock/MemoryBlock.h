/*
 * JQBSTM32 Framework - MemoryBlock.h Header
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
#ifndef _MEMORYBLOCK_H
#define _MEMORYBLOCK_H
#include "Core.h"
#include "Interface/IExternalMemory.h"

class MemoryBlock {
    public:
        MemoryBlock(IExternalMemory *device,
         uint32_t startingAddress);
         
        void loadBlock(
            uint8_t *pData, 
            uint16_t Size
        );

        void saveBlock(
            uint8_t *pData, 
            uint16_t Size
        );
    private:
        IExternalMemory *_extMemory;
        uint32_t        _startingAddress;
};

#endif
