#pragma once
#define FSPI 0
extern int spiBeginCalls,spiEndCalls,spiCLK,spiMISO,spiMOSI,spiCS;
extern bool spiOK;
struct SPIClass { explicit SPIClass(int b) {} bool begin(int c,int i,int o,int s) { ++spiBeginCalls;spiCLK=c;spiMISO=i;spiMOSI=o;spiCS=s;return spiOK; } void end() { ++spiEndCalls; } };
