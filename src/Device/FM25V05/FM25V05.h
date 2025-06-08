/*
 * JQBSTM32 Framework - FM25V05.h Header
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
#include "../../Hardware/SPI/SPI.h"
#ifdef __SPI_H_

#ifndef __FM25V05_H_
#define __FM25V05_H_

#include "../../Interface/IExternalMemory.h"

/**
 * @class FM25V05
 * @brief Driver class for FM25V05 FRAM (Ferroelectric RAM) memory device
 * @details This class provides interface for reading and writing data to FM25V05 FRAM
 *          via SPI communication. The FM25V05 is a 512Kbit (64KB) serial FRAM
 *          organized as 65,536 x 8 bits. FRAM offers unlimited write endurance
 *          and instant write operations without page programming delays.
 * @note Implements IExternalMemory interface for standardized memory operations
 */
class FM25V05 : public IExternalMemory {
    public:
        /**
         * @brief Constructor for FM25V05 FRAM driver
         * @param pInstance Pointer to SPI instance for communication
         * @param GPIOx GPIO port for chip select pin
         * @param GPIO_Pin GPIO pin number for chip select
         * @details Initializes FRAM driver with specified SPI interface and chip select control.
         *          Chip select pin is used to enable/disable SPI communication with the device.
         * @note FM25V05 operates at up to 20MHz SPI clock frequency
         */
        FM25V05(SPI *pInstance,
            GPIO_TypeDef* GPIOx,
            uint16_t GPIO_Pin
        );

        /**
         * @brief Read data from FRAM memory
         * @param MemAddress Memory address to read from (0-65535 for FM25V05)
         * @param pData Pointer to buffer where read data will be stored
         * @param Size Number of bytes to read
         * @details Reads sequential bytes from FRAM starting at specified address.
         *          FRAM allows unlimited read operations with no wear concerns.
         *          Reading is performed instantly without delays.
         * @note Reading beyond device capacity will wrap around to address 0
         */
        void readFromMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) override;

        /**
         * @brief Write data to FRAM memory
         * @param MemAddress Memory address to write to (0-65535 for FM25V05)
         * @param pData Pointer to data buffer to write
         * @param Size Number of bytes to write
         * @details Writes sequential bytes to FRAM starting at specified address.
         *          FRAM writing is instantaneous with no programming delays or page restrictions.
         *          Supports unlimited write cycles without wear-out concerns.
         * @note Writing beyond device capacity will wrap around to address 0
         * @note FRAM retains data for 10+ years without power and offers 10^14 write cycles
         */
        void writeToMemory(
            uint32_t MemAddress,
            uint8_t *pData, 
            uint16_t Size
        ) override;

        protected:
        SPI             *_pInstance;  ///< Pointer to SPI communication interface

        GPIO_TypeDef    *_CSPort;    ///< GPIO port for chip select control
        uint16_t        _CSPin;      ///< GPIO pin for chip select control
    
};
#endif
#endif
