#ifndef __REGISTERS_H_
#define __REGISTERS_H_

#include "../../Core.h"
#include "../../Util/MemoryBlock/MemoryBlock.h"

/**
 * @brief Class representing a bank of registers.
 */
class RegisterBank {
    public:
        /**
         * @brief Constructs a RegisterBank with the specified address and size.
         * 
         * @param regAddress The starting address of the register bank.
         * @param size The size of the register bank.
         */
        RegisterBank(uint16_t regAddress, uint16_t size);
        
        /**
         * @brief Constructs a RegisterBank with the specified address, size, and memory block.
         * 
         * @param regAddress The starting address of the register bank.
         * @param size The size of the register bank.
         * @param memoryBlock Pointer to the memory block.
         */
        RegisterBank(uint16_t regAddress, uint16_t size, MemoryBlock *memoryBlock);

        /**
         * @brief Gets a pointer to the value of a register at the specified address.
         * 
         * @param regAddress The address of the register.
         * @return uint16_t* Pointer to the value of the register.
         */
        uint16_t *getValuePtr(uint16_t regAddress);

        /**
         * @brief Gets a pointer to the value of a register at the specified full address.
         * 
         * @param fullAddress The full address of the register.
         * @return uint16_t* Pointer to the value of the register.
         */
        uint16_t *getRegisterPtr(uint16_t fullAddress);

        /**
         * @brief Gets the value of a register at the specified address.
         * 
         * @param regAddress The address of the register.
         * @return uint16_t The value of the register.
         */
        uint16_t getValue(uint16_t regAddress);

        /**
         * @brief Sets the value of a register at the specified address.
         * 
         * @param regAddress The address of the register.
         * @param value The value to set the register to.
         * @param instantSave Whether to save the value immediately (default is true).
         */
        void setValue(uint16_t regAddress, uint16_t value, bool instantSave = true);

        /**
         * @brief Gets the value of a register at the specified full address.
         * 
         * @param fullAddress The full address of the register.
         * @return uint16_t The value of the register.
         */
        uint16_t getRegister(uint16_t fullAddress);

        /**
         * @brief Sets the value of a register at the specified full address.
         * 
         * @param fullAddress The full address of the register.
         * @param value The value to set the register to.
         * @param instantSave Whether to save the value immediately (default is true).
         */
        void setRegister(uint16_t fullAddress, uint16_t value, bool instantSave = true);

        /**
         * @brief Frees the memory allocated for the register bank.
         */
        void free_bank();

        /**
         * @brief Finds a RegisterBank instance by the full address.
         * 
         * @param fullAddress The full address to find the RegisterBank instance.
         * @return RegisterBank* Pointer to the found RegisterBank instance.
         */
        static RegisterBank *find(uint16_t fullAddress);

        /**
         * @brief Reads multiple registers into a buffer.
         * 
         * @param buffer The buffer to read the registers into.
         * @param address The starting address to read from.
         * @param size The number of registers to read.
         * @return uint16_t The number of registers read.
         */
        uint16_t readRegisters(uint16_t *buffer, uint16_t address, uint16_t size);

        /**
         * @brief Loads the register bank from persistent storage.
         */
        void load();

        /**
         * @brief Saves the register bank to persistent storage.
         */
        void save();

    private:
        MemoryBlock *_memoryBlock = nullptr; ///< Pointer to the memory block.

        uint16_t _size;   ///< Size of the register bank.
        uint16_t _start;  ///< Start address of the register bank.
        uint16_t _stop;   ///< Stop address of the register bank.
        uint16_t *_registers; ///< Pointer to the array of registers.

        /**
         * @brief Initializes the register bank.
         */
        void _initialize();
};

#endif // __REGISTERS_H_