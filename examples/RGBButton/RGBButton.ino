#include <WirelessTagTiny.h>

bool ready = false;
bool previousPressed = false;

void setup()
{
    Serial.begin(115200);
    ready = Tiny.begin();
    if (ready) {
        // Low brightness: green when released, red while pressed.
        Tiny.setRGB(0, 8, 0);
    }
}

void loop()
{
    if (ready) {
        bool pressed = Tiny.buttonPressed();
        if (pressed != previousPressed) {
            Tiny.setRGB(pressed ? 8 : 0, pressed ? 0 : 8, 0);
            Serial.println(pressed ? "Button pressed" : "Button released");
            previousPressed = pressed;
        }
    }
    delay(20);
}
