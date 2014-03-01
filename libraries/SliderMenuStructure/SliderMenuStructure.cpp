#include "SliderMenuStructure.h"



menu_item menu_trigger[] = {
  { "DoTrigger", 'F',  (void*)some_function}
};

menu_item menu_main[] =
{
    { "Trigger     ", 'M', (void*)menu_trigger },
    { "Some funtion", 'F',(void*)some_function }
};  


menu_item* createMainMenu() {
	return menu_main&;
}

void some_function() {

  functionCalled = true;
}