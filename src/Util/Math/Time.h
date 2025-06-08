/*
 * JQBSTM32 Framework - Time.h Header
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

/**
 * @file Time.h
 * @brief Time conversion utilities and constants
 * @details This header provides macros and constants for time calculations
 *          and conversions. Useful for delay functions, scheduling, and
 *          time-based operations in millisecond units.
 */

#include <stdio.h>

#define SECONDS 1000UL  ///< Milliseconds in one second (1000ms)
#define MINUTES 60000UL ///< Milliseconds in one minute (60000ms)

/**
 * @def TIME(x, y)
 * @brief Calculate time value by multiplying time units
 * @param x Number of time units
 * @param y Time unit multiplier (e.g., SECONDS, MINUTES)
 * @return Total time in milliseconds
 * @details Convenience macro for calculating time values in milliseconds.
 *          Commonly used with SECONDS and MINUTES constants.
 * @example TIME(5, SECONDS) returns 5000 (5 seconds in milliseconds)
 * @example TIME(2, MINUTES) returns 120000 (2 minutes in milliseconds)
 */
#define TIME(x, y) x * y

