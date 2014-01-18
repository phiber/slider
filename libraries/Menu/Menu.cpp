
#include "Arduino.h"
#include "Menu.h"

 

Menu::Menu(menu_item *mainMenu) {
  current_menu = mainMenu;
  current_menu_item_index = 0;
}



menu_item *Menu::currentMenu() {
	return current_menu;
}

menu_item* Menu::currentMenuItem(){
	return &current_menu[current_menu_item_index];
}

void Menu::select() {

	menu_item *selected = currentMenuItem();
	if (&selected.type == 'M') {
		current_menu = &selected.function;
	}

}
