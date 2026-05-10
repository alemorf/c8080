/* Переписано на C8080 и Linux
 * в 2026 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * Проект CRISS CP/M - 8 bit DIY computer
 * http://www.criss.fun http://criss.radio.ru
 *
 * SCHOOL - учебная программа для обучения устному счёту.
 * Была написана изначально для Robotron 1715 примерно в 1987
 * году, удивительным образом нашлась дискета с исходником
 * и прочиталась, за что была сохранена и портирована под CRISS.
 *
 * Запуск: lifer [макс.число]
 * Без параметра все примеры идут в пределах 100, с параметром
 * можно задавать и другие диапазоны.
 *
 * SCHOOL является частью цикла учебных программ, которые
 * разрабатывались энтузиастами в одном из Кировских УПК
 * (были т.н. "учебно-производственные комбинаты" в СССР),
 * ещё были программы по русскому языку, работе на клавиатур
 * и другие.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#ifdef __linux__
#include <termios.h>
#include <poll.h>
#else
#include <c8080/console.h>
#include <cpm.h>
#endif

static unsigned x, y, max, x1, y1, k, z, d, sh, v;
static unsigned char mark;
static char l, t, o, ot;
static char st[40];

#ifdef __linux__

static void ClearConsole(void) {
    printf("\x1B[H\x1B[2J");
}

static void MoveCursor(unsigned x, unsigned y) {
    printf("\x1B[%u;%uH", x + 1, y + 1);
}

static bool KeyPressed(void) {
    rand();
    struct pollfd fds;
    fds.fd = 0; /* STDIN */
    fds.events = POLLIN;
    if (poll(&fds, 1, 0) == 1)
        return true;
    usleep(300);
    return false;
}

#else

static bool KeyPressed(void) {
    rand();
    return CpmBiosConSt();
}

#endif

static unsigned rnd(unsigned max) {
    return (unsigned)rand() % (max + 1);
}

static unsigned unsigned_sqrt(unsigned x) {
    unsigned result = 1;
    for (;;) {
        if (result * result > x)
            return result - 1;
        result++;
    }
}

static void prsc(void) {
    /* Печать экрана не реализована */
}

static void xod(char o) {
    switch (o) {
        case '+':
            x = rnd(max);
            y = rnd(max - x);
            k = x + y;
            break;
        case '-':
            x = rnd(max);
            y = rnd(x);
            k = x - y;
            break;
        case '*':
            x = rnd(unsigned_sqrt(max) - 1) + 1;
            y = rnd(max / x);
            k = x * y;
            break;
        case ':':
            y = rnd(unsigned_sqrt(max) - 1) + 1;
            k = rnd(max / y);
            x = k * y;
            break;
    }
}

static void dop(char o, unsigned d) {
    if (o == '+') {
        x1 = rnd(d);
        y1 = d - x1;
    } else {
        x1 = rnd(max - d) + d;
        y1 = x1 - d;
    }
}

