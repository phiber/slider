#ifndef Menu_h
#define Menu_h

#include "Arduino.h"


#define MENU_NAME_LEN 13

class MenuItem  // 20 bytes total
{
  public:
    char *name;  // 13 bytes
    char type;                 // 1 byte
    void *function;            // 2 bytes
    MenuItem(char* _name, char _type, void* _function);
    MenuItem(){};
};

class SubMenu 
{
    public:
        MenuItem *menuItems;
        int currentIndex;
        int maxIndex;
        SubMenu *parent_sub_menu;
        SubMenu(MenuItem * _menuItems, int _currentIndex, int _maxIndex, SubMenu *_parent_sub_menu);

};

class Menu
{
  public:
    Menu(SubMenu *mainMenu);
    SubMenu* currentMenu(); 
    MenuItem* currentMenuItem();
    void select();
    void up();
    void down();
    void back();
  private:
    SubMenu* current_sub_menu;
};

#endif