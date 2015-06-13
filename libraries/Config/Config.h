#ifndef Config_h
#define Config_h


#define SD_CS_PIN SS

#ifndef SPI_h
#include <SPI.h>
#define SPI_h
#endif


#ifndef SdFat_h
#include <SdFat.h>
#define SdFat_h
#endif



class Config {
 public:
  Config() {}
  int save();
  int load();
  //int getFrames();
  //void setFrames(int frames);
private:
  SdFat sd;
  int initSD();
};

#endif