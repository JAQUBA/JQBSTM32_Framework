/*
 * JQBSTM32 Framework - SED1520.h Header
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
#include "../../Hardware/I2C/I2C.h"
#ifdef __I2C_H_
#ifndef __SED1520_H_
#define __SED1520_H_

#include "Fonts/font6x8.h"

/**
 * @class SED1520
 * @brief Driver class for SED1520 graphic LCD controller
 * @details This class provides interface for controlling SED1520-based graphic LCD displays
 *          via I2C communication. The SED1520 is commonly used in 122x32 pixel monochrome
 *          LCD displays with dual controller configuration (61x32 per controller).
 *          Supports text rendering with built-in font and basic graphics operations.
 * @note Uses buffered output with flush() method for efficient display updates
 */
class SED1520 {
    public:
        /**
         * @brief Constructor for SED1520 display driver
         * @param i2c Pointer to I2C instance for communication
         * @details Initializes SED1520 driver with specified I2C interface.
         *          Sets up display parameters and prepares for text/graphics operations.
         * @note Display initialization sequence is performed during constructor
         */
        SED1520(I2C* i2c);
        
        /**
         * @brief Clear entire display
         * @details Clears all pixels on the display by setting them to off state.
         *          Resets cursor position to top-left corner (0,0).
         * @note Changes are buffered and require flush() call to take effect
         */
        void clear();
        
        /**
         * @brief Print text string at current cursor position
         * @param text Null-terminated string to display
         * @param fill Optional parameter to fill remaining line with spaces (default: false)
         * @details Renders text using built-in 6x8 pixel font. Automatically handles
         *          line wrapping and cursor advancement. Characters are drawn at current
         *          cursor position and cursor advances after each character.
         * @note Uses buffered output - call flush() to update display
         */
        void print(char const *text, bool fill = false);
        
        /**
         * @brief Set cursor position for text output
         * @param y Row position (0-3 for 4 text rows)
         * @param x Column position (0-20 for 21 character columns)
         * @details Sets the cursor position where next text output will appear.
         *          Position is specified in character coordinates, not pixel coordinates.
         * @note Character positions are based on 6x8 pixel font dimensions
         */
        void setCursor(uint8_t y, uint8_t x);
        
        /**
         * @brief Flush buffered commands to display
         * @details Sends all buffered display commands and data to the SED1520 controller.
         *          This method must be called after display operations to make changes visible.
         * @note Optimizes I2C communication by batching multiple operations
         */
        void flush();
        
    private:
        I2C *_i2c;  ///< Pointer to I2C communication interface
        
        /**
         * @brief Display sector enumeration for dual-controller displays
         */
        enum Sector {
            FIRST,   ///< First controller (left half of display)
            SECOND,  ///< Second controller (right half of display)
            ALL      ///< Both controllers
        };
        
        /**
         * @brief Control type enumeration for I2C communication
         */
        enum Control {
            COMMAND, ///< Command/instruction byte
            DATA     ///< Display data byte
        };
        
        /**
         * @brief Add data to transmission buffer
         * @param data Byte to transmit
         * @param control Type of data (command or display data)  
         * @param sector Target controller sector
         */
        void addBuffer(uint8_t data, Control control, Sector sector);
        
        uint8_t _current_row = 0;     ///< Current cursor row position
        uint8_t _current_column = 0;  ///< Current cursor column position

        /**
         * @brief Print single character at current cursor position
         * @param chr Character to print
         * @details Internal method for rendering individual characters using font data
         */
        void printChar(const char chr);
        
        std::list<uint8_t> dataQueue; ///< Buffer for batched I2C transmission
};

#endif
#endif
