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

#if __has_include(<c8080/consoleconsts.h>)
#include <c8080/consoleconsts.h>
#endif

/* Clear the screen */
void ClearConsole(void) __link("c8080/console_h/clearconsole.c");

/* Reset screen to default settings (such as black background color) and clear the screen */
void ResetConsole(void) __link("c8080/console_h/resetconsole.c");

/* Move console cursor to upper left corner */
void MoveCursorHome(void) __link("c8080/console_h/movecursorhome.c");

/* Set console cursor visible */
void ShowCursor(void) __link("c8080/console_h/showcursor.c");

/* Set console cursor hidden */
void HideCursor(void) __link("c8080/console_h/hidecursor.c");

/* Set ink and paper color for the next console text */
void __global SetConsoleColor(uint8_t color) __link("c8080/console_h/setconsolecolor.c");

/* Get cursor position. Low byte - x, Height byte - y. From 0. */
uint16_t GetCursorPosition(void) __link("c8080/console_h/getcursorposition.c");

/* Set cursor position. */
void __global MoveCursor(uint8_t x, uint8_t y) __link("c8080/console_h/movecursor.c");

/* Write text to console */
void WriteConsole(const char *text) __link("c8080/console_h/writeconsole.c");

/* Read console input and replace multi-byte key codes with a constant of type "KEY_" of fixed type uint16_t */
uint16_t ReadAndDecodeConsoleKeys(void) __link("c8080/console_h/readanddecodeconsolekeys.c");