static bool wwod(void) {
    ClearConsole();
    sh = 0;
    v = 0;
    printf(
        "╔════════════════════════════════════════════════════════════════════════════╗\n"
        "║                   ┌───────────────────────────────────┐                    ║\n"
        "║                   │  Решетников И.С. и Роботрон 1715  │                    ║\n"
        "║                   │           представляют            │                    ║\n"
        "║                   └───────────────────────────────────┘                    ║\n"
        "║  ┌──────────────────────────────────────────────────────────────────────┐  ║\n"
        "║  │          Учебная программа для проверки знания математики            │  ║\n"
        "║  │                     учащихся начальной школы.                        │  ║\n"
        "║  │                                                                      │  ║\n"
        "║  │    Сейчас компьютер проверит твои знания по математике. В зависимос- │  ║\n"
        "║  │ ти от нажатой тобой клавиши (1..5) тебе будет предложено 12 примеров │  ║\n"
        "║  │ соответственно на проверку знаний таблицы умножения, на сложение-вы- │  ║\n"
        "║  │ читание, на умножение, на деление или на выполнение сложных действий │  ║\n"
        "║  │ После того,  как появится очередной пример,  подумай и набери ответ, │  ║\n"
        "║  │ используя клавиши с цифрами.  Чтобы исправить последний неверно вве- │  ║\n"
        "║  │ денный символ, используй клавишу <--, а чтоб повторить ввод целиком- │  ║\n"
        "║  │ клавишу !<--. Заканчивай ввод клавишей <ЕТ> или пробелом. После того │  ║\n"
        "║  │ как ты решишь  все  12 примеров,  машина выставит тебе оценку. Время │  ║\n"
        "║  │ при решении не ограничено, но за каждую просроченную минуту добавля- │  ║\n"
        "║  │ ется минута штрафа. За 5 минут штрафа оценка снижается на балл.      │  ║\n"
        "║  │    Для выхода из программы нажми клавишу <ESC>.                      │  ║\n"
        "║  │                                                 Желаю успеха !!!     │  ║\n"
        "║  └──────────────────────────────────────────────────────────────────────┘  ║\n"
        "╚════════════════════════════════════════════════════════════════════════════╝");

    do {
        while (!KeyPressed()) { /* Инициализация генератора псевдослучайных чисел */
        }

        o = getchar();
        switch (o) {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
                ClearConsole();
                printf(
                    "╔════════════════════════════════════════════════════════════════════════════╗\n"
                    "║                   ┌───────────────────────────────────┐                    ║\n"
                    "║                   │  Решетников И.С. и Роботрон 1715  │                    ║\n"
                    "║       Всего       │          представляют.            │       Верно        ║\n"
                    "║                   └───────────────────────────────────┘                    ║\n"
                    "║                          ┌─────────────────────┐                           ║\n"
                    "║                   Время  │                     │  Штраф                    ║\n"
                    "║      примеров            └─────────────────────┘             решено        ║\n"
                    "║                   ┌───────────────────────────────────┐                    ║\n"
                    "║                   │             Примеры на            │                    ║\n"
                    "║                   │                                   │                    ║\n"
                    "║                   └───────────────────────────────────┘                    ║\n"
                    "║  ┌──────────────────────────────────────────────────────────────────────┐  ║\n");
                for (z = 0; z < 9; z++)
                    printf("║  │                                                                      │  ║\n");
                printf(
                    "║  └──────────────────────────────────────────────────────────────────────┘  ║\n"
                    "╚════════════════════════════════════════════════════════════════════════════╝");
                MoveCursor(10, 27);
                switch (o) {
                    case '1':
                        printf("знание таблицы умножения");
                        break;
                    case '2':
                        printf("        сложение");
                        break;
                    case '3':
                        printf("        умножение");
                        break;
                    case '4':
                        printf("         деление");
                        break;
                    case '5':
                        printf("   смешанные действия");
                        break;
                }
                break;
            case 'P':
            case 'p':
                prsc();
                break;
            case 27:
                return false;
        }
    } while (o < '1' || o > '5');
    return true;
}

static void play(void) {
    if (rnd(1))
        t = '+';
    else
        t = '-';
    xod(t);
    d = rnd(7) + 1;
    switch (d) {
        case 1:
        case 8:
            snprintf(st, sizeof(st), "%u%c%u", x, t, y);
            break;
        case 7:
        case 3:
        case 4:
            if (rnd(1))
                l = '+';
            else
                l = '-';
            dop(l, x);
            if (d == 3)
                snprintf(st, sizeof(st), "%u%c%u%c%u", x1, l, y1, t, y);
            else
                snprintf(st, sizeof(st), "(%u%c%u)%c%u", x1, l, y1, t, y);
            break;
        case 2:
        case 6:
        case 5:
            if (rnd(1))
                l = '+';
            else
                l = '-';
            dop(l, y);
            snprintf(st, sizeof(st), "%u%c(%u%c%u)", x, t, x1, l, y1);
    }
}

static void igra(void) {
    unsigned char j1, i, j, p, m, e;
    unsigned r;
    unsigned t;
    char ot1[6], ot2[6];

    r = 0;
    ot1[r] = 0;
    t = UINT_MAX;
    m = 0;
    e = 0;

    MoveCursor(5, 10);
    printf("%u", z);

    i = z % 4;
    if (i == 1)
        i = 14;
    if (i == 2)
        i = 16;
    if (i == 3)
        i = 18;
    if (i == 0)
        i = 20;
    j = ((z - 1) / 4) * 22 + 8;

    MoveCursor(i, j);
    printf("%s", st);

    j += strlen(st);
    j1 = j;
    do {
        do {
            t++;
            if (t >= 3000) { /* Нужно подобрать число, что бы время шло точно */
                t = 0;
                e++;
                if (e == 60) {
                    e = 0;
                    m++;
                    sh++;
                }
                MoveCursor(6, 29);
                printf("%3u:%2u    <>    %u", m, e, sh);
                MoveCursor(i, j1);
            }
        } while (!KeyPressed() && m < 10);

        if (m >= 10)
            break;

        ot = getchar();
        switch (ot) {
            case 'P':
            case 'P' % 0x1F:
                prsc();
                break;
            case 8:
            case 127:
                if (r > 0) {
                    r--;
                    st[r] = 0;
                    j1 = j + r;
                    MoveCursor(i, j1);
                    putchar(' ');
                    MoveCursor(i, j1);
                }
                break;
            case 'X' & 0x1F:
                r = 0;
                j1 = j;
                ot1[r] = 0;
                MoveCursor(i, j1);
                printf("      ");
                MoveCursor(i, j1);
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if (r < sizeof(ot1) - 1) {
                    ot1[r] = ot;
                    ot1[r + 1] = 0;
                    MoveCursor(i, j1);
                    putchar(ot);
                    r++;
                    j1 = j + r;
                }
                break;
            case 27:
                return;
        }
    } while (ot != 13 && ot != 10);

    sprintf(ot2, "%u", k);

    if (r == 0)
        snprintf(st, sizeof(st), "Ответ : %s.", ot2);
    else if (0 == strcmp(ot1, ot2))
        snprintf(st, sizeof(st), "Молодец, верно.");
    else
        snprintf(st, sizeof(st), "Неправильно !!!");
    if (0 == strcmp(ot1, ot2)) {
        v++;
        MoveCursor(5, 65);
        printf("%u", v);
    } else {
        MoveCursor(i, ((z - 1) / 4) * 22 + 7);
        printf("*");
    }
    MoveCursor(22, 31);
    printf(" %s ", st);
    sleep(2);
    MoveCursor(22, 31);
    printf("─────────────────────");
}

