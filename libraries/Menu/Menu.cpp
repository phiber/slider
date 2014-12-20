
#include "Arduino.h"
#include "Menu.h"

 

Menu::Menu(SubMenu *mainMenu) {
  current_sub_menu = mainMenu;

}

SubMenu* Menu::currentMenu() {
	return current_sub_menu;
}

MenuItem* Menu::currentMenuItem(){
	int index = current_sub_menu->currentIndex;
	return &current_sub_menu->menuItems[index];
}

void Menu::select() {
	MenuItem *selected = currentMenuItem();
	if (selected->type == 'M') {
		current_sub_menu = (SubMenu*) selected->function;
		int index = current_sub_menu->currentIndex;
		MenuItem *currentMenuItem = &current_sub_menu->menuItems[index];
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
	if (current_sub_menu->parent_sub_menu != NULL) {
		current_sub_menu = current_sub_menu->parent_sub_menu;
		MenuItem *selected = currentMenuItem();
	}
}

SubMenu::SubMenu(MenuItem * _menuItems, int _currentIndex, int _maxIndex, SubMenu *_parent_sub_menu){
	menuItems = _menuItems;
	currentIndex = _currentIndex;
	maxIndex = _maxIndex;
	parent_sub_menu = _parent_sub_menu;
}

MenuItem::MenuItem(char* _name, char _type, void* _function){
	name = _name;
	type = _type;
	function = _function;
}
