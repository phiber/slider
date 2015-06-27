#ifndef Config_h
#define Config_h


//#define SD_CS_PIN SS
#define SD_CS_PIN 50

#ifndef SPI_h
#include <SPI.h>
#define SPI_h
#endif


#ifndef SdFat_h
#include <SdFat.h>
#define SdFat_h
#endif


#ifndef ArduinoJson_h
#include <ArduinoJson.h>
#define ArduinoJson_h
#endif


const float DIRECTION_TOWARDS_SWITCH_2 = -1;
const float DIRECTION_TOWARDS_SWITCH_1 = 1;

typedef struct config_t
{
  float _direction;
  int timedHours;
  int timedMinutes;
  int timedSeconds;
  int timedLaps;
  int frames;
  long bulbTime;
  int initSpeed;
  config_t(){
    _direction = DIRECTION_TOWARDS_SWITCH_2;
    initSpeed = 9999;
    timedHours = 0;
    timedMinutes = 0;
    timedSeconds = 0;
    timedLaps = 0;
    frames = 0;
    bulbTime = 0;
  };
};

class Config {
 public:
  Config() {}
  int save(config_t* configuration);
  int loadTo(config_t* configuration);
private:
  SdFat sd;
  int initSD();
};

#endif