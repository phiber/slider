#include <ArduinoUnit.h>
#include <Menu.h>

TestSuite suite;


void do_trigger() {

}

const menu_item menu_trigger[] = {
	{ "Trigger   ", 'F',  (void*)do_trigger}
};

const menu_item menu_main[] =
{
    { "Trigger     ", 'M', (void*)menu_trigger }
};



void setup() {
  Serial.begin(9600);
}
  
test(triggerMenuIsPreselected) {
	
	Menu menu((menu_item*)menu_main);

	menu_item *current = menu.currentMenuItem();
	assertTrue(current.name == ((menu_item*)menu_main[0]).name);

} 



void loop() {
  suite.run();
}