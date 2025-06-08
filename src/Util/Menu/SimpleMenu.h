/*
 * JQBSTM32 Framework - SimpleMenu.h Header
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
#ifndef __SIMPLEMENU_H_
#define __SIMPLEMENU_H_

#include "../../Core.h"

typedef const char *String;  ///< String type alias for menu text

/**
 * @class SimpleMenu
 * @brief Simple hierarchical menu system for embedded applications
 * @details This class provides a flexible menu system with support for submenus,
 *          value editing, function callbacks, and dynamic list generation.
 *          Designed for use with simple display interfaces like LCD or LED displays.
 *          Supports navigation with up/down/select/back operations.
 * @note Requires external display and input handling through callback functions
 */
class SimpleMenu
{
    public:
        /**
         * @brief Constructor for root menu with fixed submenu array
         * @param _numberMenuItems Number of items in the submenu array
         * @param _submenus Pointer to array of submenu items
         * @details Creates root menu with specified number of static submenu items
         */
        SimpleMenu(int _numberMenuItems, SimpleMenu *_submenus);
        
        /**
         * @brief Constructor for function-only menu item
         * @param _CallBack Function to execute when menu item is selected
         * @details Creates menu item that executes a function when selected
         */
        SimpleMenu(void (*_CallBack)());
        
        /**
         * @brief Constructor for dynamic list menu with callback
         * @param _ListCallBack Function to populate list items dynamically
         * @param _CallBack Function to execute when list item is selected
         * @details Creates menu with dynamically generated list items
         */
        SimpleMenu(bool (*_ListCallBack)(int index), void (*_CallBack)());
        
        /**
         * @brief Constructor for dynamic list menu with submenus
         * @param _ListCallBack Function to populate list items dynamically
         * @param _submenus Submenus to navigate to from list items
         * @details Creates menu with dynamically generated list items leading to submenus
         */
        SimpleMenu(bool (*_ListCallBack)(int index), SimpleMenu *_submenus);
        
        /**
         * @brief Constructor for named submenu
         * @param _name Display name for the menu item
         * @param _numberMenuItems Number of items in submenu array
         * @param _submenus Pointer to array of submenu items
         * @details Creates named submenu with specified static items
         */
        SimpleMenu(String _name, int _numberMenuItems, SimpleMenu *_submenus);
        
        /**
         * @brief Constructor for value editing menu item
         * @param _name Display name for the menu item
         * @param _value Pointer to integer value to edit
         * @details Creates menu item for editing integer values without limits
         */
        SimpleMenu(String _name, int *_value);
        
        /**
         * @brief Constructor for value editing menu item with limits
         * @param _name Display name for the menu item
         * @param _value Pointer to integer value to edit
         * @param _min Minimum allowed value
         * @param _max Maximum allowed value
         * @details Creates menu item for editing integer values with specified range limits
         */
        SimpleMenu(String _name, int *_value, int _min, int _max);
        
        /**
         * @brief Constructor for named function menu item
         * @param _name Display name for the menu item
         * @param _CallBack Function to execute when item is selected
         * @details Creates named menu item that executes function when selected
         */
        SimpleMenu(String _name, void (*_CallBack)());
        
        /**
         * @brief Constructor for named dynamic list menu with submenus
         * @param _name Display name for the menu item
         * @param _ListCallBack Function to populate list items dynamically
         * @param _submenus Submenus to navigate to from list items
         * @details Creates named menu with dynamically generated list items and submenus
         */
        SimpleMenu(String _name, bool (*_ListCallBack)(int index), SimpleMenu *_submenus);
        
        /**
         * @brief Constructor for named dynamic list menu with callback
         * @param _name Display name for the menu item
         * @param _ListCallBack Function to populate list items dynamically
         * @param _CallBack Function to execute when list item is selected
         * @details Creates named menu with dynamically generated list items and callback
         */
        SimpleMenu(String _name, bool (*_ListCallBack)(int index), void (*_CallBack)());

        /**
         * @brief Initialize menu system with default display callback
         * @details Basic initialization for menu system operation
         */
        void begin();
        
        /**
         * @brief Initialize menu system with custom display callback
         * @param _displayCallBack Function to handle menu display updates
         * @details Initialize menu with custom display function for rendering
         */
        void begin(void (*_displayCallBack)(SimpleMenu *_menu));
        
