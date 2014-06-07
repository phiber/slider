#include <ArduinoUnit.h>
#include <Menu.h>
#include <StackArray.h>

TestSuite suite;

bool functionCalled;

 
 
void some_function() {

	functionCalled = true;
}

menu_item menu_trigger[] = {
	{ "DoTrigger", 'F',  (void*)some_function}
};

sub_menu sub_menu_trigger = {
	menu_trigger, 0, 0
};

menu_item menu_main[] =
{
    { "Trigger", 'M', &sub_menu_trigger },
    { "Somefunction", 'F',(void*)some_function }
};

sub_menu sub_menu_main = {
	menu_main, 0, 1
};

void setup() {
  Serial.begin(9600);

}
  
test(triggerMenuIsPreselected) {
	Menu menu(&sub_menu_main);
    sub_menu *expected = &sub_menu_main;

	sub_menu *current = menu.currentMenu();

	assertTrue(current == expected);
}

test(mainMenuPreselection) {
	Menu menu(&sub_menu_main);
    menu_item *expected = &menu_main[0];

	menu_item *current = menu.currentMenuItem();

	assertTrue(current == expected);
}

// test(selectingChangesCurrentMenu) {
// 	Menu menu(&sub_menu_main); 
//     sub_menu *expected = &menu_trigger;

//     menu.select();
// 	sub_menu *current = menu.currentMenu();

// 	assertTrue(current == expected);
// }

test(callMenuFunction) {
	Menu menu(&sub_menu_trigger);
	functionCalled = false;
	
	menu.select();
	
	assertTrue(functionCalled);
}

// test(selectNextMenuItem) {	
// 	Menu menu(&sub_menu_main);
//     menu_item *expected = &sub_menu_main->menuItems[1];
	
// 	menu.down();
// 	menu_item *current = menu.currentMenuItem();
	
// 	assertTrue(current == expected);
// }

test(selectPreviousMenuItem) {	
	Menu menu(&sub_menu_main);
    menu_item *expected = &menu_main[0];
	
	menu.down();
	menu.up();
	menu_item *current = menu.currentMenuItem();
	
	assertTrue(current == expected);
}

test(navigateBackToPreviousMenu) {	
	Menu menu(&sub_menu_main);
    sub_menu *expected = &sub_menu_main;
	
	menu.select();
	
// assertTrue(menu.currentMenu() == &sub_menu_trigger);
	menu.up();
	menu.down();
	menu.back();
	sub_menu *current = menu.currentMenu();
	
	int index = current->currentIndex;
	menu_item *currentMenuItem = &current->menuItems[index];
	Serial.print(currentMenuItem->name);
	Serial.print(current->currentIndex);
	assertTrue(current == expected);
	assertTrue(current->currentIndex == expected->currentIndex);
}

void loop() {
  suite.run();
}