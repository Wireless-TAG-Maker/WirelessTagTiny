#pragma once
#include <cstdint>
#define LED_COLOR_ORDER_GRB 5
extern int rgbCalls, rgbPin, rgbOrder, rgbR, rgbG, rgbB;
inline void rgbLedWriteOrdered(int p,int o,uint8_t r,uint8_t g,uint8_t b) { ++rgbCalls;rgbPin=p;rgbOrder=o;rgbR=r;rgbG=g;rgbB=b; }
