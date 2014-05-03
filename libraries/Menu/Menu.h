#ifndef Menu_h
#define Menu_h

#include "Arduino.h"
#include <StackArray.h>

#define MENU_NAME_LEN 13

struct menu_item  // 20 bytes total
{
    char name[MENU_NAME_LEN];  // 13 bytes
    char type;                 // 1 byte
    void *function;            // 2 bytes
};

struct sub_menu 
{
    menu_item *menuItems;
    int currentIndex;
    int maxIndex;
};

class Menu
{
  public:
    Menu(sub_menu *mainMenu);
    sub_menu* currentMenu(); 
    menu_item* currentMenuItem();
    void select();
    void up();
    void down();
    void back();
    private:
    sub_menu* current_sub_menu;
    StackArray <sub_menu*> previous_menus;
};

#endif