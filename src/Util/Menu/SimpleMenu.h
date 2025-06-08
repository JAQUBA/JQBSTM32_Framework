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

typedef const char *String;

class SimpleMenu
{
    public:
        SimpleMenu(int _numberMenuItems, SimpleMenu *_submenus); //defult
        SimpleMenu(void (*_CallBack)());
        SimpleMenu(bool (*_ListCallBack)(int index), void (*_CallBack)());
        SimpleMenu(bool (*_ListCallBack)(int index), SimpleMenu *_submenus);
        SimpleMenu(String _name, int _numberMenuItems, SimpleMenu *_submenus); //sub menu
        SimpleMenu(String _name, int *_value); //Value menu
        SimpleMenu(String _name, int *_value, int _min, int _max); //Value menu with min and max
        SimpleMenu(String _name, void (*_CallBack)()); //function menu
        SimpleMenu(String _name, bool (*_ListCallBack)(int index), SimpleMenu *_submenus); //List menu with function
        SimpleMenu(String _name, bool (*_ListCallBack)(int index), void (*_CallBack)()); //List menu with function

        void begin();
        void begin(void (*_displayCallBack)(SimpleMenu *_menu));
        void begin(void (*_displayCallBack)(SimpleMenu *_menu),void (*_valueCallBack)(SimpleMenu *_menu));
        void begin(SimpleMenu *_Top,void (*_displayCallBack)(SimpleMenu *_menu),void (*_valueCallBack)(SimpleMenu *_menu));
        void setFunctions(void (*_displayCallBack)(SimpleMenu *_menu),void (*_valueCallBack)(SimpleMenu *_menu));

        void home();
        void select();
        void back();
        void returned();
        void up();
        void down();
        void index(int _index);

        SimpleMenu *next();
        SimpleMenu *next(int index);
        int getValue();
        bool hasValue();
        int getIndex();
        void print();
            
        String name;
        bool selectMenu = false;
        int menuLocation = 0;

    private:
        SimpleMenu *Top_menu = NULL;

        int numberMenuItems;
        SimpleMenu *submenus = NULL;
        SimpleMenu *subListmenus = NULL;
        int *value = NULL;
        void (*CallBack) () = NULL;
        bool (*ListCallBack)(int index) = NULL;
        void (*displayCallBack)(SimpleMenu *_menu) = NULL;
        void (*valueCallBack)(SimpleMenu *_menu) = NULL;

        int min;
        int max;
        bool minMaxSet = false;
};

#endif 
