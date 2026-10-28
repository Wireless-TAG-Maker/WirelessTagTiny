# WirelessTagTiny

Arduino helpers for the Wireless-Tag TINY board family: onboard RGB LED,
active-low user button, board identity, and SD mounting on supported boards.

This is a standalone library. It does not contain an ESP32 core, board definitions,
wireless firmware, display, camera, touch, or USB UVC drivers.

## Requirements and installation

Use Arduino IDE 2 or Arduino CLI with Arduino-ESP32 4.0.0-RC1 or the corresponding
`release/v4.0.x` source checkout, and select one of these TINY board variants:

| Board | RGB GPIO | Button GPIO | SD helper |
| --- | --- | --- | --- |
| WT9932C3-TINY | 6 | 9 | Not fitted; returns `false` |
| WT9932C5-TINY | 6 | 28 | Not fitted; returns `false` |
| WT9932P4-TINY | 51 | 35 | 4-bit SDMMC, slot 0, LDO4 |
| WT9932P4X-TINY | 51 | 35 | 4-bit SDMMC, slot 0, LDO4 |
| WT9932P4C61-TINY | 51 | 35 | SDSPI, externally powered |
| WT9932S31-TINY | 19 | 61 | Unsupported; returns `false` without accessing SD or LDO |

Until the TINY definitions are merged upstream, use the
[Wireless-Tag Arduino-ESP32 fork](https://github.com/Wireless-TAG-Maker/arduino-esp32/tree/codex/add-wireless-tag-tiny)
and follow Espressif's
[manual core installation instructions](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).
The manual checkout uses FQBNs such as `espressif:esp32:wt9932p4_tiny`.
A generic ESP32 board selection does not provide the required `WT_*` variant
configuration; the library reports an explicit compile error in that case.

Library Manager registration is pending. Until it is listed, install the library
repository ZIP with **Sketch > Include Library > Add .ZIP Library**, or clone this
repository into your sketchbook's `libraries/WirelessTagTiny` directory.
Library Manager installs libraries, so the ESP32 core and TINY board definitions
must be installed separately. There are no external Library Manager dependencies;
`FS`, `SPI`, `SD`, and `SD_MMC` are supplied by the ESP32 core.

## Usage

```cpp
#include <WirelessTagTiny.h>

void setup()
{
    Tiny.begin();
}

void loop()
{
    Tiny.setRGB(Tiny.buttonPressed() ? 16 : 0, 0, 0);
    delay(20);
}
```

`Tiny.begin()` is repeatable and only configures the button as `INPUT_PULLUP`.
It does not mount SD, initialize the RGB peripheral, or start Wi-Fi, display or
camera drivers. Call `begin()` before the other helpers. `buttonPressed()` returns
the raw button state without debouncing. `boardName()` returns the selected
variant's name.

`setRGB(red, green, blue)` uses raw 0-255 channel values in GRB wire order;
`clearRGB()` turns the LED off. The core RGB API has no return value, so driver
failures are reported through the core logs.

## SD support and ownership

After `Tiny.beginSD()` succeeds, `Tiny.sd()` returns a borrowed `fs::FS *` for use
with Arduino `File`. It returns `nullptr` before mounting, after `endSD()`, or
when SD is unsupported. Close all files before calling `endSD()`.

P4/P4X use the core's global `SD_MMC` object with the variant's slot-0 pins and
3.3 V LDO4. P4C61 uses the global `SD` object and an owned `SPIClass(FSPI)` with
SCK43, MOSI44, MISO39 and CS42, avoiding the SDMMC controller used by ESP-Hosted.
The initial clock is 20 MHz. Mount failure never formats the card.

Do not independently mount, reconfigure or stop the same global `SD` / `SD_MMC`
object while this helper owns it. Do not use a second FSPI instance on P4C61 at
the same time. Calls to this library must be serialized by the application.

**S31 SD is intentionally unsupported in this standalone library.** Its slot-0
pads require a separate 1.8 V LDO supply, while the unmodified core's `SD_MMC` API
does not expose all of the necessary board-specific voltage configuration.
`beginSD()` therefore returns `false` without touching SD, SPI or the pad LDO;
`endSD()` is a no-op for that board. RGB, button and board information remain
available. This library neither requires nor applies a custom core patch.

P4C61 Wi-Fi is outside this library. It needs matching ESP-Hosted firmware and
transport support; no C61 wireless interoperability or simultaneous wireless/SD
operation is claimed here. This standalone release has not been validated on
physical hardware.

## Examples and verification

- `BoardInfo`: prints the board, chip, memory and pin information.
- `RGBButton`: reads the raw button level and controls the RGB LED.
- `SDCard`: lists the card root and unmounts it; never writes or formats a card.
  On S31 it reports unsupported SD and returns.

The host checks compile this library against Arduino fakes and **external,
actual board variant headers**. They cover repeated initialization, button and
GRB behavior, SD/SPI failure cleanup, filesystem ownership, and the S31 no-access
path. They do not duplicate pin definitions or prove electrical behavior.

```sh
python3 tests/run_host_tests.py --variants-dir /path/to/arduino-esp32/variants
```

CI runs these checks and compiles the core's official `CIBoardsTest` plus all
three library examples for all six variants against the Wireless-Tag
Arduino-ESP32 fork, whose core and platform recipes are unchanged. It records
the tested core commit and CLI version, and passes `--library` explicitly
so a bundled copy cannot replace the library under test. Build checks do not
prove LED timing, USB enumeration, SD power, card reads, or physical operation.
No firmware upload occurs in CI.

The CI installer reads the selected core's package manifest, downloads only its
SDK, RISC-V compiler and esptool over verified HTTPS, checks each archive's
declared size and SHA-256, and uses the unchanged core installer to extract it.

Library Manager checks can be run before submission:

```sh
arduino-lint --project-type library --library-manager submit --compliance specification .
arduino-lint --project-type library --library-manager submit --compliance strict .
```

After registration, use `--library-manager update` when checking new releases.
The CI workflow also accepts the desired mode through manual dispatch; normal
push and pull-request runs use strict checks without registry-name lookup.

## License and support

Copyright 2026 Wireless-Tag. [Apache License 2.0](LICENSE).
The original helpers were exported from the local WT_Arduino board package;
its hardware mappings are based on the
[ESP-IDF WT_BSP](https://github.com/Wireless-TAG-Maker/WT_BSP).
This standalone edition explicitly disables S31 SD instead of depending on the
board package's core modification.

Report library problems at the
[WirelessTagTiny issue tracker](https://github.com/Wireless-TAG-Maker/WirelessTagTiny/issues).
