/*
 * JQBSTM32 Framework - RegisterBank.cpp Implementation
 * Copyright (C) 2024 JAQUBA (kjakubowski0492@gmail.com)
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <https://www.gnu.org/licenses/>.
 */
#include "RegisterBank.h"

struct Register {
    RegisterBank *bank;
    struct Register *next = NULL;
} *registers;

RegisterBank::RegisterBank(uint16_t start,
    uint16_t size) {

    _size = size;
    _start = start;
    _stop = _start + size;

    _initialize();
}

RegisterBank::RegisterBank(uint16_t start,
    uint16_t size,
    MemoryBlock *memoryBlock) {
        
    _size = size;
    _start = start;
    _stop = _start + size;

    _memoryBlock = memoryBlock;

    _initialize();
}
void RegisterBank::_initialize() {
    if(_size > 0) {
        _registers = (uint16_t*)malloc(sizeof(uint16_t) * _size);
        if(_registers == NULL) {
            return;
        }
    } else {
        _registers = NULL;
    }
    (void)memset(_registers, 0, sizeof(uint16_t) * _size);

    struct Register *temp = registers, *r;
    if(registers == NULL) {
        temp = (struct Register*)malloc(sizeof(struct Register));
        if (temp == NULL) {
            free(_registers);
            _registers = NULL;
            return;
        }
        temp->bank = this;
        temp->next = NULL;
        registers = temp;
    } else {
        while(temp->next != NULL) temp = temp->next;
        r = (struct Register*) malloc(sizeof(struct Register));
        if (r == NULL) {
            free(_registers);
            _registers = NULL;
            return;
        }
        r->bank = this;
        r->next = NULL;
        temp->next = r;
    }
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
    struct Register *temp = registers;
    while(temp != NULL) {
        if(temp->bank->_start <= fullAddress && fullAddress < temp->bank->_stop) return temp->bank;
        temp = temp->next;
    }
    return NULL;
}
void RegisterBank::setValue(uint16_t regAddress, uint16_t value, bool instantSave) {
	if (_registers == NULL || regAddress >= _size) return;
    _registers[regAddress] = value;
    if(instantSave) save();
}
uint16_t RegisterBank::getValue(uint16_t regAddress) {
	if (_registers == NULL || regAddress >= _size) return 0;
    return _registers[regAddress];
}
uint16_t *RegisterBank::getValuePtr(uint16_t regAddress) {
	if (_registers == NULL || regAddress >= _size) return NULL;
    return _registers + regAddress;
}
uint32_t *RegisterBank::getValuePtr32(uint16_t regAddress) {
    if (_registers == NULL || regAddress >= _size || _size - regAddress < 2) return NULL;
    return reinterpret_cast<uint32_t*>(_registers + regAddress);
}
uint32_t RegisterBank::getValue32(uint16_t regAddress) {
    if (_registers == NULL || regAddress >= _size || _size - regAddress < 2) return 0U;
    return (uint32_t)_registers[regAddress] |
        ((uint32_t)_registers[regAddress + 1U] << 16U);
}
void RegisterBank::setValue32(uint16_t regAddress, uint32_t value, bool instantSave) {
	if (_registers == NULL || regAddress >= _size || _size - regAddress < 2) return;
    _registers[regAddress] = (uint16_t)value;
    _registers[regAddress + 1U] = (uint16_t)(value >> 16U);
    if(instantSave) save();
}
bool RegisterBank::setRegister(uint16_t fullAddress, uint16_t value, bool instantSave) {
	if (_registers == NULL || fullAddress < _start || fullAddress >= _stop) return false;
    _registers[fullAddress-_start] = value;
    if(instantSave) save();
    return true;
}
bool RegisterBank::setRegister32(uint16_t fullAddress, uint32_t value, bool instantSave) {
	if (_registers == NULL || fullAddress < _start || fullAddress >= _stop ||
    _stop - fullAddress < 2) return false;
    const uint16_t regAddress = fullAddress - _start;
    _registers[regAddress] = (uint16_t)value;
    _registers[regAddress + 1U] = (uint16_t)(value >> 16U);
    if(instantSave) save();
    return true;
}
uint16_t RegisterBank::setRegisters(uint16_t fullAddress, const uint16_t *buffer, 
    uint16_t size, bool instantSave) {
	if (_registers == NULL || buffer == NULL || fullAddress < _start || fullAddress >= _stop) return 0;
    uint16_t written = 0;
    while (written < size && fullAddress + written < _stop) {
        _registers[fullAddress + written - _start] = buffer[written];
        written++;
    }
    if(instantSave && written > 0) save();
    return written;
}
uint16_t RegisterBank::getRegister(uint16_t fullAddress) {
	if (_registers == NULL || fullAddress < _start || fullAddress >= _stop) return 0;
    return _registers[fullAddress-_start];
}
uint16_t *RegisterBank::getRegisterPtr(uint16_t fullAddress) {
	if (_registers == NULL || fullAddress < _start || fullAddress >= _stop) return NULL;
    return _registers + (fullAddress-_start);
}
uint32_t *RegisterBank::getRegisterPtr32(uint16_t fullAddress) {
	if (_registers == NULL || fullAddress < _start || fullAddress >= _stop ||
    _stop - fullAddress < 2) return NULL;
    return reinterpret_cast<uint32_t*>(_registers + (fullAddress-_start));
}
uint32_t RegisterBank::getRegister32(uint16_t fullAddress) {
	if (_registers == NULL || fullAddress < _start || fullAddress >= _stop ||
    _stop - fullAddress < 2) return 0U;
    return getValue32(fullAddress - _start);
}
void RegisterBank::free_bank() {
    free(_registers);
}
uint16_t RegisterBank::readRegisters(uint16_t address, uint16_t *buffer, uint16_t size) {
	if (_registers == NULL || buffer == NULL || address < _start || address >= _stop) return 0;
    uint16_t addr = address - _start;
    uint16_t j = 0;
    for (uint16_t i = 0; i < size; i++) {
        if(address + i >= _stop) break;
        buffer[j++] = _registers[addr+i];
    }
    return j;
}
