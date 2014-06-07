
#include "Arduino.h"
#include "Menu.h"
#include <StackArray.h>
 

Menu::Menu(sub_menu *mainMenu) {
  current_sub_menu = mainMenu;
}

sub_menu* Menu::currentMenu() {
	return current_sub_menu;
}

menu_item* Menu::currentMenuItem(){
	int index = current_sub_menu->currentIndex;
	return &current_sub_menu->menuItems[index];
}

void Menu::select() {
	menu_item *selected = currentMenuItem();
	if (selected->type == 'M') {
		Serial.print("Going to push: "); Serial.println((int)current_sub_menu);
		previous_menus.push(current_sub_menu);
		Serial.print("Peek: "); Serial.println((int)previous_menus.peek());
		current_sub_menu = (sub_menu*) selected->function;
		Serial.print("New value for current_sub_menu: "); Serial.println((int)current_sub_menu);

		int index = current_sub_menu->currentIndex;
		menu_item *currentMenuItem = &current_sub_menu->menuItems[index];
		Serial.print("Current Menu: ");
		Serial.print(currentMenuItem->name);
		Serial.print("Current Index: ");
		Serial.print(current_sub_menu->currentIndex);

	} else if (selected->type == 'F'){
		((void (*)(void)) selected->function)();
	} else {
		Serial.print(selected->type);
	}
	}

void Menu::up() {
	if(current_sub_menu->currentIndex > 0){
		current_sub_menu->currentIndex--;
	}
}

void Menu::down() {
	if (current_sub_menu->currentIndex < current_sub_menu->maxIndex) {
		current_sub_menu->currentIndex++;		
	}
} 

void Menu::back() {
	if (!previous_menus.isEmpty()) {
		Serial.print("Peek before pop: "); Serial.println((int)previous_menus.peek());
		sub_menu *previousItem = previous_menus.pop();
		Serial.print("Popped: "); Serial.println((int)previousItem);
		current_sub_menu = previousItem;
		int index = current_sub_menu->currentIndex;
		menu_item *currentMenuItem = &current_sub_menu->menuItems[index];
		Serial.print("Current Menu: ");
		Serial.print(currentMenuItem->name);
		Serial.print("Current Index: ");
		Serial.print(current_sub_menu->currentIndex);	

	}
}
