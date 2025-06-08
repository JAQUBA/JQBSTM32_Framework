/*
 * JQBSTM32 Framework - Random.h Header
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
#ifndef __MATH_H
#define __MATH_H

#include <stdlib.h>

/**
 * @file Random.h
 * @brief Random number generation utilities
 * @details This header provides functions for generating random numbers within
 *          specified ranges for both integer and floating-point values.
 *          Uses standard C library rand() function as the base generator.
 */

/**
 * @brief Generate random floating-point number within specified range
 * @param min Minimum value (inclusive)
 * @param max Maximum value (exclusive)
 * @return Random float value in range [min, max)
 * @details Generates a random floating-point number uniformly distributed
 *          between min (inclusive) and max (exclusive).
 * @note Uses standard rand() function - consider seeding with srand() for better randomness
 * @note Result is in range [min, max) where max is not included
 */
float random_float(float min, float max);

/**
 * @brief Generate random integer within specified range
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 * @return Random integer value in range [min, max]
 * @details Generates a random integer uniformly distributed between
 *          min and max (both inclusive).
 * @note Uses standard rand() function - consider seeding with srand() for better randomness
 * @note Both min and max values are included in the possible results
 */
int random_num(int min, int max);

#endif

