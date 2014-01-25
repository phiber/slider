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

menu_item menu_main[] =
{
    { "Trigger     ", 'M', (void*)menu_trigger },
    { "Some funtion", 'F',(void*)some_function }
};

void setup() {
  Serial.begin(9600);

}
  
test(triggerMenuIsPreselected) {
	Menu menu((menu_item*)menu_main);
    menu_item *expected = (menu_item*)menu_main;

	menu_item *current = menu.currentMenu();

	assertTrue(current == expected);
}

test(mainMenuPreselection) {
	Menu menu((menu_item*)menu_main);
    menu_item *expected = &menu_main[0];

	menu_item *current = menu.currentMenuItem();

	assertTrue(current == expected);
}

test(selectingChangesCurrentMenu) {
	Menu menu((menu_item*)menu_main);
    menu_item *expected = (menu_item*)menu_trigger;

    menu.select();
	menu_item *current = menu.currentMenu();

	assertTrue(current == expected);
}

test(callMenuFunction) {
	Menu menu((menu_item*)menu_trigger);
	functionCalled = false;
	
	menu.select();
	
	assertTrue(functionCalled);
}

test(selectNextMenuItem) {	
	Menu menu((menu_item*)menu_main);
    menu_item *expected = &menu_main[1];
	
	menu.down();
	menu_item *current = menu.currentMenuItem();
	
	assertTrue(current == expected);
}

test(selectPreviousMenuItem) {	
	Menu menu((menu_item*)menu_main);
    menu_item *expected = &menu_main[0];
	
	menu.down();
	menu.up();
	menu_item *current = menu.currentMenuItem();
	
	assertTrue(current == expected);
}

test(navigateBackToPreviousMenu) {	
	Menu menu((menu_item*)menu_main);
    menu_item *expected = (menu_item*)menu_main;
	
	menu.select();
	menu.back();
	menu_item *current = menu.currentMenu();
	
	assertTrue(current == expected);
}


void loop() {
  suite.run();
}