
#include "Arduino.h"
#include "Menu.h"
#include <StackArray.h>
 

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
	if (selected->type == 'M') {
		previous_menus.push(current_menu);
		current_menu_item_index = 0;
		current_menu = (menu_item*) selected->function;
	} else if (selected->type == 'F'){
		((void (*)(void)) selected->function)();
	} else {
		Serial.print(selected->type);
	}
}

void Menu::up() {
	if(current_menu_item_index>0){
		current_menu_item_index--;
	}
}

void Menu::down() {
	int newIndex = current_menu_item_index+1;
	menu_item *nextItem = &current_menu[newIndex];
	if (nextItem->type != 'T') {
		current_menu_item_index++;		
	}
} 

void Menu::back() {
	menu_item *previousItem = previous_menus.pop();
	// Serial.print("previousItem = "+previousItem);
	current_menu = previousItem;
}
