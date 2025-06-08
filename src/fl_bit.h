/*
 * JQBSTM32 Framework - fl_bit.h Header
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
 * @file fl_bit.h
 * @brief Bit manipulation utilities and macros for JQBSTM32 Framework
 * @details This header provides comprehensive bit manipulation utilities including
 *          basic boolean operations, bitfield manipulation macros for 8, 16, and 32-bit
 *          data types, and helper macros for common operations.
 * @author JAQUBA
 * @version 1.0
 * @date 2024
 */

#ifndef FL_BIT_H_
#define FL_BIT_H_

/**
 * @defgroup MathUtilities Mathematical Utilities
 * @brief Basic mathematical operations and comparisons
 * @{
 */

/**
 * @brief Calculate absolute value of a number
 * @param X Number to calculate absolute value for
 * @return Absolute value of X
 */
#define ABS(X) ((X) > 0 ? (X) : -(X))

/**
 * @brief Exclusive OR operation on boolean values
 * @param A First boolean value
 * @param B Second boolean value
 * @return True if exactly one of A or B is true, false otherwise
 */
#define XOR(A,B) (!A != !B)

/**
 * @brief Boolean false constant
 */
#define FALSE     		  	0

/**
 * @brief Boolean true constant
 */
#define TRUE      	  		1

/** @} */

/**
 * @defgroup BasicBitOperations Basic Bit Operations
 * @brief Simple bit manipulation macros for flags
 * @{
 */

/**
 * @brief Set bits in a flag using a mask
 * @param flag Variable to modify
 * @param mask Bitmask specifying which bits to set
 */
#define bf_set(flag, mask)    (flag |= mask)

/**
 * @brief Clear bits in a flag using a mask
 * @param flag Variable to modify
 * @param mask Bitmask specifying which bits to clear
 */
#define bf_clr(flag, mask)    (flag &= ~mask)

/**
 * @brief Test if any bits specified by mask are set
 * @param flag Variable to test
 * @param mask Bitmask specifying which bits to test
 * @return Non-zero if any specified bits are set, zero otherwise
 */
#define bf_is(flag, mask)     ((flag & mask) != 0)

/**
 * @brief Apply mask to flag (bitwise AND)
 * @param flag Variable to mask
 * @param mask Bitmask to apply
 * @return Result of flag AND mask
 */
#define bf_mask(flag, mask)   (flag & mask)

/** @} */

/**
 * @defgroup Bit32Operations 32-bit Bitset Operations
 * @brief Bit manipulation macros for 32-bit arrays
 * @details These macros treat arrays as bitsets where each bit can be individually
 *          manipulated using bit position numbers.
 * @{
 */

/**
 * @brief Get the 32-bit element containing the specified bit
 * @param ptr Pointer to 32-bit array
 * @param bit Bit position (0-based)
 * @return Reference to the 32-bit element containing the bit
 */
#define m32_bitset_elem(ptr,bit) ((uint32_t *)(ptr))[(bit)/32]

/**
 * @brief Create bitmask for specified bit position within 32-bit element
 * @param ptr Pointer to array (used for type consistency)
 * @param bit Bit position (0-based)
 * @return Bitmask with only the relevant bit set
 */
#define m32_bitset_mask(ptr,bit) (1 << (bit) % 32)

/**
 * @brief Test if a specific bit is set in 32-bit bitset
 * @param ptr Pointer to 32-bit array
 * @param bit Bit position to test (0-based)
 * @return True if bit is set, false otherwise
 */
#define bfd_iss(ptr,bit) ((m32_bitset_elem(ptr,bit) & m32_bitset_mask(ptr,bit)) != 0)

/**
 * @brief Set a specific bit in 32-bit bitset
 * @param ptr Pointer to 32-bit array
 * @param bit Bit position to set (0-based)
 */
#define bfd_set(ptr,bit) (m32_bitset_elem(ptr,bit) |= m32_bitset_mask(ptr,bit))

/**
 * @brief Clear a specific bit in 32-bit bitset
 * @param ptr Pointer to 32-bit array
 * @param bit Bit position to clear (0-based)
 */
#define bfd_clr(ptr,bit) (m32_bitset_elem(ptr,bit) &= ~m32_bitset_mask(ptr,bit))

/**
 * @brief Toggle a specific bit in 32-bit bitset
 * @param ptr Pointer to 32-bit array
 * @param bit Bit position to toggle (0-based)
 */
#define bfd_tog(ptr,bit) (m32_bitset_elem(ptr,bit) ^= m32_bitset_mask(ptr,bit))

/** @} */

/**
 * @defgroup Bit16Operations 16-bit Bitset Operations
 * @brief Bit manipulation macros for 16-bit arrays
 * @details These macros treat arrays as bitsets where each bit can be individually
 *          manipulated using bit position numbers.
 * @{
 */

/**
 * @brief Get the 16-bit element containing the specified bit
 * @param ptr Pointer to 16-bit array
 * @param bit Bit position (0-based)
 * @return Reference to the 16-bit element containing the bit
 */
#define m16_bitset_elem(ptr,bit) ((uint16_t *)(ptr))[(bit)/16]

/**
 * @brief Create bitmask for specified bit position within 16-bit element
 * @param ptr Pointer to array (used for type consistency)
 * @param bit Bit position (0-based)
 * @return Bitmask with only the relevant bit set
 */
#define m16_bitset_mask(ptr,bit) (1 << (bit) % 16)

/**
 * @brief Test if a specific bit is set in 16-bit bitset
 * @param ptr Pointer to 16-bit array
 * @param bit Bit position to test (0-based)
 * @return True if bit is set, false otherwise
 */
