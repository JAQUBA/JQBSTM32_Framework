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
            free(_registers);
            return;
        }
    } else {
        _registers = NULL;
    }
    (void)memset(_registers, 0, sizeof(uint16_t) * _size);

    struct Register *temp = registers, *r;
    if(registers == NULL) {
        temp = (struct Register*)malloc(sizeof(struct Register));
        temp->bank = this;
        temp->next = NULL;
        registers = temp;
    } else {
        while(temp->next != NULL) temp = temp->next;
        r = (struct Register*) malloc(sizeof(struct Register));
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
        if(temp->bank->_start <= fullAddress && temp->bank->_stop >= fullAddress) return temp->bank;
        temp = temp->next;
    }
    return NULL;
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