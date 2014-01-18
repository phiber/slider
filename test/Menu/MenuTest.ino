#include <ArduinoUnit.h>
#include <Menu.h>

TestSuite suite;


void do_trigger() {

}

menu_item menu_trigger[] = {
	{ "DoTrigger", 'F',  (void*)do_trigger}
};

menu_item menu_main[] =
{
    { "Trigger     ", 'M', (void*)menu_trigger }
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

void loop() {
  suite.run();
}