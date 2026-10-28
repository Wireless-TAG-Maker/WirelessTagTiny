#pragma once
#include "FS.h"
#include "SPI.h"
#include <cassert>
struct MockSPISD : fs::FS {
 int beginCalls=0,endCalls=0;
 bool mountOK=true;
 bool begin(int cs,SPIClass &,int freq,const char *,int maxfiles,bool format) { ++beginCalls;assert(freq==20000000&&maxfiles==5&&!format);assert(cs==42);return mountOK; }
 void end() { ++endCalls; }
};
extern MockSPISD SD;
