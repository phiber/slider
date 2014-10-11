#include <StackArray.h>


struct menu_item  // 20 bytes total
{
    char name[13];  // 13 bytes
    char type;                 // 1 byte
    void *function;            // 2 bytes
};

struct sub_menu 
{
    menu_item *menuItems;
    int currentIndex;
    int maxIndex;
};


void some_function() {

}



menu_item menu_trigger[] = {
  { "SubTrigger", 'F',  (void*)some_function},
  { "SubTrigger2", 'F',  (void*)some_function}
};

sub_menu sub_menu_trigger = {
  menu_trigger, 0, 1
};


menu_item menu_main[] =
{
    { "Trigger     ", 'M', &sub_menu_trigger },
    { "Some funtion", 'F',(void*)some_function }
};

sub_menu sub_menu_main = {
  menu_main, 0, 1
};


sub_menu *a_pointer = &sub_menu_main;
sub_menu *b_pointer = &sub_menu_trigger;


// create a stack of numbers.
StackArray <sub_menu*> stack;

// startup point entry (runs once).
void setup () {
  // start serial communication.
  Serial.begin (9600);

  // set the printer of the stack.
  stack.setPrinter (Serial);
}

// loop the main sketch.
void loop () {
  


  // print the values of the numbers.
  int index_menu_a = a_pointer->currentIndex;
  int index_menu_b = b_pointer->currentIndex;
  

  Serial.print ("a: "); Serial.println (a_pointer->menuItems[index_menu_a].name); 
  Serial.print ("b: "); Serial.println (b_pointer->menuItems[index_menu_b].name);
 
  // push the numbers to the stack.
  Serial.print("Going to push: "); Serial.println((int)a_pointer);
  stack.push (a_pointer);
  Serial.print("Going to push: "); Serial.println((int)b_pointer);
  stack.push (b_pointer);

  // pop the numbers from the stack.
  a_pointer = stack.pop ();
  Serial.print("Popped: "); Serial.println((int)a_pointer);
  b_pointer = stack.pop ();
  Serial.print("Popped: "); Serial.println((int)b_pointer);
 
  // delay 1 second.
  delay (1000);
}