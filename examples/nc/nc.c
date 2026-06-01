/*
 * NC shell for CP/M
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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

#include "nc.h"
#include "arch.h"
#include <c8080/hal.h>
#include <c8080/keys.h>
#include <c8080/console.h>
#include <c8080/uint32tostring.h>
#include <c8080/touppercase.h>
#include <c8080/getunusedram.h>
#include <string.h>
#include <stdio.h>
#include <cpm.h>
#include "windows.h"
#include "panel.h"
#include "dir.h"
#include "config.h"

struct state {
    uint8_t state;
    uint8_t drive_user_b;
    uint16_t a_offset;
    uint8_t a_cursor_x;
    uint8_t a_cursor_y;
    uint16_t b_offset;
    uint8_t b_cursor_x;
    uint8_t b_cursor_y;
};

#ifdef FEATURE_HAL_SAVE_SCREEN
static struct SavedScreen saved_screen;
#endif

static const uint8_t STATE_TAB = 1 << 4;
static const uint8_t STATE_HIDDEN = 1 << 5;

static const uint16_t STACK_SIZE = 1024;

static uint8_t copy_buffer_size;  // Размер panel.buffer в 128 байтных блоках
static bool panels_hidden;

static const char help_64_no_fn[] =
#ifdef KEY_TAB_ONLY_IF_EMPTY_COMMAND_LINE
    " ;\0Tab  \0"
#else
    "  \0Tab  \0"
#endif
    " 1\0Left \0"
    " 2\0Right\0"
    " 3\0View \0"
    " 4\0Edit \0"
    " 5\0Copy \0"
    " 6\0Ren  \0"
    " 7\0Mkdir\0"
    " 8\0Del  \0"
    "\0";

static void SaveState(void) {
#ifdef NC_STATE_IN_MEMORY
    static struct state *const s = (struct state *)(NC_STATE_IN_MEMORY);
    // TODO: assert(sizeof(struct state) <= NC_STATE_IN_MEMORY_SIZE);
#else
    static struct state *const s = (void *)DEFAULT_DMA;
    memset(s, 0, CPM_128_BLOCK);
#endif
    s->drive_user_b = panel_b.drive_user;
    s->state = (panel_x ? STATE_TAB : 0);
    if (panels_hidden)
        s->state |= STATE_HIDDEN;
    s->a_cursor_x = panel_a.cursor_x;
    s->a_cursor_y = panel_a.cursor_y;
    s->a_offset = panel_a.offset;
    s->b_cursor_x = panel_b.cursor_x;
    s->b_cursor_y = panel_b.cursor_y;
    s->b_offset = panel_b.offset;
#ifndef NC_STATE_IN_MEMORY
    struct FCB file;
    memcpy(&file.drive, "\x01NC      CFG", 12);
    CpmSetUser(0);
    if (CpmOpen(&file) != 0xFF || CpmCreate(&file) != 0xFF) {
        CpmWrite(&file);
        CpmClose(&file);
    }
#endif
}

static void LoadState(void) {
#ifdef NC_STATE_IN_MEMORY
    static struct state *const s = (struct state *)(NC_STATE_IN_MEMORY);
    // TODO: assert(sizeof(struct state) <= NC_STATE_IN_MEMORY_SIZE);
#else
    static struct state *const s = (void *)DEFAULT_DMA;
    memset(s, 0, CPM_128_BLOCK);
    struct FCB file;
    memcpy(&file.drive, "\x01NC      CFG", 12);
    CpmSetUser(0);
    if (CpmOpen(&file) != 0xFF) {
        CpmRead(&file);
        CpmClose(&file);
    }
#endif
    panel_a.cursor_x = s->a_cursor_x;
    panel_a.cursor_y = s->a_cursor_y;
    panel_a.offset = s->a_offset;
    panel_b.cursor_x = s->b_cursor_x;
    panel_b.cursor_y = s->b_cursor_y;
    panel_b.offset = s->b_offset;
    panels_hidden = (s->state & STATE_HIDDEN) != 0;

    if (s->state & STATE_TAB) {
        if (panel_x != 0)
            panel_x = 0;
        else
            panel_x = TEXT_WIDTH / 2;
    }

    panel_b.drive_user = s->drive_user_b;
    if ((panel_b.drive_user & 0x0F) >= DRIVE_COUNT)
        panel_b.drive_user = panel_a.drive_user;

    // Текущий диск и папку в активную панель
    //    panel_a.drive_user = CpmGetDrive() | (CpmGetUser() << 4);
    //    panel_b.drive_user = panel_a.drive_user;
    //    TODO: CRC
}

static void NcDrawCommandLine(void) {
    DrawInput(panel_a.short_path_size + 1, TEXT_HEIGHT - 2, TEXT_WIDTH - 1 - panel_a.short_path_size,
              COLOR_COMMAND_LINE);
}

static void NcDrawCommandLinePrefix(void) {
    DrawTextXY(0, TEXT_HEIGHT - 2, COLOR_COMMAND_LINE, panel_a.path + panel_a.short_path_skip);
    DrawTextXY(panel_a.short_path_size, TEXT_HEIGHT - 2, COLOR_COMMAND_LINE, ">");
}

static void NcDrawActivePanelTitleAndCommandLine(void) {
    NcDrawCommandLinePrefix();
    NcDrawCommandLine();
    PanelDrawTitle(COLOR_PANEL_TITLE_ACTIVE);
    PanelRedrawCursor(true);
}

static void NcDrawHelp(void) {
    const char *text = help_64_no_fn;
    uint8_t x = 0;
    uint8_t color = COLOR_COMMAND_LINE;
    do {
        DrawTextXY(x, TEXT_HEIGHT - 1, color, text);
        color ^= COLOR_COMMAND_LINE ^ COLOR_HELP_LINE;
        const uint8_t text_size = strlen(text);
        x += text_size;
        text += text_size + 1;
    } while (*text != 0);
}

void NcDrawScreen(void) {
    HideCursor();
    MoveCursor(0, 0);  // Что бы уменьшить вероятность прокрутки экрана из-за ошибок CP/M выводимых на экран

#ifdef FEATURE_HAL_SAVE_SCREEN
    if (panels_hidden)
        RestoreScreen(&saved_screen);
#endif

    NcDrawHelp();

    if (panels_hidden) {
        if (panel_a.total_kb == 0)
            PanelReload();  // Для вычисления пути, который будет отображен в ком. строке
        NcDrawCommandLinePrefix();
        NcDrawCommandLine();
        return;
    }

#ifdef FULL_COLOR_MODE
    PanelDrawBorder(0);
    PanelDrawBorder(PANEL_WIDTH);
#endif

    if (panel_a.total_kb == 0)
        PanelReload();

    PanelDrawFiles(true);

    NcDrawCommandLinePrefix();
    NcDrawCommandLine();

    PanelSwap();
    if (panel_a.total_kb == 0)
        PanelReloadOrCopy();
    PanelDrawFiles(false);
    PanelSwap();

    PanelRedrawCursor(true);
}

static void NcBeforeExit(void) {
    SaveState();

#ifdef FEATURE_HAL_SAVE_SCREEN
    // Восстанавление экрана
    RestoreScreen(&saved_screen);
#else
    // Цвет по умолчанию для консоли
    SetConsoleColor(COLOR_COMMAND_LINE);
    if (!panels_hidden) {
        // Если отображаются панели, то перед выходом очищается экран
        // Курсор вверх экрана
        ClearConsole();
    } else {
        // Если не отображаются панели, то перед выходом очищаются 2 нижгние строки
        char spaces[TEXT_WIDTH + 1];
        MakeString(spaces, ' ', TEXT_WIDTH);
        spaces[TEXT_WIDTH] = 0;
        DrawTextXY(0, TEXT_HEIGHT - 2, COLOR_COMMAND_LINE, spaces);
        spaces[TEXT_WIDTH - 1] = 0;
        DrawTextXY(0, TEXT_HEIGHT - 1, COLOR_COMMAND_LINE, spaces);
        // Курсор вниз экрана
        MoveCursor(TEXT_HEIGHT - 2, 0);
    }
    ShowCursor();
#endif
}

static void NcCommand(const char *text) {
    NcBeforeExit();
    WriteConsole(panel_a.path);
    WriteConsole(">");
    WriteConsole(text);
    uint8_t d = panel_a.drive_user;
    // TODO: Если указываем диск, то всегда будет корневая папка. А потом нужно будет вставить парсер пути.
    if (text[0] != 0 && text[1] == ':') {
        common_folder = 0xF0 + (d >> 4);
        d &= 0x0F;
    }
    CpmCommand(d, text);
}

static void NcExecute(void) {
    // Выход, если папка пуста или файл не выбран
    if (panel_a.count == 0)
        return;

    // Это папка
    const uint8_t attrib = PanelGetCursor()->attrib;
    if (attrib & ATTRIB_DIR_ALL) {
        if (attrib & ATTRIB_DIR_UP)
            panel_reload_select_dir = PanelGetDirIndex() << ATTRIB_DIR_SHIFT;
        panel_a.drive_user = PanelGetDrive() | ((attrib << (4 - ATTRIB_DIR_SHIFT)) & 0xF0);
        panel_a.cursor_x = 0;
        panel_a.cursor_y = 0;
        panel_a.offset = 0;
        PanelReload();

        PanelDrawFiles(true);
        NcDrawActivePanelTitleAndCommandLine();
        return;
    }

    char text[sizeof(panel_a.selected_name)];
    strcpy(text, panel_a.selected_name);

    char *ext = strchr(text, '.');
    if (ext && ext[1] == 'C' && ext[2] == 'O' && ext[3] == 'M') {
        ext[0] = 0;
        NcCommand(text);
        return;
    }

    // TODO: Запуск TXT, BAS файлов
}

static void NcSelectDrive(uint8_t offset) {
    const bool swap = panel_x != offset;
    const uint8_t result = SelectDriveWindow((swap ? panel_b.drive_user : panel_a.drive_user) & 0x0F, offset);
    if (result == 0xFF)
        return;
    if (swap)
        PanelSwap();
    panel_a.drive_user = result;
    panel_a.cursor_x = 0;
    panel_a.cursor_y = 0;
    panel_a.offset = 0;
    PanelReload();
    if (swap)
        PanelSwap();
}

static void NcDriveChanged(uint8_t drive_user) {
    if (panel_a.drive_user == drive_user || drive_user == 0xFF)
        PanelReload();
    if (panel_b.drive_user == drive_user || drive_user == 0xFF) {
        PanelSwap();
        PanelReloadOrCopy();
        PanelSwap();
    }
}

static void NcCopyMoveRename(bool rename) {
    // Выход, если папка пуста или файл не выбран
    if (panel_a.count == 0 || (PanelGetCursor()->attrib & ATTRIB_DIR_UP) != 0)
        return;

    // Исходный файл
    struct FileInfo *source_file = PanelGetCursor();
    struct FCB source;
    source.drive = PanelGetDrive() + 1;
    memcpy(source.name83, source_file->name83, sizeof(source_file->name83));

    // Окно
    const char *title = rename ? " Rename or move " : " Copy ";
    const uint8_t y = DrawWindow(WINDOW_X_CENTER, 8, title);  // Original: Copy | Rename
    DrawWindowText(y, "File");                                // Original: Copy | Rename or move
    DrawWindowText(y + 1, panel_a.selected_name);             // TODO: Можно добавить путь
    DrawWindowText(y + 2, "to");
    DrawButtons(y + 5, 0, rename ? "Rename\0Cancel\0" : "Copy\0Cancel\0");
    // Original: Copy\0Cancel | Rename/Move\0Cancel
    input[0] = 0;
    if (!RunInput(y + 3))
        return;

    ToUpperCase(input);

    struct FCB dest;
    uint8_t dest_drive_user = DirParsePathName(&dest, input, panel_b.drive_user, panel_a.drive_user);
    if (dest_drive_user == 0xFF) {
        ErrorWindow("Incorrect file name");
        return;
    }

    // Если имя файла не указано, то используется имя исходного файла
    if (dest.name83[0] == ' ')
        memcpy(dest.name83, source.name83, sizeof(dest.name83));

    // Для вывода на экран
    DirMakePathName(input, sizeof(input), dest_drive_user, &dest);

    // Есть ли файл в папке назначения?
    const uint8_t dest_user = dest_drive_user >> 4;
    CpmSetUser(dest_user);
    if (CpmSearchFirst(dest.drive, dest.name83, false) != NULL) {
        ErrorWindow("The file already exists");  // Original
        // В оригинале предлагается заменить файл
        // Если это сделаем, то нужно проверить, что мы не переносим файл сам в себя
        return;
    }

    // Копирование атрибут, всех 16 бит.
    CpmSetUser(PanelGetDirIndex());
    struct FCB *source_info = CpmSearchFirst(source.drive, source.name83, false);
    if (source_info == NULL) {
        ErrorWindow("Incorrect file name");
        return;
    }
    CpmSetAttrib(dest.name83, CpmGetAttrib(source_info->name83));

    // Переименование файла.
    // Средствами системы нельзя переносить файл между папками, т.к. нельзя изменять пользователя.
    // А может и можно.
    if (rename && panel_a.drive_user == dest_drive_user) {
        memcpy(source.rename, &dest, sizeof(dest.drive) + sizeof(dest.name83));
        CpmRename(&source);  // TODO: Error
        NcDriveChanged(dest_drive_user);
        return;
    }

    if (source_file->attrib & ATTRIB_DIR_ALL) {
        if (!rename || source.drive != dest.drive) {
            ErrorWindow("Can't copy the folder");
            return;
        }
    }

    if (CpmOpen(&source) == 0xFF) {
        ErrorWindow("Can't open the file");  // Original
        return;
    }

    CpmSetUser(dest_user);

    if (CpmCreate(&dest) == 0xFF) {
        ErrorWindow("Can't create the file");  // Original
        rename = false;                        // Не удалять исходный файл при ошибке
    } else {
        uint8_t y = DrawWindow(WINDOW_X_CENTER, 9, title);
        DrawWindowTextCenter(y, "File");
        // Original: Copying the file or directory | Renaming or Moving the file or directory
        DrawWindowTextCenter(y + 1, panel_a.selected_name);
        DrawWindowTextCenter(y + 2, "to");
        DrawWindowTextCenter(y + 3, input);
        DrawProgress(y + 4);
        DrawButtons(y + 6, 0, "Cancel\0");  // В оригинале нет кнопки

        panel_b.count = 0;  // Список файлов будет уничтожен

        uint16_t i = 0;
        for (;;) {
            CpmSetUser(PanelGetDirIndex());

            uint8_t *buffer = (void *)panel_b.files;
            uint8_t count = 0;
            uint8_t result;
            do {
                CpmSetDma(buffer);
                result = CpmRead(&source);
                if (result != 0) {
                    if (result == CPM_READ_EOF)
                        break;
                    ErrorWindow("Can't read the file");  // Original
                    rename = false;  // Не удалять исходный файл при ошибке
                    goto break2;
                }
                buffer += CPM_128_BLOCK;
                count++;
            } while (count < copy_buffer_size);

            CpmSetUser(dest_user);

            buffer = (void *)panel_b.files;
            while (count > 0) {
                CpmSetDma(buffer);
                if (CpmWrite(&dest) != 0) {
                    ErrorWindow("Can't write the file");  // Original
                    rename = false;  // Не удалять исходный файл при ошибке
                    goto break2;
                }
                buffer += CPM_128_BLOCK;
                count--;
            }

            if (result == CPM_READ_EOF)
                break;

            i += copy_buffer_size;
            DrawProgressNext(y + 4, i, source_file->blocks_128);

            if (CpmConsoleDirect(0xFF) == KEY_ESC) {
                rename = false;  // Не удалять исходный файл
                break;
            }
        }
        DrawProgressNext(y + 4, 1, 1);

    break2:
        CpmSetDma(DEFAULT_DMA);
        CpmSetUser(dest_user);

        if (CpmClose(&dest) == 0xFF) {
            rename = false;  // Не удалять исходный файл при ошибке
            ErrorWindow("Can't close the file");
        }
    }

    CpmSetUser(PanelGetDirIndex());

    if (CpmClose(&source) == 0xFF) {
        rename = false;  // Не удалять исходный файл при ошибке
        ErrorWindow("Can't close the file");
    }

    if (rename)
        if (CpmDelete(&source) == 0xFF)
            ErrorWindow("Can't delete the file");

    // Обновить все панели нужно в любом случае, потому что:
    // 1) panel_b.files использован как буфер
    // 2) при переносе из panel_a удален файл
    // 3) копирование могло быть в panel_a или panel_b (указано в dest_drive_user)
    NcDriveChanged(0xFF);
}

static void NcMakeDir(void) {
    if (!MakeDirWindow())
        return;

    ToUpperCase(input);

    static const char *errors[] = {"Folder creation limit", "Incorrect file name", "The file already exists",
                                   "Can't create the file", "Can't close the file"};

    const uint8_t drive_dir = DirMake(panel_a.drive_user, input);
    if (drive_dir >= DIR_MAKE_ERROR_LIMIT) {
        ErrorWindow(errors[(uint8_t)(drive_dir - DIR_MAKE_ERROR_LIMIT)]);
        return;
    }

    // TODO: Курсор на созданную папку. У нас есть для этого переменная.
    NcDriveChanged(drive_dir);
}

static void NcDelete(void) {
    // Выход, если папка пуста или файл не выбран
    if (panel_a.count == 0 || (PanelGetCursor()->attrib & ATTRIB_DIR_UP) != 0)
        return;

    // Диалог
    if (!DeleteWindow(panel_a.selected_name))
        return;

    struct FileInfo *c = PanelGetCursor();

    struct FCB f;
    f.drive = PanelGetDrive() + 1;
    memcpy(f.name83, c->name83, sizeof(f.name83));

    // Проверка, что папка не пустая
    if (c->attrib & ATTRIB_DIR_MASK) {
        CpmSetUser(GET_DIR_FROM_ATTRIB(c->attrib));
        if (CpmSearchFirst(f.drive, NULL, 0) != NULL) {
            ErrorWindow("Folder is not empty");
            return;
        }
    }

    // Удаление файла
    CpmSetUser(PanelGetDirIndex());
    CpmDelete(&f);  // TODO: Проверить ошибку

    // TODO: Выйти из удаленной папки
    // Обновить список файлов
    NcDriveChanged(panel_a.drive_user);
}

int main(int, char **) {
    const size_t panel_buffer_bytes = GetUnusedRam((void **)&panel_a.files, STACK_SIZE) / 2;
    panel_b.files = (void *)panel_a.files + panel_buffer_bytes;
    panel_files_max = panel_buffer_bytes / sizeof(panel_a.files[0]);

    // Вычисление круглого кол-ва 128 байтных буферов в panel_a.files
    uint8_t allow_loop = panel_buffer_bytes / (CPM_128_BLOCK * 2);  // Еще можно умножить на 2
    if (allow_loop < 2)
        return 1;  // Недостаточно памяти для copy_buffer_size и panel_a.files
    static const uint8_t MAX_ROUND_UINT8 = 128;
    copy_buffer_size = 1;
    do {
        copy_buffer_size *= 2;
    } while (copy_buffer_size <= allow_loop && copy_buffer_size < MAX_ROUND_UINT8 / 2);

    // CCP будет запускать A:NC вместо ожидания ввода команды пользователем
    common_dont_exec_nc = 0;

    // Что бы командер нижней строкой не закрывал полезные данные
#ifdef FEATURE_CONSOLE_GET_CURSOR_POSITION
    const uint16_t xy = GetCursorPosition();
    if (xy >= (TEXT_HEIGHT - 1) << 8) {
        CpmConsoleWrite('\n');
        MoveCursor(xy, (xy >> 8) - 1);
    }
#else
    WriteConsole("\n");
#endif

    // Сохранение консоли
#ifdef FEATURE_HAL_SAVE_SCREEN
    SaveScreen(&saved_screen);
#endif

    // Скрытие курсора
    HideCursor();

    // Текущий диск и папку в активную панель
    panel_a.drive_user = ((((common_folder & 0xF0) == 0xE0) ? common_folder : CpmGetUser()) << 4) | CpmGetDrive();
    common_folder = 0;

    // Восстановление состояния
    LoadState();

    NcDrawScreen();

    for (;;) {
        if (!CpmBiosConSt()) {
            if (!panels_hidden)
                PanelRedrawCursor(true);
            NcDrawCommandLine();
            DrawInputCursor();
        }
        const int c = ReadAndDecodeConsoleKeys();
        if (!panels_hidden) {
            if (input_size == 0) {
                switch (c) {
#ifdef KEY_TAB_ONLY_IF_EMPTY_COMMAND_LINE
                    case KEY_TAB:
                        PanelDrawTitle(COLOR_PANEL_TITLE);
                        PanelRedrawCursor(false);
                        PanelSwap();
                        NcDrawActivePanelTitleAndCommandLine();
                        continue;
#endif
                    case '1':
                        NcSelectDrive(0);
                        NcDrawScreen();
                        continue;
                    case '2':
                        NcSelectDrive(PANEL_WIDTH);
                        NcDrawScreen();
                        continue;
                    case KEY_ENTER:
                        NcExecute();
                        continue;
                    case '5':
                        NcCopyMoveRename(false);
                        NcDrawScreen();
                        continue;
                    case '6':
                        NcCopyMoveRename(true);
                        NcDrawScreen();
                        continue;
                    case '7':
                        NcMakeDir();
                        NcDrawScreen();
                        continue;
                    case '8':
                        NcDelete();
                        NcDrawScreen();
                        continue;
                }
            }
            switch (c) {
#ifndef KEY_TAB_ONLY_IF_EMPTY_COMMAND_LINE
                case KEY_TAB:
                    PanelDrawTitle(COLOR_PANEL_TITLE);
                    PanelRedrawCursor(false);
                    PanelSwap();
                    NcDrawActivePanelTitleAndCommandLine();
                    continue;
#endif
                case KEY_LEFT:
                    PanelMoveCursorLeft();
                    continue;
                case KEY_RIGHT:
                    PanelMoveCursorRight();
                    continue;
                case KEY_DOWN:
                    PanelMoveCursorDown();
                    continue;
                case KEY_UP:
                    PanelMoveCursorUp();
                    continue;
            }
        }
        if (input_size == 0) {
            switch (c) {
                case '0':
                    NcBeforeExit();
                    common_dont_exec_nc = 1;
                    return 0;
            }
        }

        switch (c) {
            case KEY_ENTER:
                NcCommand(input);
                continue;
            case 0x0A: {  // CTRL+ENTER
                const uint8_t new_size = input_size + strlen(panel_a.selected_name);
                if (new_size < sizeof(input) - 1) {
                    input_size = new_size;
                    strcat(input, panel_a.selected_name);
                    NcDrawCommandLine();
                }
                continue;
            }
            case 'I' & 0x1F:  // CTRL+I
                panel_x = PANEL_WIDTH - panel_x;
                NcDrawScreen();
                continue;
            case 'O' & 0x1F:  // CTRL+O
#ifndef FEATURE_HAL_SAVE_SCREEN
                if (!panels_hidden) {
                    SetConsoleColor(1);
                    ClearConsole();
                }
#endif
                panels_hidden = !panels_hidden;
                NcDrawScreen();
                continue;
        }
        ProcessInput(c);
    }
}
