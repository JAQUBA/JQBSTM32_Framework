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
struct RegisterBankEntry {
    RegisterBank *bank;
    bool active;
    
    RegisterBankEntry() : bank(nullptr), active(false) {}
};

static RegisterBankEntry registeredBanks[REGISTER_BANK_MAX_INSTANCES];
static uint8_t registeredBankCount = 0;

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
    _initialized = false;
    
    // Check size limit
    if(_size == 0 || _size > REGISTER_BANK_MAX_SIZE) {
        _size = 0;
        return;
    }
    
    // Zero-initialize the register array
    for(uint16_t i = 0; i < _size; i++) {
        _registers[i] = 0;
    }
    
    // Register this bank in global registry
    if(registeredBankCount < REGISTER_BANK_MAX_INSTANCES) {
        for(uint8_t i = 0; i < REGISTER_BANK_MAX_INSTANCES; i++) {
            if(!registeredBanks[i].active) {
                registeredBanks[i].bank = this;
                registeredBanks[i].active = true;
                registeredBankCount++;
                _initialized = true;
                break;
            }
        }
    }
    
    if(_initialized) {
        load();
    }
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
    for(uint8_t i = 0; i < REGISTER_BANK_MAX_INSTANCES; i++) {
        if(registeredBanks[i].active && registeredBanks[i].bank != nullptr) {
            RegisterBank* bank = registeredBanks[i].bank;
            if(bank->_start <= fullAddress && bank->_stop > fullAddress) {
                // Update cache
                lastAccessedBank = bank;
                lastAccessedAddress = fullAddress;
                return bank;
            }
        }
    }
    return nullptr;
}
void RegisterBank::setValue(uint16_t regAddress, uint16_t value, bool instantSave) {
    if(!_initialized || regAddress >= _size) return;
    _registers[regAddress] = value;
    if(instantSave) save();
}
uint16_t RegisterBank::getValue(uint16_t regAddress) {
    if(!_initialized || regAddress >= _size) return 0;
    return _registers[regAddress];
}
uint16_t *RegisterBank::getValuePtr(uint16_t regAddress) {
    if(!_initialized || regAddress >= _size) return nullptr;
    return _registers + regAddress;
}
void RegisterBank::setRegister(uint16_t fullAddress, uint16_t value, bool instantSave) {
    if(!_initialized || fullAddress < _start || fullAddress >= _stop) return;
    _registers[fullAddress-_start] = value;
    if(instantSave) save();
}
uint16_t RegisterBank::getRegister(uint16_t fullAddress) {
    if(!_initialized || fullAddress < _start || fullAddress >= _stop) return 0;
    return _registers[fullAddress-_start];
}
uint16_t *RegisterBank::getRegisterPtr(uint16_t fullAddress) {
    if(!_initialized || fullAddress < _start || fullAddress >= _stop) return nullptr;
    return _registers + (fullAddress-_start);
}
void RegisterBank::free_bank() {
    // Remove from global registry
    for(uint8_t i = 0; i < REGISTER_BANK_MAX_INSTANCES; i++) {
        if(registeredBanks[i].active && registeredBanks[i].bank == this) {
            registeredBanks[i].bank = nullptr;
            registeredBanks[i].active = false;
            registeredBankCount--;
            break;
        }
    }
    
    // Clear cache if it points to this bank
    if(lastAccessedBank == this) {
        lastAccessedBank = nullptr;
        lastAccessedAddress = 0xFFFF;
    }
    
    _initialized = false;
    _size = 0;
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
