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

#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace {
RegisterBank *firstBank = nullptr;
}

RegisterBank::RegisterBank(uint16_t start, uint16_t size)
    : _size(size), _start(start) {
    _initialize();
}

RegisterBank::RegisterBank(uint16_t start, uint16_t size, MemoryBlock *memoryBlock)
    : _memoryBlock(memoryBlock), _size(size), _start(start) {
    _initialize();
}

void RegisterBank::_initialize() {
    const uint32_t stop = static_cast<uint32_t>(_start) + _size;
    const bool persistentSizeValid = _memoryBlock == nullptr ||
        _size <= UINT16_MAX / sizeof(uint16_t);
    if (_size == 0U || stop > static_cast<uint32_t>(UINT16_MAX) + 1U ||
        !persistentSizeValid) {
        _size = 0U;
        return;
    }

    _registers = static_cast<uint16_t*>(std::calloc(_size, sizeof(uint16_t)));
    if (_registers == nullptr) {
        _size = 0U;
        return;
    }

    RegisterBank **tail = &firstBank;
    while (*tail != nullptr) {
        tail = &((*tail)->_next);
    }
    *tail = this;

    load();
}

void RegisterBank::load() {
    if (_memoryBlock != nullptr && _registers != nullptr) {
        _memoryBlock->loadBlock(
            reinterpret_cast<uint8_t*>(_registers),
            static_cast<uint16_t>(_size * sizeof(uint16_t))
        );
    }
}

void RegisterBank::save() {
    if (_memoryBlock != nullptr && _registers != nullptr) {
        _memoryBlock->saveBlock(
            reinterpret_cast<uint8_t*>(_registers),
            static_cast<uint16_t>(_size * sizeof(uint16_t))
        );
    }
}

RegisterBank *RegisterBank::find(uint16_t fullAddress) {
    RegisterBank *bank = firstBank;
    while (bank != nullptr) {
        if (fullAddress >= bank->_start && fullAddress - bank->_start < bank->_size) {
            return bank;
        }
        bank = bank->_next;
    }
    return nullptr;
}

void RegisterBank::setValue(uint16_t regAddress, uint16_t value, bool instantSave) {
    if (_registers == nullptr || regAddress >= _size) return;
    _registers[regAddress] = value;
    if (instantSave) save();
}

uint16_t RegisterBank::getValue(uint16_t regAddress) {
    if (_registers == nullptr || regAddress >= _size) return 0U;
    return _registers[regAddress];
}

uint16_t *RegisterBank::getValuePtr(uint16_t regAddress) {
    if (_registers == nullptr || regAddress >= _size) return nullptr;
    return _registers + regAddress;
}

uint32_t *RegisterBank::getValuePtr32(uint16_t regAddress) {
    if (_registers == nullptr || regAddress >= _size ||
        static_cast<uint16_t>(_size - regAddress) < 2U) return nullptr;
    uint16_t *value = _registers + regAddress;
    if (reinterpret_cast<uintptr_t>(value) % alignof(uint32_t) != 0U) return nullptr;
    return reinterpret_cast<uint32_t*>(value);
}

uint32_t RegisterBank::getValue32(uint16_t regAddress) {
    if (_registers == nullptr || regAddress >= _size ||
        static_cast<uint16_t>(_size - regAddress) < 2U) return 0U;
    return static_cast<uint32_t>(_registers[regAddress]) |
        (static_cast<uint32_t>(_registers[regAddress + 1U]) << 16U);
}

void RegisterBank::setValue32(uint16_t regAddress, uint32_t value, bool instantSave) {
    if (_registers == nullptr || regAddress >= _size ||
        static_cast<uint16_t>(_size - regAddress) < 2U) return;
    _registers[regAddress] = static_cast<uint16_t>(value);
    _registers[regAddress + 1U] = static_cast<uint16_t>(value >> 16U);
    if (instantSave) save();
}

bool RegisterBank::setRegister(uint16_t fullAddress, uint16_t value, bool instantSave) {
    if (_registers == nullptr || fullAddress < _start || fullAddress - _start >= _size) return false;
    _registers[fullAddress - _start] = value;
    if (instantSave) save();
    return true;
}

bool RegisterBank::setRegister32(uint16_t fullAddress, uint32_t value, bool instantSave) {
    if (_registers == nullptr || fullAddress < _start || fullAddress - _start >= _size) return false;
    const uint16_t regAddress = fullAddress - _start;
    if (static_cast<uint16_t>(_size - regAddress) < 2U) return false;
    _registers[regAddress] = static_cast<uint16_t>(value);
    _registers[regAddress + 1U] = static_cast<uint16_t>(value >> 16U);
    if (instantSave) save();
    return true;
}

uint16_t RegisterBank::setRegisters(uint16_t fullAddress, const uint16_t *buffer,
    uint16_t size, bool instantSave) {
    if (_registers == nullptr || buffer == nullptr || fullAddress < _start ||
        fullAddress - _start >= _size) return 0U;
    const uint16_t regAddress = fullAddress - _start;
    const uint16_t available = _size - regAddress;
    const uint16_t written = size < available ? size : available;
    (void)std::memcpy(_registers + regAddress, buffer, written * sizeof(uint16_t));
    if (instantSave && written > 0U) save();
    return written;
}

uint16_t RegisterBank::getRegister(uint16_t fullAddress) {
    if (_registers == nullptr || fullAddress < _start || fullAddress - _start >= _size) return 0U;
    return _registers[fullAddress - _start];
}

uint16_t *RegisterBank::getRegisterPtr(uint16_t fullAddress) {
    if (_registers == nullptr || fullAddress < _start || fullAddress - _start >= _size) return nullptr;
    return _registers + (fullAddress - _start);
}

uint32_t *RegisterBank::getRegisterPtr32(uint16_t fullAddress) {
    if (fullAddress < _start) return nullptr;
    return getValuePtr32(fullAddress - _start);
}

uint32_t RegisterBank::getRegister32(uint16_t fullAddress) {
    if (fullAddress < _start) return 0U;
    return getValue32(fullAddress - _start);
}

void RegisterBank::free_bank() {
    RegisterBank **bank = &firstBank;
    while (*bank != nullptr) {
        if (*bank == this) {
            *bank = _next;
            break;
        }
        bank = &((*bank)->_next);
    }

    std::free(_registers);
    _registers = nullptr;
    _next = nullptr;
    _size = 0U;
}

uint16_t RegisterBank::readRegisters(uint16_t address, uint16_t *buffer, uint16_t size) {
    if (_registers == nullptr || buffer == nullptr || address < _start ||
        address - _start >= _size) return 0U;
    const uint16_t regAddress = address - _start;
    const uint16_t available = _size - regAddress;
    const uint16_t read = size < available ? size : available;
    (void)std::memcpy(buffer, _registers + regAddress, read * sizeof(uint16_t));
    return read;
}