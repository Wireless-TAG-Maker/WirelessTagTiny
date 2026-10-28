/**
 * @file WirelessTagTiny.h
 * @author Wireless-Tag
 * @brief Arduino helpers for the selected Wireless-Tag TINY variant.
 * @version 0.1.0
 * @date 2026-10-28
 * @copyright Copyright (c) 2026, Wireless-Tag. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Modified for standalone distribution with an explicit S31 SD limitation.
 */

#ifndef WIRELESS_TAG_TINY_H
#define WIRELESS_TAG_TINY_H

/* ==================== [Includes] ========================================== */

#include <Arduino.h>
#include <FS.h>

/* ==================== [Defines] =========================================== */

#if !defined(WT_BOARD_NAME) || !defined(WT_RGB_PIN) || !defined(WT_BUTTON_PIN) || !defined(WT_HAS_SDMMC)
#error "Select a supported Wireless-Tag TINY board before using WirelessTagTiny."
#endif

/* ==================== [Typedefs] ========================================== */

/**
 * @brief Board helpers backed by the Arduino core and the selected variant.
 *
 * Use the global Tiny object. Hardware resources stay with the Arduino core;
 * begin() does not initialize SD, Wi-Fi, display, camera or their power pins.
 * Calls must be serialized by the application. The library does not debounce
 * the button or provide interrupt callbacks.
 */
class WirelessTagTinyClass {
public:
    /** @brief Configure the active-low button with a pull-up; safe to repeat. */
    bool begin();

    /**
     * @brief Set the onboard WS2812 LED in GRB wire order after begin().
     * @param[in] red Red channel, 0..255.
     * @param[in] green Green channel, 0..255.
     * @param[in] blue Blue channel, 0..255.
     * @note The Arduino RGB API has no success result; errors go to core logs.
     */
    void setRGB(uint8_t red, uint8_t green, uint8_t blue);

    /** @brief Switch off the onboard RGB LED after begin(). */
    void clearRGB();

    /** @brief Return the current button level; false before begin(). */
    bool buttonPressed() const;

    /** @brief Return the selected variant's static board name. */
    const char *boardName() const;

    /**
     * @brief Mount the supported onboard SD socket at /sdcard.
     * @return true when mounted; false before begin(), on unsupported boards,
     *         or if pin, power or filesystem setup fails. Never formats a card.
     * @note Uses SPI on P4C61 and 4-bit SDMMC on P4/P4X. S31 returns false
     *       without touching the SD controller or its 1.8 V pad LDO.
     * @note Owns the corresponding global SD or SD_MMC instance until endSD();
     *       do not independently mount that instance while using this helper.
     */
    bool beginSD();

    /**
     * @brief Return the mounted SD filesystem or nullptr before/after mounting.
     * @return Borrowed Arduino filesystem; never delete the returned pointer.
     *         Close all File objects before endSD().
     */
    fs::FS *sd() const;

    /** @brief Unmount a card mounted by beginSD(); safe to repeat. */
    void endSD();

private:
    bool initialized_ = false;
    bool sdMounted_ = false;
    fs::FS *sdFilesystem_ = nullptr;
};

/* ==================== [Global Prototypes] ================================= */

/** @brief Singleton board helper; initialization is explicit through begin(). */
extern WirelessTagTinyClass Tiny;

#endif // WIRELESS_TAG_TINY_H
