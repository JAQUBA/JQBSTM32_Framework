/*
 * JQBSTM32 Framework - RegisterBank.cpp Implementation
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
#include "RegisterBank.h"

// Global registry of register banks for efficient lookup
struct Register {
    RegisterBank *bank;
    struct Register *next = nullptr;
} *registers = nullptr;

// Performance optimization: cache for last accessed bank
static RegisterBank* lastAccessedBank = nullptr;
static uint16_t lastAccessedAddress = 0xFFFF;

RegisterBank::RegisterBank(uint16_t start, uint16_t size) {
    _size = size;
    _start = start;
    _stop = _start + size;
    _memoryBlock = nullptr;
    
    _initialize();
}

RegisterBank::RegisterBank(uint16_t start, uint16_t size, MemoryBlock *memoryBlock) {
    _size = size;
    _start = start;
    _stop = _start + size;
    _memoryBlock = memoryBlock;
    
    _initialize();
}

void RegisterBank::_initialize() {
    if(_size > 0) {        // Use calloc instead of malloc for zero-initialized memory
        _registers = (uint16_t*)calloc(_size, sizeof(uint16_t));
        if(_registers == nullptr) {
            // Handle allocation failure gracefully
            _size = 0;
            return;
        }
    } else {
        _registers = nullptr;
    }
    // Remove redundant memset since calloc already zeros the memory
    
    // Register this bank in global registry
    struct Register *temp = registers, *r;
    if(registers == nullptr) {
        temp = (struct Register*)malloc(sizeof(struct Register));
        if(temp == nullptr) {
            // Handle allocation failure
            free(_registers);
            _registers = nullptr;
            _size = 0;
            return;
        }
        temp->bank = this;
        temp->next = nullptr;
        registers = temp;    } else {
        while(temp->next != nullptr) temp = temp->next;
        r = (struct Register*) malloc(sizeof(struct Register));
        if(r == nullptr) {
            // Handle allocation failure gracefully
            return;
        }
        r->bank = this;
        r->next = nullptr;
        temp->next = r;    }
    load();
}

void RegisterBank::load() {
     if(_memoryBlock) {
        _memoryBlock->loadBlock(
            (uint8_t*) _registers,
            _size * sizeof(uint16_t)
        );
     }
}
void RegisterBank::save() {
    if(_memoryBlock) {
        _memoryBlock->saveBlock(
            (uint8_t*) _registers,
            _size * sizeof(uint16_t)
        );
    }
}
RegisterBank *RegisterBank::find(uint16_t fullAddress) {
    // Performance optimization: check cache first
    if (lastAccessedBank != nullptr && 
        lastAccessedAddress == fullAddress && 
        lastAccessedBank->_start <= fullAddress && 
        lastAccessedBank->_stop > fullAddress) {
        return lastAccessedBank;
    }
    
    // Search through registry
    struct Register *temp = registers;
    while(temp != nullptr) {
        if(temp->bank->_start <= fullAddress && temp->bank->_stop > fullAddress) {
            // Update cache
            lastAccessedBank = temp->bank;
            lastAccessedAddress = fullAddress;
            return temp->bank;
        }
        temp = temp->next;
    }
    return nullptr;
}
void RegisterBank::setValue(uint16_t regAddress, uint16_t value, bool instantSave) {
    _registers[regAddress] = value;
    if(instantSave) save();
}
uint16_t RegisterBank::getValue(uint16_t regAddress) {
    return _registers[regAddress];
}
uint16_t *RegisterBank::getValuePtr(uint16_t regAddress) {
    return _registers + regAddress;
}
void RegisterBank::setRegister(uint16_t fullAddress, uint16_t value, bool instantSave) {
    _registers[fullAddress-_start] = value;
    if(instantSave) save();
}
uint16_t RegisterBank::getRegister(uint16_t fullAddress) {
    return _registers[fullAddress-_start];
}
uint16_t *RegisterBank::getRegisterPtr(uint16_t fullAddress) {
    return _registers + (fullAddress-_start);
}
void RegisterBank::free_bank() {
    free(_registers);
}
uint16_t RegisterBank::readRegisters(uint16_t *buffer, uint16_t address, uint16_t size) {
    uint16_t addr = address - _start;
    uint16_t j = 0;
    for (uint16_t i = 0; i < size; i++) {
        if(address + i > _stop) break;
        buffer[j++] = _registers[addr+i];
    }
    return j;
}
