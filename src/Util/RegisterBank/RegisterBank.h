/*
 * JQBSTM32 Framework - RegisterBank.h Header
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
#ifndef __REGISTERS_H_
#define __REGISTERS_H_

#include "../../Core.h"
#include "../../Util/MemoryBlock/MemoryBlock.h"

/**
 * @class RegisterBank
 * @brief Register bank management system for configuration and data storage
 * @details This class provides a structured way to manage registers (16-bit values) with
 *          support for persistent storage via MemoryBlock. Commonly used for device
 *          configuration, parameter storage, and Modbus-style register mapping.
 *          Supports both volatile (RAM-only) and persistent (external memory) register banks.
 * @note All register addresses are 16-bit values, suitable for Modbus RTU applications
 */
class RegisterBank {
    public:
        /**
         * @brief Constructor for volatile register bank
         * @param regAddress Starting address of the register bank
         * @param size Number of 16-bit registers in the bank
         * @details Creates register bank in RAM without persistent storage capability.
         *          Registers are initialized to zero and lost on power cycle.
         * @note Register addresses range from regAddress to (regAddress + size - 1)
         */
        RegisterBank(uint16_t regAddress, uint16_t size);
        
        /**
         * @brief Constructor for persistent register bank with external memory
         * @param regAddress Starting address of the register bank
         * @param size Number of 16-bit registers in the bank
         * @param memoryBlock Pointer to MemoryBlock for persistent storage
         * @details Creates register bank with persistent storage capability via MemoryBlock.
         *          Registers can be saved to and loaded from external memory (EEPROM, FRAM, etc.).
         * @note Memory block should have sufficient capacity for (size * 2) bytes
         */
        RegisterBank(uint16_t regAddress, uint16_t size, MemoryBlock *memoryBlock);

        /**
         * @brief Get pointer to register value using relative address
         * @param regAddress Relative address within this register bank (0-based offset)
         * @return Pointer to 16-bit register value, NULL if address is out of range
         * @details Returns direct pointer to register for efficient access and modification.
         *          Address is relative to bank start address.
         * @note Use with caution - direct pointer access bypasses bounds checking
         */
        uint16_t *getValuePtr(uint16_t regAddress);

        /**
         * @brief Get pointer to register value using full/absolute address
         * @param fullAddress Absolute register address in global register space
         * @return Pointer to 16-bit register value, NULL if address is not in this bank
         * @details Returns direct pointer to register using global addressing.
         *          Automatically calculates offset within this bank.
         * @note Address must fall within this bank's address range
         */
        uint16_t *getRegisterPtr(uint16_t fullAddress);

        /**
         * @brief Get register value using relative address
         * @param regAddress Relative address within this register bank (0-based offset)
         * @return 16-bit register value, 0 if address is out of range
         * @details Safe method to read register value with bounds checking.
         *          Address is relative to bank start address.
         */
        uint16_t getValue(uint16_t regAddress);

        /**
         * @brief Set register value using relative address
         * @param regAddress Relative address within this register bank (0-based offset)
         * @param value 16-bit value to write to register
         * @param instantSave If true and MemoryBlock is available, immediately save to persistent storage
         * @details Sets register value with optional immediate persistence.
         *          Address is relative to bank start address.
         * @note Bounds checking is performed - invalid addresses are ignored
         */
        void setValue(uint16_t regAddress, uint16_t value, bool instantSave = true);

        /**
         * @brief Get register value using full/absolute address
         * @param fullAddress Absolute register address in global register space
         * @return 16-bit register value, 0 if address is not in this bank
         * @details Safe method to read register using global addressing.
         *          Returns 0 if address doesn't belong to this bank.
         */
        uint16_t getRegister(uint16_t fullAddress);

        /**
         * @brief Set register value using full/absolute address
         * @param fullAddress Absolute register address in global register space
         * @param value 16-bit value to write to register
         * @param instantSave If true and MemoryBlock is available, immediately save to persistent storage
         * @details Sets register value using global addressing with optional persistence.
         *          Operation is ignored if address doesn't belong to this bank.
         */
        void setRegister(uint16_t fullAddress, uint16_t value, bool instantSave = true);

        /**
         * @brief Free allocated memory for register bank
         * @details Releases dynamically allocated memory for register array.
         *          RegisterBank instance becomes invalid after this call.
         * @note Should be called before destroying RegisterBank to prevent memory leaks
         */
        void free_bank();

        /**
         * @brief Find RegisterBank instance containing specified address
         * @param fullAddress Absolute register address to search for
         * @return Pointer to RegisterBank containing the address, NULL if not found
         * @details Static method to locate which RegisterBank instance manages
         *          a specific global register address. Useful for register routing.
         * @note Searches through all created RegisterBank instances
         */
        static RegisterBank *find(uint16_t fullAddress);

        /**
         * @brief Read multiple consecutive registers into buffer
         * @param buffer Pointer to buffer for storing read register values
         * @param address Starting relative address within this bank
         * @param size Number of registers to read
         * @return Number of registers actually read (may be less if hitting bank boundary)
         * @details Efficiently reads block of consecutive registers into provided buffer.
         *          Reading stops at bank boundary if requested size exceeds available registers.
         * @note Buffer must have space for at least 'size' uint16_t values
         */
        uint16_t readRegisters(uint16_t *buffer, uint16_t address, uint16_t size);

        /**
         * @brief Load register bank from persistent storage
         * @details Loads all register values from associated MemoryBlock if available.
         *          Operation is ignored if no MemoryBlock is configured.
         * @note Overwrites current register values with stored data
         */
        void load();

        /**
         * @brief Save register bank to persistent storage
         * @details Saves all register values to associated MemoryBlock if available.
         *          Operation is ignored if no MemoryBlock is configured.
         * @note Write operation duration depends on memory type (EEPROM vs FRAM)
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
