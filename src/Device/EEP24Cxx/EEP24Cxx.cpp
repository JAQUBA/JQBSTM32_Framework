/*
 * JQBSTM32 Framework - EEP24Cxx.cpp Implementation
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
#include "EEP24Cxx.h"
#ifdef __EEP24CXX_H_

const EEP24Cxx::DeviceConfig EEP24Cxx::CONFIG_24AA02  = {  256U,   8U, ADDRESS_SIZE_8BIT, 0U };
const EEP24Cxx::DeviceConfig EEP24Cxx::CONFIG_24LC02B = {  256U,   8U, ADDRESS_SIZE_8BIT, 0U };
const EEP24Cxx::DeviceConfig EEP24Cxx::CONFIG_24LC04B = {  512U,  16U, ADDRESS_SIZE_8BIT, 1U };
const EEP24Cxx::DeviceConfig EEP24Cxx::CONFIG_24LC08B = { 1024U,  16U, ADDRESS_SIZE_8BIT, 2U };
const EEP24Cxx::DeviceConfig EEP24Cxx::CONFIG_24LC16B = { 2048U,  16U, ADDRESS_SIZE_8BIT, 3U };
const EEP24Cxx::DeviceConfig EEP24Cxx::CONFIG_24LC32A = { 4096U,  32U, ADDRESS_SIZE_16BIT, 0U };
const EEP24Cxx::DeviceConfig EEP24Cxx::CONFIG_24LC64  = { 8192U,  32U, ADDRESS_SIZE_16BIT, 0U };
const EEP24Cxx::DeviceConfig EEP24Cxx::CONFIG_24LC128 = {16384U,  64U, ADDRESS_SIZE_16BIT, 0U };
const EEP24Cxx::DeviceConfig EEP24Cxx::CONFIG_24LC256 = {32768U,  64U, ADDRESS_SIZE_16BIT, 0U };
const EEP24Cxx::DeviceConfig EEP24Cxx::CONFIG_24LC512 = {65536U, 128U, ADDRESS_SIZE_16BIT, 0U };

EEP24Cxx::EEP24Cxx(
    I2C *pInstance,
    const DeviceConfig &config,
    uint16_t DevAddress,
    uint32_t BaseAddress
) {
    _pInstance = pInstance;
    _DevAddress = DevAddress;
    _BaseAddress = BaseAddress;

    _sizeBytes = config.sizeBytes;
    _pageSize = config.pageSize;
    _memAddSize = config.memAddSize;
    _blockAddressBits = config.blockAddressBits;
}

uint32_t EEP24Cxx::normalizeAddress(uint32_t address) const {
    if(_sizeBytes == 0U) return 0U;
    return address % _sizeBytes;
}

uint16_t EEP24Cxx::getDeviceAddressForOffset(uint32_t memoryOffset) const {
    if(_memAddSize != ADDRESS_SIZE_8BIT || _blockAddressBits == 0U) {
        return _DevAddress;
    }

    const uint16_t blockMask = (uint16_t)((1U << _blockAddressBits) - 1U);
    const uint16_t block = (uint16_t)((memoryOffset >> 8U) & blockMask);
    return (uint16_t)(_DevAddress | (uint16_t)(block << 1U));
}

uint16_t EEP24Cxx::getInternalMemAddress(uint32_t memoryOffset) const {
    if(_memAddSize == ADDRESS_SIZE_8BIT) {
        return (uint16_t)(memoryOffset & 0xFFU);
    }
    return (uint16_t)(memoryOffset & 0xFFFFU);
}

uint16_t EEP24Cxx::getBlockRemaining(uint32_t memoryOffset) const {
    if(_memAddSize != ADDRESS_SIZE_8BIT || _blockAddressBits == 0U) {
        return 0xFFFFU;
    }

    const uint16_t inBlock = (uint16_t)(memoryOffset & 0xFFU);
    return (uint16_t)(256U - inBlock);
}

uint16_t EEP24Cxx::getPageRemaining(uint32_t memoryOffset) const {
    if(_pageSize == 0U) {
        return 0xFFFFU;
    }

    const uint16_t inPage = (uint16_t)(memoryOffset % _pageSize);
    return (uint16_t)(_pageSize - inPage);
}

void EEP24Cxx::readFromMemory(
    uint32_t MemAddress,
    uint8_t *pData,
    uint16_t Size
) {
    if(_pInstance == nullptr || pData == nullptr || Size == 0U || _sizeBytes == 0U) {
        return;
    }

    uint16_t transferred = 0U;
    while(transferred < Size) {
        const uint32_t currentOffset = normalizeAddress(_BaseAddress + MemAddress + transferred);
        const uint16_t devAddress = getDeviceAddressForOffset(currentOffset);
        const uint16_t internalAddress = getInternalMemAddress(currentOffset);

        const uint16_t blockRemaining = getBlockRemaining(currentOffset);
        uint16_t chunk = (uint16_t)(Size - transferred);
        if(chunk > blockRemaining) {
            chunk = blockRemaining;
        }

        _pInstance->readFromMemory(devAddress, internalAddress, _memAddSize, &pData[transferred], chunk);
        transferred = (uint16_t)(transferred + chunk);
    }
}

void EEP24Cxx::writeToMemory(
    uint32_t MemAddress,
    uint8_t *pData,
    uint16_t Size
) {
    if(_pInstance == nullptr || pData == nullptr || Size == 0U || _sizeBytes == 0U) {
        return;
    }

    uint16_t transferred = 0U;
    while(transferred < Size) {
        const uint32_t currentOffset = normalizeAddress(_BaseAddress + MemAddress + transferred);
        const uint16_t devAddress = getDeviceAddressForOffset(currentOffset);
        const uint16_t internalAddress = getInternalMemAddress(currentOffset);

        const uint16_t pageRemaining = getPageRemaining(currentOffset);
        const uint16_t blockRemaining = getBlockRemaining(currentOffset);

        uint16_t chunk = (uint16_t)(Size - transferred);
        if(chunk > pageRemaining) {
            chunk = pageRemaining;
        }
        if(chunk > blockRemaining) {
            chunk = blockRemaining;
        }

        _pInstance->writeToMemory(devAddress, internalAddress, _memAddSize, &pData[transferred], chunk);
        transferred = (uint16_t)(transferred + chunk);
    }
}

#endif
