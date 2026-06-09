/*
 * NC shell for CP/M
 * Copyright (c) 2026 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "panel.h"
#include <string.h>
#include <c8080/hal.h>
#include <c8080/uint32tostring.h>
#include <c8080/tolowercase.h>
#include <c8080/zerobitcount.h>
#include <c8080/console.h>
#include "arch.h"
#include "nc.h"
#include "dir.h"

static const uint8_t NO_CURSOR = 0xFF;

size_t panel_files_max;
struct Panel panel_a;
struct Panel panel_b;
uint8_t panel_x;
uint8_t panel_reload_select_dir = 0xFF;  // Глобальная переменная для оптимизации размера

#define SORT_STACK_MAX 32

static uint8_t SortFilesPred(struct FileInfo *a, struct FileInfo *b) {
    if ((a->attrib ^ b->attrib) & ATTRIB_DIR_MASK)
        return b->attrib & ATTRIB_DIR_MASK;

    int8_t r = memcmp(a->name83 + 8, b->name83 + 8, 3);
    if (r != 0)
        return r == 1;
    if (1 == memcmp(a->name83, b->name83, 8))
        return 1;
    return 0;
}

static void SortFiles(struct FileInfo *low, struct FileInfo *high) {
    struct FileInfo *stack_place[SORT_STACK_MAX * 2];
    struct FileInfo **stack = stack_place;
    uint8_t stack_count = 0;
    for (;;) {
        struct FileInfo *l = low;
        struct FileInfo *h = high;
        struct FileInfo *m = l + ((uint8_t *)h - (uint8_t *)l) / (sizeof(*h) * 2);
        do {
            while (0 != SortFilesPred(m, l))
                l++;
            while (0 != SortFilesPred(h, m))
                h--;
            if (l <= h) {
                memswap(l, h, sizeof(struct FileInfo));
                if (m == l)
                    m = h;
                else if (m == h)
                    m = l;
                l++;
                h--;
            }
        } while (h > l);
        if (l < high) {
            if (low < h && stack_count != SORT_STACK_MAX) {
                *stack = low;
                stack++;
                *stack = h;
                stack++;
                stack_count++;
            }
            low = l;
            continue;
        }
        if (low < h) {
            high = h;
            continue;
        }
        if (stack_count == 0)
            break;
        stack_count--;
        stack--;
        high = *stack;
        stack--;
        low = *stack;
    }
}

void PanelReloadOrCopy(void) {
    if (panel_a.drive_user == panel_b.drive_user) {
        memcpy(panel_a.copy_start, panel_b.copy_start, panel_a.copy_end - panel_a.copy_start);
        memcpy(panel_a.files, panel_b.files, sizeof(panel_a.files[0]) * panel_a.count);
    } else {
        PanelReload();
    }
}

void PanelReload(void) {
    panel_a.count = 0;

    // Что бы не прокручивался экран, если CP/M выводит тексты ошибок
    MoveCursor(0, 0);

    if (CpmSetDrive(PanelGetDrive()) == 0xFF)
        return;  // TODO: Вывод ошибки

    // Получение размера накопителя и свободного места
    // Тут ограничение размера накопителя в 64 МБ
    const struct DPB *const dpb = CpmGetDpb();
    panel_a.total_kb = (dpb->dsm + 1) << (dpb->bsh - 3);
    panel_a.free_kb = ZeroBitCount(CpmGetAllocationBitmap(), panel_a.total_kb / 8);

    // Добавление элемента "..", если это не корневая папка
    struct FileInfo *d = panel_a.files;
    const uint8_t dir_index = PanelGetDirIndex();
    if (dir_index != 0) {
        static const struct FileInfo parent = {"..         ", ATTRIB_DIR_UP};
        memcpy(d, &parent, sizeof(parent));
        d++;
        panel_a.count = 1;
    }

    // Подготовка к построению дерева папок
    struct DirInfo dir_info;
    DirInfoReset(&dir_info, PanelGetDrive());

    // Поиск всех файлов
    const struct FCB *x = CpmSearchFirst('?', NULL, '?');
    while (x != NULL) {
        if (x->drive < CPM_MAX_USERS) {
            d->attrib = CpmGetNameAndAttrib(d->name83, x->name83) << ATTRIB_SHIFT;

            // Построение дерева папок
            if (d->attrib & ATTRIB_DIR_MASK) {
                if (DirInfoAdd(&dir_info, x) == dir_index) {
                    // Найден предок
                    panel_a.files[0].attrib = ATTRIB_DIR_UP | (x->drive << ATTRIB_DIR_SHIFT);
                }
            }

            // Сохранение файлов нашей папки
            if (x->drive == dir_index) {
                d->blocks_128 = x->rc + (x->ex & 0x1F) * 128;  // TODO: Ограчение размера файла в 512 КБ
                d++;
                panel_a.count++;
                if (panel_a.count == panel_files_max)
                    break;  // TODO: Вывести ошибку "слишком много файлов"
            }
        }
        x = CpmSearchNext();
    }

    // Формируем путь
    DirInfoMakePath(&dir_info, panel_a.path, sizeof(panel_a.path), PanelGetDirIndex());

    // Формируем короткий путь, который влезет в экран
    panel_a.short_path_skip = 0;
    panel_a.short_path_size = strlen(panel_a.path);  // Размер panel_a.path меньше 256 байт
    if (panel_a.short_path_size > PANEL_SHORT_PATH) {
        panel_a.short_path_skip = panel_a.short_path_size - PANEL_SHORT_PATH;
        panel_a.short_path_size = PANEL_SHORT_PATH;
    }

    if (panel_a.count >= 2) {
        panel_a.count--;

        // Сортировка. Но первый элемент ".." не перемещаем.
        // TODO: Проверить на 1 элемент
        SortFiles(panel_a.files + (dir_index ? 1 : 0), panel_a.files + panel_a.count);

        // Объединение экстентов
        struct FileInfo *s = panel_a.files + 1;
        d = panel_a.files;
        do {
            if (0 == memcmp(d->name83, s->name83, sizeof(s->name83))) {
                if (d->blocks_128 < s->blocks_128)
                    d->blocks_128 = s->blocks_128;
            } else {
                d++;
                memcpy(d, s, sizeof(*d));
            }
            s++;

            // Установка курсора на папку предка при выходе из папки
            if (panel_reload_select_dir == (d->attrib & ATTRIB_DIR_MASK)) {
                // В текущей реализации в CP/M может быть 15 папок и все они поместятся
                // в одну колонку. Но если вдруг не поместятся, то ниже cursor_y будет
                // проверен и сброшен в 0.
                panel_a.offset = 0;
                panel_a.cursor_x = 0;
                panel_a.cursor_y = d - panel_a.files;
            }

            panel_a.count--;
        } while (panel_a.count != 0);
        panel_a.count = d - panel_a.files + 1;
    }

    // Предотвращение выхода за пределы буфера
    PanelGetCursorIndex();

    // Сброс режима установки курсора на папку предка при выходе из папки
    panel_reload_select_dir = 0xFF;
}

uint16_t PanelGetCursorIndex(void) {
    const uint16_t result = panel_a.offset + panel_a.cursor_y + panel_a.cursor_x * PANEL_ROWS_COUNT;

    // Предотвращение выхода за пределы буфера
    // Может быть при восстановлении состояния при запуске или после удаления файла
    if (result >= panel_a.count || panel_a.cursor_x >= PANEL_COLUMNS_COUNT || panel_a.cursor_y >= PANEL_ROWS_COUNT) {
        panel_a.cursor_x = 0;
        panel_a.cursor_y = 0;
        panel_a.offset = 0;
        return 0;
    }

    return result;
}

struct FileInfo *PanelGetCursor(void) {
    return panel_a.files + PanelGetCursorIndex();
}

uint8_t PanelGetDrive(void) {
    return panel_a.drive_user & 0x0F;
}

uint8_t PanelGetDirIndex(void) {
    return panel_a.drive_user >> 4;
}

#ifdef FULL_COLOR_MODE
void PanelDrawBorder(uint8_t x) {
    DrawTextXY(x, 0, COLOR_PANEL_BORDER, "╔══════════════════════════════╗");
    for (uint8_t y = 1; y < TEXT_HEIGHT - 6; y++)
        DrawTextXY(x, y, COLOR_PANEL_BORDER, "║              │               ║");
    DrawTextXY(x + 6, 1, COLOR_PANEL_HEADER, "Name");
    DrawTextXY(x + 6 + 15, 1, COLOR_PANEL_HEADER, "Name");
    DrawTextXY(x, TEXT_HEIGHT - 6, COLOR_PANEL_BORDER, "╟──────────────┴───────────────╢");
    DrawTextXY(x, TEXT_HEIGHT - 5, COLOR_PANEL_BORDER, "║                              ║");
    DrawTextXY(x, TEXT_HEIGHT - 4, COLOR_PANEL_BORDER, "║                              ║");
    DrawTextXY(x, TEXT_HEIGHT - 3, COLOR_PANEL_BORDER, "╚══════════════════════════════╝");
}
#endif

void PanelDrawTitle(uint8_t color) {
    DrawTextXY(panel_x, 0, COLOR_PANEL_BORDER, "╔══════════════════════════════╗");

    char buf[PANEL_SHORT_PATH + 3];
    buf[0] = ' ';
    strcpy(buf + 1, panel_a.path + panel_a.short_path_skip);
    strcat(buf, " ");

    DrawTextXY(panel_x + (uint8_t)(PANEL_WIDTH - 2 - panel_a.short_path_size) / 2, 0, color, buf);
}

#ifndef FULL_COLOR_MODE
static const uint8_t COLOR_PANEL_FOOTER = COLOR_PANEL_BORDER;
#endif

void PanelDrawFreeSpace(void) {
#ifdef FULL_COLOR_MODE
    static char text[] = "           kbytes free on drive A ";
    static const uint8_t offset = PANEL_OX;
#else
    static char text[] = "           kbytes free on drive A ║";
    static const uint8_t offset = 0;
#endif
    Uint32ToString(text, panel_a.free_kb, 10);  // Применено вместо Uint16To для уменьшения размера программы
#ifndef FULL_COLOR_MODE
    text[3] = '║';
#endif
    text[10] = ' ';
    text[32] = 'A' + PanelGetDrive();
    DrawTextXY(panel_x + offset, TEXT_HEIGHT - 4, COLOR_PANEL_FOOTER, text + 3 + offset);
}

static void ReplaceForbiddenChars(char *text, uint8_t size) {
    do {
        if ((uint8_t)*text < 32)
            *text = '?';
        text++;
        size--;
    } while (size != 0);
}

void PanelDrawFileInfo(void) {
#ifdef FULL_COLOR_MODE
    static char text[] = "                            ";
    static const uint8_t offset = 0;
#else
    static char text[] = "║                              ║";
    static const uint8_t offset = PANEL_OX;
#endif
    memset(text + offset, ' ', sizeof(text) - offset * 2 - 1);
    if (panel_a.count != 0) {
        struct FileInfo *file_pointer = PanelGetCursor();
        CpmConvertFromName83(panel_a.selected_name, file_pointer->name83);
        ReplaceForbiddenChars(panel_a.selected_name, strlen(panel_a.selected_name));
        memcpy(text + offset, panel_a.selected_name, strlen(panel_a.selected_name));
        if (file_pointer->attrib & ATTRIB_DIR_ALL) {
            memcpy(&text[offset + 12 + 7], (file_pointer->attrib & ATTRIB_DIR_UP) ? "►UP--DIR◄" : "►SUB-DIR◄", 9);
        } else {
            Uint32ToString(text + (2 + 12), (uint32_t)file_pointer->blocks_128 * 128, 10);
            memcpy(&text[offset + 12 + 10], " bytes", 6);
        }
    }
    DrawTextXY(panel_x + (PANEL_OX - offset), TEXT_HEIGHT - 5, COLOR_PANEL_FOOTER, text);
}

#ifdef FULL_COLOR_MODE
static uint8_t panel_cursor_color;
#endif

static void DrawPanelFileInt(size_t file_index, char *panel_cursor_text) {
    if (file_index >= panel_a.count) {
        memcpy(panel_cursor_text, "            ", 12);
        return;
    }

    struct FileInfo *file_info = panel_a.files + file_index;

    memcpy(panel_cursor_text, file_info->name83, 8);
    memcpy(panel_cursor_text + 9, file_info->name83 + 8, 3);

    ReplaceForbiddenChars(panel_cursor_text, 12);

    if ((file_info->attrib & ATTRIB_DIR_ALL) == 0)
        ToLowerCase(panel_cursor_text);

#ifdef FULL_COLOR_MODE
    panel_cursor_color = (file_info->attrib & ATTRIB_DIR_ALL) ? COLOR_PANEL_DIR : COLOR_PANEL_FILE;
#endif
}

void PanelDrawCursor(uint8_t color, uint8_t x, uint8_t y) {
    if (panel_a.count == 0 || x >= PANEL_COLUMNS_COUNT || y >= PANEL_ROWS_COUNT)
        return;

    const uint16_t index = panel_a.offset + y + x * PANEL_ROWS_COUNT;
    if (index >= panel_a.count)
        return;

    static char panel_cursor_text[] = "            ";
    DrawPanelFileInt(index, panel_cursor_text);
#ifdef FULL_COLOR_MODE
    if (color != COLOR_PANEL_CURSOR)
        color = panel_cursor_color;
#endif
    DrawTextXY(PANEL_OX + x * PANEL_COLUMN_WIDTH + panel_x, PANEL_OY + y, color, panel_cursor_text);
}

static void PanelHideCursor(void) {
    if (panel_a.cursor_y_now != NO_CURSOR) {
        PanelDrawCursor(COLOR_PANEL_BORDER, panel_a.cursor_x_now, panel_a.cursor_y_now);
        panel_a.cursor_y_now = NO_CURSOR;
    }
}

void PanelDrawFiles(bool active) {
    PanelDrawTitle(active ? COLOR_PANEL_TITLE_ACTIVE : COLOR_PANEL_TITLE);

#ifndef FULL_COLOR_MODE
    DrawTextXY(panel_x, 1, COLOR_PANEL_BORDER, "║     Name     │     Name      ║");
#endif

    PanelGetCursorIndex();
    size_t file_index_0 = panel_a.offset;
    uint8_t y = PANEL_OY;
    do {
#ifdef FULL_COLOR_MODE
        static char file_name[] = "            ";
#else
        static char panel_line[] = "║              │               ║";
#endif
        uint8_t column = 0;
        uint8_t x = PANEL_OX;
        uint16_t file_index = file_index_0;
        do {
#ifdef FULL_COLOR_MODE
            DrawPanelFileInt(file_index, file_name);
            DrawTextXY(panel_x + x, y, panel_cursor_color, file_name);
#else
            DrawPanelFileInt(file_index, panel_line + x);
#endif
            column++;
            x += PANEL_COLUMN_WIDTH;
            file_index += PANEL_ROWS_COUNT;
        } while (column < PANEL_COLUMNS_COUNT);
#ifndef FULL_COLOR_MODE
        DrawTextXY(panel_x, y, COLOR_PANEL_BORDER, panel_line);
#endif
        file_index_0++;
        y++;
    } while (y < PANEL_OY + PANEL_ROWS_COUNT);

#ifndef FULL_COLOR_MODE
    DrawTextXY(panel_x, TEXT_HEIGHT - 6, COLOR_PANEL_BORDER, "╟──────────────┴───────────────╢");
#endif

    PanelDrawFileInfo();
    PanelDrawFreeSpace();

#ifndef FULL_COLOR_MODE
    DrawTextXY(panel_x, TEXT_HEIGHT - 3, COLOR_PANEL_BORDER, "╚══════════════════════════════╝");
#endif

    panel_a.offset_now = panel_a.offset;
    panel_a.cursor_y_now = NO_CURSOR;
}

void PanelRedrawCursor(bool show) {
    if (panel_a.offset_now != panel_a.offset)
        PanelDrawFiles(true);
    if (!show) {
        PanelHideCursor();
        return;
    }
    if (panel_a.cursor_y_now != panel_a.cursor_y || panel_a.cursor_x_now != panel_a.cursor_x) {
        PanelHideCursor();
        PanelDrawCursor(COLOR_PANEL_CURSOR, panel_a.cursor_x, panel_a.cursor_y);
        panel_a.cursor_x_now = panel_a.cursor_x;
        panel_a.cursor_y_now = panel_a.cursor_y;
        PanelDrawFileInfo();
    }
}

void PanelMoveCursorLeft(void) {
    if (panel_a.cursor_x != 0) {
        panel_a.cursor_x--;
    } else if (panel_a.offset != 0) {
        if (panel_a.offset > PANEL_ROWS_COUNT)
            panel_a.offset -= PANEL_ROWS_COUNT;
        else
            panel_a.offset = 0;
    } else if (panel_a.cursor_y != 0) {
        panel_a.cursor_y = 0;
    }
}

void PanelMoveCursorRight(void) {
    // Переместится вправо нельзя
    uint16_t w = PanelGetCursorIndex();
    if (w + PANEL_ROWS_COUNT >= panel_a.count) {  //! перепутаны > и >=
        // Это последний файл
        if (w + 1 >= panel_a.count)
            return;
        // Вычисляем положение по Y
        panel_a.cursor_y = panel_a.count - (panel_a.offset + panel_a.cursor_x * PANEL_ROWS_COUNT + 1);
        // Корректируем курсор
        if (panel_a.cursor_y > PANEL_ROWS_COUNT - 1) {
            panel_a.cursor_y -= PANEL_ROWS_COUNT;
            if (panel_a.cursor_x == 1) {
                panel_a.offset += PANEL_ROWS_COUNT;
            } else {
                panel_a.cursor_x++;
            }
        }
    } else if (panel_a.cursor_x == 1) {
        panel_a.offset += PANEL_ROWS_COUNT;
    } else {
        panel_a.cursor_x++;
    }
}

void PanelMoveCursorUp(void) {
    if (panel_a.cursor_y != 0) {
        panel_a.cursor_y--;
    } else if (panel_a.cursor_x != 0) {
        panel_a.cursor_x--;
        panel_a.cursor_y = PANEL_ROWS_COUNT - 1;
    } else if (panel_a.offset != 0) {
        panel_a.offset--;
    }
}

void PanelMoveCursorDown(void) {
    if (PanelGetCursorIndex() + 1 >= panel_a.count)
        return;
    if (panel_a.cursor_y < PANEL_ROWS_COUNT - 1) {
        panel_a.cursor_y++;
    } else if (panel_a.cursor_x == 0) {
        panel_a.cursor_y = 0;
        panel_a.cursor_x++;
    } else {
        panel_a.offset++;
    }
}

void PanelSwap(void) {
    panel_x = PANEL_WIDTH - panel_x;
    memswap(&panel_a, &panel_b, sizeof(panel_a));
}
