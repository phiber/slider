#include "Config.h"


int Config::initSD() {
  Serial.begin(9600);
  //while (!Serial) {}  // wait for Leonardo
  //delay(400);  // catch Due reset problem


 // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(SD_CS_PIN, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
  }

  return 0;
}



int Config::loadTo(config_t* configuration) {

  Config::initSD();
  SdFile myFile;  

  // re-open the file for reading:
  if (!myFile.open("config.json", O_READ)) {
    sd.errorHalt("opening config.json for read failed");
    return -1;
  }
  Serial.println("config.json:");

  // read from the file until there's nothing else in it:
  int data;
  char json[2048];
  StaticJsonBuffer<2048> jsonBuffer;
  long index = 0;
  while ((data = myFile.read()) >= 0) {
    Serial.write(data);
    json[index++] = data;
  }
  // close the file:
  myFile.close();

  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    Serial.println("parseObject() failed");
     return -1;
  }


  configuration->_direction = root["direction"];
  configuration->initSpeed = root["initSpeed"];
  configuration->frames = root["frames"];
  configuration->bulbTime = root["bulbTime"];
  configuration->timedHours = root["timed"]["hours"];
  configuration->timedMinutes = root["timed"]["minutes"];
  configuration->timedSeconds = root["timed"]["seconds"];
  configuration->timedLaps = root["timed"]["laps"];

  return 0;

}

int Config::save(config_t* configuration) {
  Config::initSD(); 
 
  // if the file opened okay, write to it:
  Serial.print("Writing to config.json...");

  StaticJsonBuffer<2048> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  
  root["direction"] =  configuration->_direction;
  root["initSpeed"] = configuration->initSpeed;
  root["frames"] = configuration->frames;
  root["bulbTime"] = configuration->bulbTime;
  JsonObject& timed = root.createNestedObject("timed");

  timed["hours"] = configuration->timedHours;
  timed["minutes"] = configuration->timedMinutes;
  timed["seconds"] = configuration->timedSeconds;
  timed["laps"] = configuration->timedLaps;

  root.prettyPrintTo(Serial);
 

  SdFile myFile;
 // open the file for write at end like the Native SD library
  if (!myFile.open("config.json", O_RDWR | O_CREAT)) {
    sd.errorHalt("opening config.json for write failed");
    return -1;
  }
  root.printTo(myFile);

  // close the file:
  myFile.close();
  Serial.println("done.");
  return 0;
}
