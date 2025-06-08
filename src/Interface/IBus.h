/*
 * JQBSTM32 Framework - IBus.h Header
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
#ifndef __IBUS_H
#define __IBUS_H

#include <functional>

#define MEM_ADR_INVALID 0 ///< Invalid memory address constant

/**
 * @brief Bus communication interface
 * @details Abstract base class for communication bus implementations (I2C, SPI, UART, etc.)
 * @note Provides common interface for all bus communication classes
 */
class IBus {
    public:
    protected:
};

#endif
