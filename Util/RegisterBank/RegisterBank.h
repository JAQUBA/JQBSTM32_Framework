#ifndef __REGISTERS_H_
#define __REGISTERS_H_
#include "../../Core.h"
#include "../EEPROM/EEPROM.h"

class RegisterBank {
    public:
        RegisterBank(uint16_t regAddress, uint16_t size);
        RegisterBank(uint16_t regAddress, uint16_t size, EEPROM *eeprom);

        static void init();

        uint16_t *getValuePtr(uint16_t regAddress);
        uint16_t *getRegisterPtr(uint16_t fullAddress);

        uint16_t getValue(uint16_t regAddress);
        void setValue(uint16_t regAddress, uint16_t value);

        uint16_t getRegister(uint16_t fullAddress);
        void setRegister(uint16_t fullAddress, uint16_t value);
        
        void free_bank();
        static RegisterBank *find(uint16_t fullAddress);
        uint16_t readRegisters(uint16_t *buffer, uint16_t address, uint16_t size);
    private:
        void _initialize();
        EEPROM *_eeprom;
        static uint16_t _eepromOffset;
        uint16_t _eepromLocation;

        bool _preserve;
        uint16_t _size;
        uint16_t _start;
        uint16_t _stop;
        uint16_t *_registers;

        void load();
        void save();
};
#endif