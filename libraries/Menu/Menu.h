#ifndef Menu_h
#define Menu_h

#include "Arduino.h"

#define MENU_NAME_LEN 13

struct menu_item  // 20 bytes total
{
    char name[MENU_NAME_LEN];  // 13 bytes
    char type;                 // 1 byte
    void *function;            // 2 bytes
};


class Menu
{
  public:
    Menu(menu_item *mainMenu);
    menu_item* currentMenu(); 
    menu_item* currentMenuItem();
    void select();
    private:
    menu_item* current_menu;
    int current_menu_item_index;

};

#endif