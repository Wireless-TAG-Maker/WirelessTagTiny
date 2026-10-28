#pragma once
#include <cstdint>
#include "pins_arduino.h"
#define INPUT_PULLUP 2
#define LOW 0
extern int pinCalls, readLevel, lastPin;
inline void pinMode(int p,int mode) { ++pinCalls;lastPin=p; }
inline int digitalRead(int p) { return readLevel; }

extern int errorLogs;
#define log_e(...) (++errorLogs)