int main(int argc, char **argv) {
#ifdef __linux__
    // Отключаем эхо (ECHO), канонический режим (ICANON) и буферизацию
    struct termios oldt;
    tcgetattr(0, &oldt);
    struct termios newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(0, TCSANOW, &newt);
    setvbuf(stdout, NULL, _IONBF, 0);
#endif

    max = 100;

    if (argc > 1) {
        errno = 0;
        unsigned long arg = strtoul(argv[1], 0, 0);
        if (errno != 0 || arg > UINT_MAX)
            return 1;
        if (arg > 100)
            max = arg;
    }

    while (wwod()) {
        for (z = 1; z <= 12; z++) {
            switch (o) {
                case '1':
                    x = rnd(10);
                    y = rnd(10);
                    k = x * y;
                    snprintf(st, sizeof(st), "%u*%u", x, y);
                    break;
                case '2':
                    play();
                    break;
                case '3':
                case '4':
                    if (o == 3)
                        l = '*';
                    else
                        l = ':';
                    xod(l);
                    snprintf(st, sizeof(st), "%u%c%u", x, l, y);
                    break;
                case '5':
                    switch (rnd(5)) {
                        case 0:
                        case 5:
                            t = '+';
                            break;
                        case 3:
                        case 2:
                            t = ':';
                            break;
                        case 1:
                        case 4:
                            t = '*';
                            break;
                    }
                    if (t == '+') {
                        play();
                    } else {
                        xod(t);
                        if (rnd(1))
                            l = '+';
                        else
                            l = '-';
                        switch (rnd(7)) {
                            case 0:
                            case 7:
                                dop(l, x);
                                snprintf(st, sizeof(st), "(%u%c%u)%c%u", x1, l, y1, t, y);
                                break;
                            case 1:
                            case 6:
                                dop(l, y);
                                snprintf(st, sizeof(st), "%u%c(%u%c%u)", x, t, x1, l, y1);
                                break;
                            case 2:
                            case 5:
                                if (rnd(1)) {
                                    x1 = rnd(max - k) + k;
                                    k = x1 - k;
                                    snprintf(st, sizeof(st), "%u-%u%c%u", x1, x, t, y);
                                } else {
                                    x1 = rnd(max - k);
                                    k = k + x1;
                                    snprintf(st, sizeof(st), "%u%c%u+%u", x, t, y, x1);
                                }
                                break;
                            case 3:
                            case 4:
                                if (rnd(1)) {
                                    x1 = rnd(k);
                                    k = k - x1;
                                    snprintf(st, sizeof(st), "%u%c%u-%u", x, t, y, x1);
                                } else {
                                    x1 = rnd(max - k);
                                    k = k + x1;
                                    snprintf(st, sizeof(st), "%u+%u%c%u", x1, x, t, y);
                                }
                        }
                    }
            }
            strcat(st, "=");
            igra();
            if (ot == 27)
                goto lab1;
        }
        ClearConsole();
        mark = v / 3 + 1 - sh / 5;
        if (mark < 2)
            mark = 2;
        MoveCursor(11, 29);
        printf("Твоя оценка  : %u", mark);
        MoveCursor(13, 24);
        switch (mark) {
            case 5:
                printf("        Отлично !!!");
                break;
            case 4:
                printf("       Хорошо, но...");
                break;
            case 3:
                printf("Удовлетворительно, поработай еще.");
                break;
            case 2:
                printf("       Очень плохо...");
        }

        MoveCursor(23, 1);
        printf("Нажми любую клавишу...");
        getchar();
lab1:;
    }

    ClearConsole();

#ifdef __linux__
    tcsetattr(0, TCSANOW, &oldt);
#endif

    return 0;
}
