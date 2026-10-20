/*
 * JQBSTM32 Framework - RegisterBankRef.cpp Implementation
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
#include "RegisterBankRef.h"

namespace {
enum Operation : uint8_t {
    Assign,
    Increment,
    Decrement,
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    BitAnd,
    BitOr,
    BitXor,
    ShiftLeft,
    ShiftRight
};

uint32_t lockInterrupts() {
    const uint32_t interruptState = __get_PRIMASK();
    __disable_irq();
    return interruptState;
}

void unlockInterrupts(uint32_t interruptState) {
    if (interruptState == 0U) {
        __enable_irq();
    }
}

template<typename T>
T applyOperation(T current, uint8_t operation, T value) {
    switch (operation) {
        case Assign:     return value;
        case Increment:  return static_cast<T>(current + 1U);
        case Decrement:  return static_cast<T>(current - 1U);
        case Add:        return static_cast<T>(current + value);
        case Subtract:   return static_cast<T>(current - value);
        case Multiply:   return static_cast<T>(current * value);
        case Divide:     return value == 0U ? current : static_cast<T>(current / value);
        case Modulo:     return value == 0U ? current : static_cast<T>(current % value);
        case BitAnd:     return static_cast<T>(current & value);
        case BitOr:      return static_cast<T>(current | value);
        case BitXor:     return static_cast<T>(current ^ value);
        case ShiftLeft:
            return value >= sizeof(T) * 8U ? 0U : static_cast<T>(current << value);
        case ShiftRight:
            return value >= sizeof(T) * 8U ? 0U : static_cast<T>(current >> value);
        default:         return current;
    }
}
}

RegisterRef16::RegisterRef16(RegisterBankRef *bank, uint16_t index)
    : _bank(bank), _index(index) {
}

bool RegisterRef16::isValid() const {
    return _bank != nullptr && _bank->isValidReference(_index, 1U);
}

RegisterRef16::operator uint16_t() const {
    return _bank == nullptr ? 0U : _bank->readReference16(_index);
}

RegisterRef16 &RegisterRef16::operator=(uint16_t value) {
    if (_bank != nullptr) _bank->writeReference16(_index, value);
    return *this;
}

RegisterRef16 &RegisterRef16::operator=(const RegisterRef16 &other) {
    return *this = static_cast<uint16_t>(other);
}

uint16_t RegisterRef16::apply(uint8_t operation, uint16_t value) {
    return _bank == nullptr ? 0U : _bank->applyReference16(_index, operation, value);
}

RegisterRef16 &RegisterRef16::operator++() { (void)apply(Increment, 0U); return *this; }
uint16_t RegisterRef16::operator++(int) { return apply(Increment, 0U); }
RegisterRef16 &RegisterRef16::operator--() { (void)apply(Decrement, 0U); return *this; }
uint16_t RegisterRef16::operator--(int) { return apply(Decrement, 0U); }
RegisterRef16 &RegisterRef16::operator+=(uint16_t value) { (void)apply(Add, value); return *this; }
RegisterRef16 &RegisterRef16::operator-=(uint16_t value) { (void)apply(Subtract, value); return *this; }
RegisterRef16 &RegisterRef16::operator*=(uint16_t value) { (void)apply(Multiply, value); return *this; }
RegisterRef16 &RegisterRef16::operator/=(uint16_t value) { (void)apply(Divide, value); return *this; }
RegisterRef16 &RegisterRef16::operator%=(uint16_t value) { (void)apply(Modulo, value); return *this; }
RegisterRef16 &RegisterRef16::operator&=(uint16_t value) { (void)apply(BitAnd, value); return *this; }
RegisterRef16 &RegisterRef16::operator|=(uint16_t value) { (void)apply(BitOr, value); return *this; }
RegisterRef16 &RegisterRef16::operator^=(uint16_t value) { (void)apply(BitXor, value); return *this; }
RegisterRef16 &RegisterRef16::operator<<=(uint8_t value) { (void)apply(ShiftLeft, value); return *this; }
RegisterRef16 &RegisterRef16::operator>>=(uint8_t value) { (void)apply(ShiftRight, value); return *this; }

RegisterRef32::RegisterRef32(RegisterBankRef *bank, uint16_t index)
    : _bank(bank), _index(index) {
}

bool RegisterRef32::isValid() const {
    return _bank != nullptr && _bank->isValidReference(_index, 2U);
}

RegisterRef32::operator uint32_t() const {
    return _bank == nullptr ? 0U : _bank->readReference32(_index);
}

RegisterRef32 &RegisterRef32::operator=(uint32_t value) {
    if (_bank != nullptr) _bank->writeReference32(_index, value);
    return *this;
}

RegisterRef32 &RegisterRef32::operator=(const RegisterRef32 &other) {
    return *this = static_cast<uint32_t>(other);
}

uint32_t RegisterRef32::apply(uint8_t operation, uint32_t value) {
    return _bank == nullptr ? 0U : _bank->applyReference32(_index, operation, value);
}

RegisterRef32 &RegisterRef32::operator++() { (void)apply(Increment, 0U); return *this; }
uint32_t RegisterRef32::operator++(int) { return apply(Increment, 0U); }
RegisterRef32 &RegisterRef32::operator--() { (void)apply(Decrement, 0U); return *this; }
uint32_t RegisterRef32::operator--(int) { return apply(Decrement, 0U); }
RegisterRef32 &RegisterRef32::operator+=(uint32_t value) { (void)apply(Add, value); return *this; }
RegisterRef32 &RegisterRef32::operator-=(uint32_t value) { (void)apply(Subtract, value); return *this; }
RegisterRef32 &RegisterRef32::operator*=(uint32_t value) { (void)apply(Multiply, value); return *this; }
RegisterRef32 &RegisterRef32::operator/=(uint32_t value) { (void)apply(Divide, value); return *this; }
RegisterRef32 &RegisterRef32::operator%=(uint32_t value) { (void)apply(Modulo, value); return *this; }
RegisterRef32 &RegisterRef32::operator&=(uint32_t value) { (void)apply(BitAnd, value); return *this; }
RegisterRef32 &RegisterRef32::operator|=(uint32_t value) { (void)apply(BitOr, value); return *this; }
RegisterRef32 &RegisterRef32::operator^=(uint32_t value) { (void)apply(BitXor, value); return *this; }
RegisterRef32 &RegisterRef32::operator<<=(uint8_t value) { (void)apply(ShiftLeft, value); return *this; }
RegisterRef32 &RegisterRef32::operator>>=(uint8_t value) { (void)apply(ShiftRight, value); return *this; }

RegisterBankRef::RegisterBankRef(uint16_t regAddress, uint16_t size, uint32_t saveInterval)
    : RegisterBank(regAddress, size), _saveInterval(saveInterval) {
    initializeTask();
}

RegisterBankRef::RegisterBankRef(uint16_t regAddress, uint16_t size,
    MemoryBlock *memoryBlock, uint32_t saveInterval)
    : RegisterBank(regAddress, size, memoryBlock), _saveInterval(saveInterval) {
    initializeTask();
}

void RegisterBankRef::initializeTask() {
    addTaskMain([this](taskStruct*) {
        const uint32_t interruptState = lockInterrupts();
        const bool saveRequested = _saveRequested;
        _saveRequested = false;
        unlockInterrupts(interruptState);

        if (saveRequested) {
            save();
        }
    }, _saveInterval);
}

RegisterRef16 RegisterBankRef::getValueRef(uint16_t regAddress) {
    return isValidReference(regAddress, 1U) ? RegisterRef16(this, regAddress) : RegisterRef16();
}

RegisterRef32 RegisterBankRef::getValueRef32(uint16_t regAddress) {
    return isValidReference(regAddress, 2U) ? RegisterRef32(this, regAddress) : RegisterRef32();
}

RegisterRef16 RegisterBankRef::getRegisterRef(uint16_t fullAddress) {
    if (fullAddress < _start) return RegisterRef16();
    return getValueRef(fullAddress - _start);
}

RegisterRef32 RegisterBankRef::getRegisterRef32(uint16_t fullAddress) {
    if (fullAddress < _start) return RegisterRef32();
    return getValueRef32(fullAddress - _start);
}

bool RegisterBankRef::isValidReference(uint16_t index, uint16_t width) const {
    return _registers != nullptr && index < _size && width <= _size - index;
}

uint16_t RegisterBankRef::readReference16(uint16_t index) const {
    if (!isValidReference(index, 1U)) return 0U;
    const uint32_t interruptState = lockInterrupts();
    const uint16_t value = _registers[index];
    unlockInterrupts(interruptState);
    return value;
}

uint32_t RegisterBankRef::readReference32(uint16_t index) const {
    if (!isValidReference(index, 2U)) return 0U;
    const uint32_t interruptState = lockInterrupts();
    const uint32_t value = static_cast<uint32_t>(_registers[index]) |
        (static_cast<uint32_t>(_registers[index + 1U]) << 16U);
    unlockInterrupts(interruptState);
    return value;
}

void RegisterBankRef::writeReference16(uint16_t index, uint16_t value) {
    if (!isValidReference(index, 1U)) return;
    const uint32_t interruptState = lockInterrupts();
    _registers[index] = value;
    unlockInterrupts(interruptState);
    requestSave();
}

void RegisterBankRef::writeReference32(uint16_t index, uint32_t value) {
    if (!isValidReference(index, 2U)) return;
    const uint32_t interruptState = lockInterrupts();
    _registers[index] = static_cast<uint16_t>(value);
    _registers[index + 1U] = static_cast<uint16_t>(value >> 16U);
    unlockInterrupts(interruptState);
    requestSave();
}

uint16_t RegisterBankRef::applyReference16(uint16_t index, uint8_t operation, uint16_t value) {
    if (!isValidReference(index, 1U)) return 0U;
    const uint32_t interruptState = lockInterrupts();
    const uint16_t previous = _registers[index];
    _registers[index] = applyOperation(previous, operation, value);
    unlockInterrupts(interruptState);
    requestSave();
    return previous;
}

uint32_t RegisterBankRef::applyReference32(uint16_t index, uint8_t operation, uint32_t value) {
    if (!isValidReference(index, 2U)) return 0U;
    const uint32_t interruptState = lockInterrupts();
    const uint32_t previous = static_cast<uint32_t>(_registers[index]) |
        (static_cast<uint32_t>(_registers[index + 1U]) << 16U);
    const uint32_t result = applyOperation(previous, operation, value);
    _registers[index] = static_cast<uint16_t>(result);
    _registers[index + 1U] = static_cast<uint16_t>(result >> 16U);
    unlockInterrupts(interruptState);
    requestSave();
    return previous;
}

void RegisterBankRef::requestSave() {
    if (_memoryBlock != nullptr) {
        _saveRequested = true;
    }
}