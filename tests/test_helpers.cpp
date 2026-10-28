#include <cassert>
#include <cstring>
#include "WirelessTagTiny.h"
#include "SD_MMC.h"
#include "SD.h"
int pinCalls=0,readLevel=1,lastPin=-1;
int rgbCalls=0,rgbPin=-1,rgbOrder=-1,rgbR=-1,rgbG=-1,rgbB=-1;
MockSD SD_MMC;
MockSPISD SD;
int spiBeginCalls=0,spiEndCalls=0,spiCLK=-1,spiMISO=-1,spiMOSI=-1,spiCS=-1;
bool spiOK=true;
int acquireCalls=0,releaseCalls=0,errorLogs=0;
bool acquireOK=true,releaseOK=true;
int main() {
 assert(Tiny.sd()==nullptr);assert(!Tiny.beginSD());assert(!Tiny.buttonPressed());Tiny.setRGB(1,2,3);assert(rgbCalls==0);
 assert(!std::strcmp(Tiny.boardName(),WT_BOARD_NAME));
 assert(Tiny.begin());assert(Tiny.begin());assert(pinCalls==1);assert(lastPin==WT_BUTTON_PIN);
 assert(!Tiny.buttonPressed());readLevel=0;assert(Tiny.buttonPressed());
 Tiny.setRGB(1,2,3);assert(rgbCalls==1&&rgbPin==WT_RGB_PIN&&rgbOrder==5&&rgbR==1&&rgbG==2&&rgbB==3);
 Tiny.clearRGB();assert(rgbCalls==2&&rgbR==0&&rgbG==0&&rgbB==0);
#if WT_HAS_SDMMC && WT_SD_USE_SPI
 spiOK=false;assert(!Tiny.beginSD());assert(Tiny.sd()==nullptr);assert(SD.beginCalls==0&&spiEndCalls==1);
 spiOK=true;SD.mountOK=false;assert(!Tiny.beginSD());assert(Tiny.sd()==nullptr);assert(spiEndCalls==2&&SD.endCalls==1);
 SD.mountOK=true;assert(Tiny.beginSD());assert(Tiny.sd()==&SD);
 assert(spiCLK==WT_SD_CLK&&spiMISO==WT_SD_D0&&spiMOSI==WT_SD_CMD&&spiCS==WT_SD_D3);
 int calls=SD.beginCalls;assert(Tiny.beginSD());assert(SD.beginCalls==calls);
 Tiny.endSD();Tiny.endSD();assert(Tiny.sd()==nullptr);assert(SD.endCalls==2&&spiEndCalls==3);
#elif WT_HAS_SDMMC && defined(CONFIG_IDF_TARGET_ESP32S31)
 assert(!Tiny.beginSD());assert(!Tiny.beginSD());Tiny.endSD();Tiny.endSD();
 assert(Tiny.sd()==nullptr);
 assert(SD_MMC.pinCalls==0&&SD_MMC.powerCalls==0&&SD_MMC.beginCalls==0&&SD_MMC.endCalls==0);
 assert(acquireCalls==0&&releaseCalls==0&&spiBeginCalls==0&&spiEndCalls==0&&SD.beginCalls==0);
#elif WT_HAS_SDMMC
 SD_MMC.pinsOK=false;assert(!Tiny.beginSD());assert(SD_MMC.powerCalls==0&&SD_MMC.beginCalls==0);
 SD_MMC.pinsOK=true;SD_MMC.powerOK=false;assert(!Tiny.beginSD());assert(SD_MMC.beginCalls==0);
 SD_MMC.powerOK=true;SD_MMC.mountOK=false;assert(!Tiny.beginSD());Tiny.endSD();assert(SD_MMC.endCalls==0);
 SD_MMC.mountOK=true;assert(Tiny.beginSD());assert(SD_MMC.power==WT_SD_LDO_CHANNEL);
 int calls=SD_MMC.beginCalls;assert(Tiny.beginSD());assert(SD_MMC.beginCalls==calls);
 assert(Tiny.sd()==&SD_MMC);Tiny.endSD();Tiny.endSD();assert(Tiny.sd()==nullptr);assert(SD_MMC.endCalls==1);
#else
 assert(!Tiny.beginSD());Tiny.endSD();assert(SD_MMC.pinCalls==0&&SD_MMC.powerCalls==0&&SD_MMC.beginCalls==0&&SD_MMC.endCalls==0);
#endif
}
