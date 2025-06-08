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

#ifndef FL_BIT_H_
#define FL_BIT_H_

#define ABS(X) ((X) > 0 ? (X) : -(X))
#define XOR(A,B) (!A != !B)

#define FALSE     		  	0
#define TRUE      	  		1

#define bf_set(flag, mask)    (flag |= mask)
#define bf_clr(flag, mask)    (flag &= ~mask)
#define bf_is(flag, mask)     ((flag & mask) != 0)
#define bf_mask(flag, mask)   (flag & mask)

/* Makra do latwej manipulacji bitami liczb (32, 16, 8) bitowych */
#define m32_bitset_elem(ptr,bit) ((uint32_t *)(ptr))[(bit)/32]
#define m32_bitset_mask(ptr,bit) (1 << (bit) % 32)

#define bfd_iss(ptr,bit) ((m32_bitset_elem(ptr,bit) & m32_bitset_mask(ptr,bit)) != 0)
#define bfd_set(ptr,bit) (m32_bitset_elem(ptr,bit) |= m32_bitset_mask(ptr,bit))
#define bfd_clr(ptr,bit) (m32_bitset_elem(ptr,bit) &= ~m32_bitset_mask(ptr,bit))
#define bfd_tog(ptr,bit) (m32_bitset_elem(ptr,bit) ^= m32_bitset_mask(ptr,bit))

#define m16_bitset_elem(ptr,bit) ((uint16_t *)(ptr))[(bit)/16]
#define m16_bitset_mask(ptr,bit) (1 << (bit) % 16)

#define fw_iss(ptr,bit) ((m16_bitset_elem(ptr,bit) & m16_bitset_mask(ptr,bit)) != 0)
#define fw_set(ptr,bit) (m16_bitset_elem(ptr,bit) |= m16_bitset_mask(ptr,bit))
#define fw_clr(ptr,bit) (m16_bitset_elem(ptr,bit) &= ~m16_bitset_mask(ptr,bit))
#define fw_tog(ptr,bit) (m16_bitset_elem(ptr,bit) ^= m16_bitset_mask(ptr,bit))

#define bfw_iss(ptr,bit) ((m16_bitset_elem(ptr,bit) & m16_bitset_mask(ptr,bit)) != 0)
#define bfw_set(ptr,bit) (m16_bitset_elem(ptr,bit) |= m16_bitset_mask(ptr,bit))
#define bfw_clr(ptr,bit) (m16_bitset_elem(ptr,bit) &= ~m16_bitset_mask(ptr,bit))
#define bfw_tog(ptr,bit) (m16_bitset_elem(ptr,bit) ^= m16_bitset_mask(ptr,bit))

#define m8_bitset_elem(ptr,bit) ((uint8_t *)(ptr))[(bit)/8]
#define m8_bitset_mask(ptr,bit) (1 << (bit) % 8)

#define bfb_iss(ptr,bit) ((m8_bitset_elem(ptr,bit) & m8_bitset_mask(ptr,bit)) != 0)
#define bfb_set(ptr,bit) (m8_bitset_elem(ptr,bit) |= m8_bitset_mask(ptr,bit))
#define bfb_clr(ptr,bit) (m8_bitset_elem(ptr,bit) &= ~m8_bitset_mask(ptr,bit))
#define bfb_tog(ptr,bit) (m8_bitset_elem(ptr,bit) ^= m8_bitset_mask(ptr,bit))

#endif /* FL_BIT_H_ */

