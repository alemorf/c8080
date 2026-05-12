/*
 * c8080 stdlib
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdbool.h>
#include <c8080/halconsts.h>

/*** Direct video ***/

/* Reset screen to default settings (such as black background color) and clear the screen */
void ResetScreen(void) __link("c8080/hal_h/resetscreen.c");

/* Draw text on the screen.
 * "tile" and "x" must be calculated by DRAWTEXTARG macro. */
void __global DrawText(void *tile, uint8_t x, uint8_t color, const char *text) __link("c8080/hal_h/drawtext.c");

/* Draw text at the screen. */
void __global DrawTextXY(uint8_t x, uint8_t y, uint8_t color, const char *text) __link("c8080/hal_h/drawtextxy.c");

/* Replace colors in screen rect
 * The rect size and position are multiples of the tile size.
 * The "tile" value must be calculated using the TILE macro. */
void __global ChangeTileColor(void *tile, uint8_t color, uint8_t width, uint8_t height)
    __link("c8080/hal_h/changetilecolor.c");

/* Quick draw an image on the screen.
 * The image size and position are multiples of the tile size.
 * The "tile" value must be calculated using the TILE macro. */
void __global DrawImageTile(void *tile, const void *image) __link("c8080/hal_h/drawimagetile.c");

/* Quick draw an image on the screen.
 * The image size and position are multiples of the tile size. */
void DrawImageTileXY(uint8_t x, uint8_t y, const void *image) __link("c8080/hal_h/drawimagetilexy.c");

/* Quick draw an image on the screen.
 * The image size and position are multiples of the tile size.
 * One of the image colors is changed to the color from args.
 * The "tile" value must be calculated using the TILE macro. */
void __global DrawImageColorTile(void *tile, uint8_t color, const void *image)
    __link("c8080/hal_h/drawimagecolortile.c");

/* Quick draw an image on the screen.
 * One of the image colors is changed to the color from args.
 * The image size and position are multiples of the tile size. */
void DrawImageColorTileXY(uint8_t x, uint8_t y, uint8_t color, const void *image)
    __link("c8080/hal_h/drawimagecolortilexy.c");

/* Beautifully draw the compressed full-screen image.
 * The "tempBuffer" value is a buffer of DRAW_SCREEN_TEMP_BUFFER_SIZE bytes */
void DrawScreenBuffer(void *tempBuffer, const void *image) __link("c8080/hal_h/drawscreen.c");

/* Beautifully draw the compressed full-screen image.
 * Buffer allocated on stack */
void DrawScreen(const void *image) __link("c8080/hal_h/drawscreen.c");

/*** Sound ***/

/* Play sound.
 * 1 period equals 30 CPU ticks.
 * 1 count is 1 oscillation period (+ and -). */
void __global Sound(uint8_t period, uint16_t count) __link("c8080/hal_h/sound.c");

/*** Keyboard ***/

/* Get pressed key number without delay.
 * The function returns 0xFF if no key is pressed.
 * Shift and layout are not used. */
uint8_t ScanKey(void);

/* Get pressed key character code without delay.
 * The function returns 0xFF if no key is pressed.
 * Shift and layout are not used. */
uint8_t InKey(void);

/* Converts the pressed key number to the character code.
 * Shift and layout are not used. */
uint8_t DecodeInKey(uint8_t n);

/* Check the state of the shift key */
bool IsShiftPressed(void);

/* Get the character code of the pressed key with a delay.
 * The function waits if no key is pressed.
 * Shift and layout are used. */
uint8_t ReadKey(void);

/* Converts the pressed key number to the character code.
 * Shift and layout are used.
 * The function returns 0xFF if a service key was processed
 * (for example, switching the layout) */
uint8_t DecodeReadKey(uint8_t n);

/* Save screen, current color, cursor position, cursor visibility */
/* FEATURE_HAL_SAVE_SCREEN if supported */
struct SavedScreen;
void SaveScreen(struct SavedScreen *s) __link("c8080/hal_h/savescreen.c");

/* Restore screen, current color, cursor position, cursor visibility */
/* FEATURE_HAL_SAVE_SCREEN if supported */
void RestoreScreen(struct SavedScreen *s) __link("c8080/hal_h/restorescreen.c");
