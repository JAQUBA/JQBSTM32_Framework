/*
 * JQBSTM32 Framework - RegisterBankRef.h Header
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
#ifndef __REGISTER_BANK_REF_H_
#define __REGISTER_BANK_REF_H_

#include "../RegisterBank/RegisterBank.h"

class RegisterBankRef;

/**
 * @class RegisterRef16
 * @brief Mutable reference to one 16-bit register in a RegisterBankRef.
 * @details Provides value conversion, assignment, increment, decrement,
 *          arithmetic, bitwise, and shift compound operators. Every modifying
 *          operation is atomic relative to interrupts and requests a deferred
 *          save when the owning bank uses persistent memory.
 * @note An invalid reference reads as zero and ignores modifying operations.
 * @note Division or modulo by zero leaves the stored value unchanged.
 */
class RegisterRef16 {
    public:
        /** @brief Construct an invalid reference. */
        RegisterRef16() = default;

        /** @return true when the referenced register is available. */
        bool isValid() const;

        /** @return Current register value, or zero when invalid. */
        operator uint16_t() const;

        /** @name Assignment and increment operators
         * Prefix operators return this reference; postfix operators return the previous value.
         * @{ */
        RegisterRef16 &operator=(uint16_t value);
        RegisterRef16 &operator=(const RegisterRef16 &other);
        RegisterRef16 &operator++();
        uint16_t operator++(int);
        RegisterRef16 &operator--();
        uint16_t operator--(int);
        /** @} */

        /** @name Compound arithmetic and bitwise operators
         * Shift counts greater than or equal to 16 produce zero.
         * @{ */
        RegisterRef16 &operator+=(uint16_t value);
        RegisterRef16 &operator-=(uint16_t value);
        RegisterRef16 &operator*=(uint16_t value);
        RegisterRef16 &operator/=(uint16_t value);
        RegisterRef16 &operator%=(uint16_t value);
        RegisterRef16 &operator&=(uint16_t value);
        RegisterRef16 &operator|=(uint16_t value);
        RegisterRef16 &operator^=(uint16_t value);
        RegisterRef16 &operator<<=(uint8_t value);
        RegisterRef16 &operator>>=(uint8_t value);
        /** @} */

    private:
        friend class RegisterBankRef;
        RegisterRef16(RegisterBankRef *bank, uint16_t index);
        uint16_t apply(uint8_t operation, uint16_t value);

        RegisterBankRef *_bank = nullptr;
        uint16_t _index = 0U;
};

/**
 * @class RegisterRef32
 * @brief Mutable reference to a 32-bit value stored in two consecutive registers.
 * @details The low 16-bit word is stored at the selected register and the high
 *          word at the following register. Reads and modifications of both
 *          words are atomic relative to interrupts. Modifications request a
 *          deferred save when the owning bank uses persistent memory.
 * @note An invalid reference reads as zero and ignores modifying operations.
 * @note Division or modulo by zero leaves the stored value unchanged.
 */
class RegisterRef32 {
    public:
        /** @brief Construct an invalid reference. */
        RegisterRef32() = default;

        /** @return true when both referenced registers are available. */
        bool isValid() const;

        /** @return Current 32-bit value, or zero when invalid. */
        operator uint32_t() const;

        /** @name Assignment and increment operators
         * Prefix operators return this reference; postfix operators return the previous value.
         * @{ */
        RegisterRef32 &operator=(uint32_t value);
        RegisterRef32 &operator=(const RegisterRef32 &other);
        RegisterRef32 &operator++();
        uint32_t operator++(int);
        RegisterRef32 &operator--();
        uint32_t operator--(int);
        /** @} */

        /** @name Compound arithmetic and bitwise operators
         * Shift counts greater than or equal to 32 produce zero.
         * @{ */
        RegisterRef32 &operator+=(uint32_t value);
        RegisterRef32 &operator-=(uint32_t value);
        RegisterRef32 &operator*=(uint32_t value);
        RegisterRef32 &operator/=(uint32_t value);
        RegisterRef32 &operator%=(uint32_t value);
        RegisterRef32 &operator&=(uint32_t value);
        RegisterRef32 &operator|=(uint32_t value);
        RegisterRef32 &operator^=(uint32_t value);
        RegisterRef32 &operator<<=(uint8_t value);
        RegisterRef32 &operator>>=(uint8_t value);
        /** @} */

    private:
        friend class RegisterBankRef;
        RegisterRef32(RegisterBankRef *bank, uint16_t index);
        uint32_t apply(uint8_t operation, uint32_t value);

        RegisterBankRef *_bank = nullptr;
        uint16_t _index = 0U;
};

/**
 * @class RegisterBankRef
 * @brief Register bank providing safe mutable 16-bit and 32-bit references.
 * @details Extends RegisterBank with operator-enabled references. Each instance
 *          automatically registers its own main-scheduler task in the
 *          constructor. The task coalesces modifications and saves the whole
 *          bank at most once per configured interval.
 * @warning The main scheduler must be constructed before a RegisterBankRef
 *          instance is created.
 */
class RegisterBankRef : public RegisterBank {
    public:
        /**
         * @brief Construct a volatile reference-enabled register bank.
         * @param regAddress First absolute register address.
         * @param size Number of 16-bit registers.
         * @param saveInterval Deferred-save task interval in milliseconds.
         */
        RegisterBankRef(uint16_t regAddress, uint16_t size, uint32_t saveInterval = 100U);

        /**
         * @brief Construct a persistent reference-enabled register bank.
         * @param regAddress First absolute register address.
         * @param size Number of 16-bit registers.
         * @param memoryBlock Persistent storage used by load() and save().
         * @param saveInterval Deferred-save task interval in milliseconds.
         */
        RegisterBankRef(uint16_t regAddress, uint16_t size, MemoryBlock *memoryBlock,
            uint32_t saveInterval = 100U);

        /**
         * @brief Get a 16-bit reference by relative register index.
         * @return Valid reference when the index is in range; otherwise an invalid reference.
         */
        RegisterRef16 getValueRef(uint16_t regAddress);

        /**
         * @brief Get a 32-bit reference by relative low-word index.
         * @return Valid reference when two registers are available; otherwise an invalid reference.
         */
        RegisterRef32 getValueRef32(uint16_t regAddress);

        /**
         * @brief Get a 16-bit reference by absolute register address.
         * @return Valid reference when the address belongs to this bank; otherwise an invalid reference.
         */
        RegisterRef16 getRegisterRef(uint16_t fullAddress);

        /**
         * @brief Get a 32-bit reference by absolute low-word address.
         * @return Valid reference when two registers are available; otherwise an invalid reference.
         */
        RegisterRef32 getRegisterRef32(uint16_t fullAddress);

    private:
        friend class RegisterRef16;
        friend class RegisterRef32;

        void initializeTask();
        bool isValidReference(uint16_t index, uint16_t width) const;
        uint16_t readReference16(uint16_t index) const;
        uint32_t readReference32(uint16_t index) const;
        void writeReference16(uint16_t index, uint16_t value);
        void writeReference32(uint16_t index, uint32_t value);
        uint16_t applyReference16(uint16_t index, uint8_t operation, uint16_t value);
        uint32_t applyReference32(uint16_t index, uint8_t operation, uint32_t value);
        void requestSave();

        uint32_t _saveInterval; ///< Deferred-save task interval in milliseconds.
        volatile bool _saveRequested = false; ///< Pending persistent-save request.
};

#endif // __REGISTER_BANK_REF_H_