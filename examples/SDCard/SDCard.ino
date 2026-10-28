#include <WirelessTagTiny.h>

// ESP32-S31 SD is intentionally unsupported by this standalone library.
// beginSD() returns false and leaves its SD controller and pad LDO untouched.

void setup()
{
    Serial.begin(115200);
    delay(1000);
    if (!Tiny.begin() || !Tiny.beginSD()) {
#if defined(CONFIG_IDF_TARGET_ESP32S31)
        Serial.println("S31 SD is intentionally unsupported; SD controller and pad LDO were left untouched");
#else
        Serial.println("SD unavailable, unsupported, or mount failed; no format attempted");
#endif
        return;
    }

    fs::FS *card = Tiny.sd();
    if (card == nullptr) {
        Serial.println("SD filesystem unavailable");
        Tiny.endSD();
        return;
    }
    File root = card->open("/");
    if (!root || !root.isDirectory()) {
        Serial.println("Cannot open SD root directory");
    } else {
        File entry = root.openNextFile();
        while (entry) {
            Serial.printf("%s%s\n", entry.name(), entry.isDirectory() ? "/" : "");
            entry.close();
            entry = root.openNextFile();
        }
    }
    root.close();
    Tiny.endSD();
}

void loop()
{
    delay(1000);
}
