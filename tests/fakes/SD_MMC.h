#pragma once
#include "FS.h"
#define SDMMC_FREQ_DEFAULT 20000
struct MockSD : fs::FS {
 int pinCalls=0,powerCalls=0,beginCalls=0,endCalls=0,power=-2;
 bool pinsOK=true,powerOK=true,mountOK=true;
 bool setPins(int a,int b,int c,int d,int e,int f) { ++pinCalls;return pinsOK; }

#ifndef CONFIG_IDF_TARGET_ESP32S31
 bool setPowerChannel(int p) { ++powerCalls;power=p;return powerOK; }
#endif
 bool begin(const char *path,bool one,bool format,int freq) { ++beginCalls; if (one||format||freq!=20000) return false;return mountOK; }
 void end() { ++endCalls; }
};
extern MockSD SD_MMC;
