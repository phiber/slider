
#include "Arduino.h"
#include "Menu.h"
#include <StackArray.h>
 

Menu::Menu(menu_item *mainMenu) {
  
Serial.print("Menu constructor");

  current_menu = mainMenu;
  current_menu_item_index = 0;

  Serial.print("MC sizeof current_menu: ");
	Serial.print(sizeof current_menu);
	Serial.print("\n");
	Serial.print(sizeof current_menu / sizeof *current_menu);
	Serial.print("\n");
	Serial.print(sizeof *current_menu);
	Serial.print("\n");

}

menu_item *Menu::currentMenu() {
	return current_menu;
}

menu_item* Menu::currentMenuItem(){
	return &current_menu[current_menu_item_index];
}

void Menu::select() {
	Serial.print("Select Called");
	menu_item *selected = currentMenuItem();
	if (selected->type == 'M') {
		previous_menus.push(current_menu);
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
	Serial.print("sizeof current_menu: ");
	Serial.print(sizeof(&current_menu));
	Serial.print("\n");
	

	int newIndex = current_menu_item_index++;
	menu_item *nextItem = &current_menu[current_menu_item_index++];
	if (nextItem->type != 'T') {
		current_menu_item_index++;		
	}
	
} 

void Menu::back() {
	current_menu = previous_menus.pop();
}
