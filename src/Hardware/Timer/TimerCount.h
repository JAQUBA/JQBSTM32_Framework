/*
 * JQBSTM32 Framework - TimerCount.h Header
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
 * @file TimerCount.h
 * @brief Timer availability detection and counting utility
 * @details This header provides compile-time detection of available timer peripherals
 *          on the target STM32 microcontroller. It defines availability flags for each
 *          possible timer (TIM1-TIM22) and calculates the total count of available timers.
 * @note These macros can be used for conditional compilation and resource management
 */

// Checking availability of individual timers
#ifdef TIM1
    #define TIMER1_AVAILABLE 1   ///< Timer 1 is available on this MCU
#else
    #define TIMER1_AVAILABLE 0   ///< Timer 1 is not available on this MCU
#endif

#ifdef TIM2
    #define TIMER2_AVAILABLE 1   ///< Timer 2 is available on this MCU
#else
    #define TIMER2_AVAILABLE 0   ///< Timer 2 is not available on this MCU
#endif

#ifdef TIM3
    #define TIMER3_AVAILABLE 1   ///< Timer 3 is available on this MCU
#else
    #define TIMER3_AVAILABLE 0   ///< Timer 3 is not available on this MCU
#endif

#ifdef TIM4
    #define TIMER4_AVAILABLE 1   ///< Timer 4 is available on this MCU
#else
    #define TIMER4_AVAILABLE 0   ///< Timer 4 is not available on this MCU
#endif

#ifdef TIM5
    #define TIMER5_AVAILABLE 1   ///< Timer 5 is available on this MCU
#else
    #define TIMER5_AVAILABLE 0   ///< Timer 5 is not available on this MCU
#endif

#ifdef TIM6
    #define TIMER6_AVAILABLE 1   ///< Timer 6 is available on this MCU
#else
    #define TIMER6_AVAILABLE 0   ///< Timer 6 is not available on this MCU
#endif

#ifdef TIM7
    #define TIMER7_AVAILABLE 1   ///< Timer 7 is available on this MCU
#else
    #define TIMER7_AVAILABLE 0   ///< Timer 7 is not available on this MCU
#endif

#ifdef TIM8
    #define TIMER8_AVAILABLE 1   ///< Timer 8 is available on this MCU
#else
    #define TIMER8_AVAILABLE 0   ///< Timer 8 is not available on this MCU
#endif

#ifdef TIM9
    #define TIMER9_AVAILABLE 1   ///< Timer 9 is available on this MCU
#else
    #define TIMER9_AVAILABLE 0   ///< Timer 9 is not available on this MCU
#endif

#ifdef TIM10
    #define TIMER10_AVAILABLE 1  ///< Timer 10 is available on this MCU
#else
    #define TIMER10_AVAILABLE 0  ///< Timer 10 is not available on this MCU
#endif

#ifdef TIM11
    #define TIMER11_AVAILABLE 1  ///< Timer 11 is available on this MCU
#else
    #define TIMER11_AVAILABLE 0  ///< Timer 11 is not available on this MCU
#endif

#ifdef TIM12
    #define TIMER12_AVAILABLE 1  ///< Timer 12 is available on this MCU
#else
    #define TIMER12_AVAILABLE 0  ///< Timer 12 is not available on this MCU
#endif

#ifdef TIM13
    #define TIMER13_AVAILABLE 1  ///< Timer 13 is available on this MCU
#else
    #define TIMER13_AVAILABLE 0  ///< Timer 13 is not available on this MCU
#endif

#ifdef TIM14
    #define TIMER14_AVAILABLE 1  ///< Timer 14 is available on this MCU
#else
    #define TIMER14_AVAILABLE 0  ///< Timer 14 is not available on this MCU
#endif

#ifdef TIM15
    #define TIMER15_AVAILABLE 1  ///< Timer 15 is available on this MCU
#else
    #define TIMER15_AVAILABLE 0  ///< Timer 15 is not available on this MCU
#endif

#ifdef TIM16
    #define TIMER16_AVAILABLE 1  ///< Timer 16 is available on this MCU
#else
    #define TIMER16_AVAILABLE 0  ///< Timer 16 is not available on this MCU
#endif

#ifdef TIM17
    #define TIMER17_AVAILABLE 1  ///< Timer 17 is available on this MCU
#else
    #define TIMER17_AVAILABLE 0  ///< Timer 17 is not available on this MCU
#endif

#ifdef TIM18
    #define TIMER18_AVAILABLE 1  ///< Timer 18 is available on this MCU
#else
    #define TIMER18_AVAILABLE 0  ///< Timer 18 is not available on this MCU
#endif

#ifdef TIM19
    #define TIMER19_AVAILABLE 1  ///< Timer 19 is available on this MCU
#else
    #define TIMER19_AVAILABLE 0  ///< Timer 19 is not available on this MCU
#endif

#ifdef TIM20
    #define TIMER20_AVAILABLE 1  ///< Timer 20 is available on this MCU
#else
    #define TIMER20_AVAILABLE 0  ///< Timer 20 is not available on this MCU
#endif

#ifdef TIM21
    #define TIMER21_AVAILABLE 1  ///< Timer 21 is available on this MCU
#else
    #define TIMER21_AVAILABLE 0  ///< Timer 21 is not available on this MCU
#endif

#ifdef TIM22
    #define TIMER22_AVAILABLE 1  ///< Timer 22 is available on this MCU
#else
    #define TIMER22_AVAILABLE 0  ///< Timer 22 is not available on this MCU
#endif

/**
 * @def TIMERS_AVAILABLE
 * @brief Total count of available timer peripherals on the target MCU
 * @details This macro calculates the sum of all available timers by adding up
 *          the individual timer availability flags. Can be used for resource
 *          allocation and validation during compile time.
 * @note Value will be between 0 and 22 depending on the STM32 variant
 */
#define TIMERS_AVAILABLE ( \
    TIMER1_AVAILABLE + TIMER2_AVAILABLE + TIMER3_AVAILABLE + TIMER4_AVAILABLE + \
    TIMER5_AVAILABLE + TIMER6_AVAILABLE + TIMER7_AVAILABLE + TIMER8_AVAILABLE + \
    TIMER9_AVAILABLE + TIMER10_AVAILABLE + TIMER11_AVAILABLE + TIMER12_AVAILABLE + \
    TIMER13_AVAILABLE + TIMER14_AVAILABLE + TIMER15_AVAILABLE + TIMER16_AVAILABLE + \
    TIMER17_AVAILABLE + TIMER18_AVAILABLE + TIMER19_AVAILABLE + TIMER20_AVAILABLE + \
    TIMER21_AVAILABLE + TIMER22_AVAILABLE )