        /**
         * @brief Initialize menu system with display and value callbacks
         * @param _displayCallBack Function to handle menu display updates
         * @param _valueCallBack Function to handle value change notifications
         * @details Full initialization with both display and value change callbacks
         */
        void begin(void (*_displayCallBack)(SimpleMenu *_menu),void (*_valueCallBack)(SimpleMenu *_menu));
        
        /**
         * @brief Initialize menu system with top menu and callbacks
         * @param _Top Pointer to top-level menu for navigation
         * @param _displayCallBack Function to handle menu display updates
         * @param _valueCallBack Function to handle value change notifications
         * @details Full initialization with specific top menu and callbacks
         */
        void begin(SimpleMenu *_Top,void (*_displayCallBack)(SimpleMenu *_menu),void (*_valueCallBack)(SimpleMenu *_menu));
        
        /**
         * @brief Set callback functions for menu operations
         * @param _displayCallBack Function to handle menu display updates
         * @param _valueCallBack Function to handle value change notifications
         * @details Configure callback functions after menu creation
         */
        void setFunctions(void (*_displayCallBack)(SimpleMenu *_menu),void (*_valueCallBack)(SimpleMenu *_menu));

        /**
         * @brief Navigate to top-level menu
         * @details Returns navigation to the root menu level
         */
        void home();
        
        /**
         * @brief Select current menu item
         * @details Activates current menu item (enter submenu, execute function, or edit value)
         */
        void select();
        
        /**
         * @brief Navigate back to parent menu
         * @details Returns to previous menu level in hierarchy
         */
        void back();
        
        /**
         * @brief Return from value editing mode
         * @details Exits value editing and returns to menu navigation
         */
        void returned();
        
        /**
         * @brief Navigate up in menu list
         * @details Moves selection to previous item in current menu level
         */
        void up();
        
        /**
         * @brief Navigate down in menu list
         * @details Moves selection to next item in current menu level
         */
        void down();
        
        /**
         * @brief Jump to specific menu index
         * @param _index Target menu item index
         * @details Directly sets menu selection to specified index
         */
        void index(int _index);

        /**
         * @brief Get next menu item
         * @return Pointer to next menu item in sequence
         * @details Returns pointer to next menu item for iteration
         */
        SimpleMenu *next();
        
        /**
         * @brief Get menu item at specific index
         * @param index Target menu item index
         * @return Pointer to menu item at specified index
         * @details Returns pointer to menu item at given index
         */
        SimpleMenu *next(int index);
        
        /**
         * @brief Get current value from value menu item
         * @return Current integer value
         * @details Returns current value for value editing menu items
         */
        int getValue();
        
        /**
         * @brief Check if menu item has editable value
         * @return true if menu item has value, false otherwise
         * @details Tests whether menu item is a value editing type
         */
        bool hasValue();
        
        /**
         * @brief Get current menu selection index
         * @return Current menu index position
         * @details Returns current selection index within menu level
         */
        int getIndex();
        
        /**
         * @brief Print menu information for debugging
         * @details Outputs menu structure and state information for debugging
         */
        void print();
            
        String name;             ///< Display name of the menu item
        bool selectMenu = false; ///< Flag indicating if menu item is selected
        int menuLocation = 0;    ///< Current position in menu hierarchy

    private:
        SimpleMenu *Top_menu = NULL;  ///< Pointer to top-level menu

        int numberMenuItems;                                        ///< Number of submenu items
        SimpleMenu *submenus = NULL;                               ///< Array of static submenu items
        SimpleMenu *subListmenus = NULL;                           ///< Dynamic submenu items
        int *value = NULL;                                         ///< Pointer to editable value
        void (*CallBack) () = NULL;                               ///< Function callback for menu action
        bool (*ListCallBack)(int index) = NULL;                   ///< Dynamic list generation callback
        void (*displayCallBack)(SimpleMenu *_menu) = NULL;        ///< Display update callback
        void (*valueCallBack)(SimpleMenu *_menu) = NULL;          ///< Value change callback

        int min;                ///< Minimum value for value editing
        int max;                ///< Maximum value for value editing
        bool minMaxSet = false; ///< Flag indicating if min/max limits are set
};

#endif
