/**
 * @file WirelessTagTiny.cpp
 * @author Wireless-Tag
 * @brief Standalone Arduino helpers for TINY button, RGB and supported SD.
 * @version 0.1.0
 * @date 2026-10-28
 * @copyright Copyright (c) 2026, Wireless-Tag. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Modified for standalone distribution; S31 SD does not use a patched core.
 */

/* ==================== [Includes] ========================================== */

#include "WirelessTagTiny.h"

#include "soc/soc_caps.h"
#include "esp32-hal-rgb-led.h"

#if WT_HAS_SDMMC && WT_SD_USE_SPI && !defined(CONFIG_IDF_TARGET_ESP32S31)
#include <SD.h>
#include <SPI.h>
#elif WT_HAS_SDMMC && SOC_SDMMC_HOST_SUPPORTED && !defined(CONFIG_IDF_TARGET_ESP32S31)
#include <SD_MMC.h>
#endif

/* ==================== [Defines] =========================================== */

#if !SOC_RMT_SUPPORTED
#error "WirelessTagTiny RGB helpers require an Arduino target with RMT support."
#endif

/* ==================== [Static Variables] ================================== */

WirelessTagTinyClass Tiny;

#if WT_HAS_SDMMC && WT_SD_USE_SPI && !defined(CONFIG_IDF_TARGET_ESP32S31)
static SPIClass s_sdSPI(FSPI);
#endif

/* ==================== [Global Functions] ================================== */

bool WirelessTagTinyClass::begin()
{
    if (!initialized_) {
        pinMode(WT_BUTTON_PIN, INPUT_PULLUP);
        initialized_ = true;
    }
    return true;
}

void WirelessTagTinyClass::setRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    if (initialized_) {
        rgbLedWriteOrdered(WT_RGB_PIN, LED_COLOR_ORDER_GRB, red, green, blue);
    }
}

void WirelessTagTinyClass::clearRGB()
{
    setRGB(0, 0, 0);
}

bool WirelessTagTinyClass::buttonPressed() const
{
    return initialized_ && digitalRead(WT_BUTTON_PIN) == LOW;
}

const char *WirelessTagTinyClass::boardName() const
{
    return WT_BOARD_NAME;
}

bool WirelessTagTinyClass::beginSD()
{
    if (!initialized_) {
        return false;
    }
    if (sdMounted_) {
        return true;
    }

#if defined(CONFIG_IDF_TARGET_ESP32S31)
    // The unmodified core cannot express this board's separate 1.8 V pad rail
    // through its SD_MMC API. Leave the controller and LDO untouched.
    return false;
#elif WT_HAS_SDMMC && WT_SD_USE_SPI
    if (!s_sdSPI.begin(WT_SD_CLK, WT_SD_D0, WT_SD_CMD, WT_SD_D3)) {
        s_sdSPI.end();
        return false;
    }
    if (!SD.begin(WT_SD_D3, s_sdSPI, 20000000, "/sdcard", 5, false)) {
        SD.end();
        s_sdSPI.end();
        return false;
    }
    sdMounted_ = true;
    sdFilesystem_ = &SD;
    return true;
#elif WT_HAS_SDMMC && SOC_SDMMC_HOST_SUPPORTED
#if WT_SD_LDO_CHANNEL >= 0 && WT_SD_LDO_MV != 3300
    return false;
#else
    if (!SD_MMC.setPins(WT_SD_CLK, WT_SD_CMD, WT_SD_D0, WT_SD_D1, WT_SD_D2, WT_SD_D3)) {
        return false;
    }
#if SOC_SDMMC_IO_POWER_EXTERNAL
    if (!SD_MMC.setPowerChannel(WT_SD_LDO_CHANNEL)) {
        return false;
    }
#elif WT_SD_LDO_CHANNEL >= 0
    return false;
#endif
    // Use 20 MHz initially. A mount failure must never format the user's card.
    sdMounted_ = SD_MMC.begin("/sdcard", false, false, SDMMC_FREQ_DEFAULT);
    if (sdMounted_) {
        sdFilesystem_ = &SD_MMC;
    }
    return sdMounted_;
#endif
#else
    // C3/C5 have no onboard socket.
    return false;
#endif
}

fs::FS *WirelessTagTinyClass::sd() const
{
    return sdFilesystem_;
}

void WirelessTagTinyClass::endSD()
{
#if WT_HAS_SDMMC && WT_SD_USE_SPI && !defined(CONFIG_IDF_TARGET_ESP32S31)
    if (sdMounted_) {
        SD.end();
        s_sdSPI.end();
        sdMounted_ = false;
        sdFilesystem_ = nullptr;
    }
#elif WT_HAS_SDMMC && SOC_SDMMC_HOST_SUPPORTED && !defined(CONFIG_IDF_TARGET_ESP32S31)
    if (sdMounted_) {
        SD_MMC.end();
        sdMounted_ = false;
        sdFilesystem_ = nullptr;
    }
#endif
}