#define fw_iss(ptr,bit) ((m16_bitset_elem(ptr,bit) & m16_bitset_mask(ptr,bit)) != 0)

/**
 * @brief Set a specific bit in 16-bit bitset
 * @param ptr Pointer to 16-bit array
 * @param bit Bit position to set (0-based)
 */
#define fw_set(ptr,bit) (m16_bitset_elem(ptr,bit) |= m16_bitset_mask(ptr,bit))

/**
 * @brief Clear a specific bit in 16-bit bitset
 * @param ptr Pointer to 16-bit array
 * @param bit Bit position to clear (0-based)
 */
#define fw_clr(ptr,bit) (m16_bitset_elem(ptr,bit) &= ~m16_bitset_mask(ptr,bit))

/**
 * @brief Toggle a specific bit in 16-bit bitset
 * @param ptr Pointer to 16-bit array
 * @param bit Bit position to toggle (0-based)
 */
#define fw_tog(ptr,bit) (m16_bitset_elem(ptr,bit) ^= m16_bitset_mask(ptr,bit))

/**
 * @brief Test if a specific bit is set in 16-bit bitset (alternative naming)
 * @param ptr Pointer to 16-bit array
 * @param bit Bit position to test (0-based)
 * @return True if bit is set, false otherwise
 */
#define bfw_iss(ptr,bit) ((m16_bitset_elem(ptr,bit) & m16_bitset_mask(ptr,bit)) != 0)

/**
 * @brief Set a specific bit in 16-bit bitset (alternative naming)
 * @param ptr Pointer to 16-bit array
 * @param bit Bit position to set (0-based)
 */ #define bfw_set(ptr,bit) (m16_bitset_elem(ptr,bit) |= m16_bitset_mask(ptr,bit))

/**
 * @brief Clear a specific bit in 16-bit bitset (alternative naming)
 * @param ptr Pointer to 16-bit array
 * @param bit Bit position to clear (0-based)
 */
#define bfw_clr(ptr,bit) (m16_bitset_elem(ptr,bit) &= ~m16_bitset_mask(ptr,bit))

/**
 * @brief Toggle a specific bit in 16-bit bitset (alternative naming)
 * @param ptr Pointer to 16-bit array
 * @param bit Bit position to toggle (0-based)
 */
#define bfw_tog(ptr,bit) (m16_bitset_elem(ptr,bit) ^= m16_bitset_mask(ptr,bit))

/** @} */

/**
 * @defgroup Bit8Operations 8-bit Bitset Operations
 * @brief Bit manipulation macros for 8-bit arrays
 * @details These macros treat arrays as bitsets where each bit can be individually
 *          manipulated using bit position numbers.
 * @{
 */

/**
 * @brief Get the 8-bit element containing the specified bit
 * @param ptr Pointer to 8-bit array
 * @param bit Bit position (0-based)
 * @return Reference to the 8-bit element containing the bit
 */
#define m8_bitset_elem(ptr,bit) ((uint8_t *)(ptr))[(bit)/8]

/**
 * @brief Create bitmask for specified bit position within 8-bit element
 * @param ptr Pointer to array (used for type consistency)
 * @param bit Bit position (0-based)
 * @return Bitmask with only the relevant bit set
 */
#define m8_bitset_mask(ptr,bit) (1 << (bit) % 8)

/**
 * @brief Test if a specific bit is set in 8-bit bitset
 * @param ptr Pointer to 8-bit array
 * @param bit Bit position to test (0-based)
 * @return True if bit is set, false otherwise
 */
#define bfb_iss(ptr,bit) ((m8_bitset_elem(ptr,bit) & m8_bitset_mask(ptr,bit)) != 0)

/**
 * @brief Set a specific bit in 8-bit bitset
 * @param ptr Pointer to 8-bit array
 * @param bit Bit position to set (0-based)
 */
#define bfb_set(ptr,bit) (m8_bitset_elem(ptr,bit) |= m8_bitset_mask(ptr,bit))

/**
 * @brief Clear a specific bit in 8-bit bitset
 * @param ptr Pointer to 8-bit array
 * @param bit Bit position to clear (0-based)
 */
#define bfb_clr(ptr,bit) (m8_bitset_elem(ptr,bit) &= ~m8_bitset_mask(ptr,bit))

/**
 * @brief Toggle a specific bit in 8-bit bitset
 * @param ptr Pointer to 8-bit array
 * @param bit Bit position to toggle (0-based)
 */
#define bfb_tog(ptr,bit) (m8_bitset_elem(ptr,bit) ^= m8_bitset_mask(ptr,bit))

/** @} */

/**
 * @example bit_manipulation_example.c
 * @brief Example usage of bit manipulation macros
 * 
 * @code
 * #include "fl_bit.h"
 * 
 * uint32_t flags = 0;
 * uint32_t bitset[4] = {0};
 * 
 * // Basic bit operations
 * bf_set(flags, 0x0F);        // Set lower 4 bits
 * bf_clr(flags, 0x03);        // Clear lower 2 bits
 * if (bf_is(flags, 0x04)) {   // Test if bit 2 is set
 *     // Bit 2 is set
 * }
 * 
 * // 32-bit bitset operations
 * bfd_set(bitset, 35);        // Set bit 35 (in second 32-bit element)
 * bfd_clr(bitset, 10);        // Clear bit 10
 * bfd_tog(bitset, 5);         // Toggle bit 5
 * 
 * if (bfd_iss(bitset, 35)) {  // Test if bit 35 is set
 *     // Bit 35 is set
 * }
 * @endcode
 */

#endif /* FL_BIT_H_ */

