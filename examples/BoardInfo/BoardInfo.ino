#include <WirelessTagTiny.h>

void setup()
{
    Serial.begin(115200);
    delay(1000);
    if (!Tiny.begin()) {
        Serial.println("TINY initialization failed");
        return;
    }

    Serial.printf("Board: %s\n", Tiny.boardName());
    Serial.printf("Chip: %s, revision %u\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("CPU: %u MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Flash: %lu bytes\n", (unsigned long)ESP.getFlashChipSize());
    Serial.printf("PSRAM: %lu bytes\n", (unsigned long)ESP.getPsramSize());
    Serial.printf("RGB GPIO: %d; button GPIO: %d (active low)\n", WT_RGB_PIN, WT_BUTTON_PIN);
    Serial.printf("SD socket fitted: %s\n", WT_HAS_SDMMC ? "yes" : "no");
#if defined(CONFIG_IDF_TARGET_ESP32S31)
    Serial.println("SD helper: unsupported on S31 with the unmodified Arduino core");
#endif
}

void loop()
{
    delay(1000);
}
