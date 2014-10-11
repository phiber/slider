
//int optoFocus = 3;
int optoTrigger = 10;
long bulbTime = 1000;
long triggerInterval = 100;
long triggerMillis = 0;
bool triggerPressed = false;
//bool focused = false;


// the setup routine runs once when you press reset:
void setup()
{
// initialize the digital pin as an output.
pinMode(optoFocus, OUTPUT);
pinMode(optoTrigger, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop()
{
   if (!triggerPressed && ((unsigned long)(millis() - triggerMillis) >= triggerInterval)) {
    digitalWrite(optoFocus, HIGH);
    triggerMillis = millis();
    triggerPressed = true;
   }
   if(triggerPressed && ((unsigned long)(millis() - triggerMillis) >= bulbTime)) {
    digitalWrite(optoFocus, LOW);
    triggerMillis = millis();
    triggerPressed = false;
   }
   
}
