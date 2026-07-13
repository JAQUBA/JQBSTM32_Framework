/*
 * JQBSTM32 Framework - EEP24Cxx.h Header
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
#include "../../Hardware/I2C/I2C.h"
#ifdef __I2C_H_

#ifndef __EEP24CXX_H_
#define __EEP24CXX_H_

#include "../../Interface/IExternalMemory.h"

class EEP24Cxx : public IExternalMemory {
    public:
        struct DeviceConfig {
            uint32_t sizeBytes;
            uint16_t pageSize;
            uint16_t memAddSize;
            uint8_t blockAddressBits;
        };

        static const DeviceConfig CONFIG_24AA02;
        static const DeviceConfig CONFIG_24LC02B;
        static const DeviceConfig CONFIG_24LC04B;
        static const DeviceConfig CONFIG_24LC08B;
        static const DeviceConfig CONFIG_24LC16B;
        static const DeviceConfig CONFIG_24LC32A;
        static const DeviceConfig CONFIG_24LC64;
        static const DeviceConfig CONFIG_24LC128;
        static const DeviceConfig CONFIG_24LC256;
        static const DeviceConfig CONFIG_24LC512;

        static constexpr uint32_t DEFAULT_TIMEOUT_MS = 10U;

        /**
         * @brief Creates an EEPROM driver with a per-operation I2C timeout.
         * @param pInstance I2C instance used by the EEPROM.
         * @param config EEPROM geometry and addressing configuration.
         * @param DevAddress Base I2C device address (default: 0xA0).
         * @param BaseAddress Memory offset applied to each request (default: 0x00).
         * @param timeoutMs Maximum duration of each I2C operation after it starts (default: 10 ms).
         */
        EEP24Cxx(
            I2C *pInstance,
            const DeviceConfig &config,
            uint16_t DevAddress = 0xA0,
            uint32_t BaseAddress = 0x00,
            uint32_t timeoutMs = DEFAULT_TIMEOUT_MS
        );

        void readFromMemory(
            uint32_t MemAddress,
            uint8_t *pData,
            uint16_t Size
        ) override;

        void writeToMemory(
            uint32_t MemAddress,
            uint8_t *pData,
            uint16_t Size
        ) override;

    protected:
        I2C      *_pInstance;
        uint16_t  _DevAddress;
        uint32_t  _BaseAddress;
        uint32_t  _timeoutMs;

        uint32_t _sizeBytes;
        uint16_t _pageSize;
        uint16_t _memAddSize;
        uint8_t  _blockAddressBits;

        uint32_t normalizeAddress(uint32_t address) const;
        uint16_t getDeviceAddressForOffset(uint32_t memoryOffset) const;
        uint16_t getInternalMemAddress(uint32_t memoryOffset) const;
        uint16_t getBlockRemaining(uint32_t memoryOffset) const;
        uint16_t getPageRemaining(uint32_t memoryOffset) const;
};

#endif
#endif
