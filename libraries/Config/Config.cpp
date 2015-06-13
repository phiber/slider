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



int Config::load() {

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
  while ((data = myFile.read()) >= 0) {
    Serial.write(data);
  }
  // close the file:
  myFile.close();
  return 0;

}

int Config::save() {
  
  Config::initSD();
  SdFile myFile;
 // open the file for write at end like the Native SD library
  if (!myFile.open("config.json", O_RDWR | O_CREAT)) {
    sd.errorHalt("opening config.json for write failed");
    return -1;
  }
  // if the file opened okay, write to it:
  Serial.print("Writing to config.json...");
  myFile.print("testing 1, 2, 3.");

  // close the file:
  myFile.close();
  Serial.println("done.");
  return 0;
}
