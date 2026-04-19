# C8080 — Компилятор языка C для платформ на базе Intel 8080

Документация для разработчиков. Проект: `/Users/alexander/github/c8080`. Лицензия компилятора — **GPLv3**, стандартной библиотеки — **Apache 2.0**. Автор: Алексей Морозов.

---

## 1. Общая архитектура

C8080 — кросс-компилятор языка C (с расширениями), транслирующий в ассемблер **sjasmplus** для Intel 8080 / КР580ВМ80А. Итоговая сборка производится внешним `sjasmplus` (поставляется в комплекте: `include/sjasmplus`, `sjasmplus.exe`, `sjasmplus.macos`).

Пайплайн (`c8080.cpp:168-265`):

```
исходники .c ──► CParser (лексер/препроцессор/AST)
             ──► Prepare (оптимизации дерева)
             ──► Compile / CompileCmm (генерация ассемблера)
             ──► asm-оптимизатор (peephole)
             ──► .asm ── sjasmplus ──► .bin / .lst
             ──► MakeRKS (если -Orks) .rks
```

Исходники компилятора (C++17) разбиты на:

| Каталог | Назначение |
|---|---|
| `c8080_source/c/` | фронтенд: парсер C, токенизатор, макропроцессор, условная компиляция |
| `c8080_source/prepare/` | оптимизации AST (удаление мёртвых операций, замена div/mul на сдвиги, jump-оптимизация, `staticstack`) |
| `c8080_source/8080/compiler/` | генератор кода 8080 |
| `c8080_source/8080/asm/` | внутреннее представление ассемблера + оптимизатор (`deadcode`, `jumpjump`, `loadsave`) |
| `c8080_source/8080/cmm/` | поддержка языка **CMM** |
| `c8080_source/tools/` | утилиты файловой системы и строк |

---

## 2. Сборка компилятора

`c8080_source/Makefile`:

```bash
cd c8080_source
make              # Linux/macOS ─► c8080
make full         # все платформы (Linux + Win64 + Win32)
make clean
make check        # cppcheck
make format       # clang-format
```

Флаги: `-Wall -Wno-switch -O2 -static --std=c++17`. Кросс-компиляторы: `x86_64-w64-mingw32-g++` и `i686-w64-mingw32-g++`. Внешних зависимостей нет. Готовые бинарники уже лежат в корне репозитория: `c8080`, `c8080.exe`, `c8080.win32.exe`, `c8080.14-08-2025.macos`.

---

## 3. Интерфейс командной строки

Из `c8080.cpp:50-64` и `ParseOptions` (строки 67-121):

| Опция | Назначение |
|---|---|
| `-m` | компиляция языка **CMM** вместо C |
| `-I<path>` | каталог поиска заголовков |
| `-Ocpm` | выходной формат CP/M (по умолчанию) |
| `-Oi1080` | формат «Искра 1080 Tartu» |
| `-Orks` | формат RKS (Специалист/Радио-86РК) с заголовком и контрольной суммой |
| `-D<define>` | передать `#define` |
| `-o<file>` | имя бинарного файла (иначе — `<первый_исходник>.bin`) |
| `-a<file>` | имя выходного `.asm` (иначе — `<base>.asm`) |
| `-A<file>` | альтернативный путь к `sjasmplus` |
| `-V` | распечатать AST **до** оптимизаций |
| `-W` | распечатать AST **после** оптимизаций |
| `--` | прекратить разбор опций |

Путь `<exe>/include` добавляется в include-путь автоматически, если существует.

**Примеры:**

```bash
./c8080 hello.c                                        # CP/M по умолчанию
./c8080 -DARCH_SPECIALIST -Orks -ogame.rks game.c      # Specialist
./c8080 -A../../include/sjasmplus.macos -m test.c      # CMM, macOS
./c8080 -I./specialist -DARCH_86RK -Orks prog.c
```

---

## 4. Целевые платформы

Архитектурные заголовки — `include/arch/`:

| Платформа | Define | Формат | Что в библиотеке |
|---|---|---|---|
| **CP/M** | `ARCH_CPM` | `-Ocpm` | `cpm.h` (BDOS, FCB, DPB), stdio через BDOS |
| **Искра 1080 Tartu** | `ARCH_ISKRA1080` | `-Oi1080` | советская машина на 8080 |
| **«Специалист»** | `ARCH_SPECIALIST` | `-Orks` | `console.h`, `keys.h`, `colors.h` |
| **Радио-86РК** | `ARCH_86RK` | `-Orks` | расширенная консоль, цвет, клавиатура |
| **Микро-80 (цвет)** | `ARCH_MICRO80_COLOR` | `-Ocpm` | палитра, `hal.h` |

Формат **RKS** (`MakeRKS`, `c8080.cpp:137-166`): 4 байта заголовка (`start_le16=0`, `stop_le16=len-1`) + данные + 2 байта CRC, где CRC = `(Σ data[i]·257 & 0xFF00) + ((… + data.back()) & 0xFF)`.

---

## 5. Возможности языка C

**Базовые типы:** `char`/`signed char`/`unsigned char`, `short`/`int` (**16 бит**), `long` (32 бит), `long long` (64 бит), `_Bool`, `void`. Указатели — 16-битные (`SIZE_MAX = 0xFFFF`). Поддерживаются `struct` (в т.ч. вложенные), массивы (в т.ч. многомерные), указатели на функции.

Плавающая точка декларируется, но реальных операций над `float`/`double` нет — требуется внешняя библиотека.

### Расширения компилятора (`include/c8080/c8080.h`)

```c
#define __global        // статический кадр стека — по умолчанию
#define __stack         // классический динамический стек
#define __link(FILE)    // приклеить внешний .c к модулю
#define __address(N)    // жёсткая привязка объекта к адресу
```

А также `__attribute__((aligned(N)))` (коммит `5b2ac65`).

### Модели стека — ключевая особенность (`doc/function_args.txt`)

Два режима расчёта адресов переменных:

- **`__stack`** — классический C: адреса через `SP`. Например, `int __stack calc(int a,int b){return b+a;}` порождает 12 инструкций с `add hl,sp`, медленно.
- **`__global`** *(по умолчанию)* — адреса локальных переменных и параметров вычисляются **в момент компиляции**; кадр размещается статически в `__static_stack`. Тот же код → **4 инструкции**:
  ```asm
  ex   hl, de
  ld   hl, (__a_1_calc)
  add  hl, de
  ret
  ```
  Ограничение: такие функции **нельзя вызывать рекурсивно**.

Доступны внутренние символы:
- `__static_stack` — базовый статический стек;
- `__s_<func>` — адрес кадра функции;
- `__a_<N>_<func>` — адрес N-го параметра (имя функции в нижнем регистре).

Если последний параметр — целое число 8/16/32 бит, он передаётся в регистрах **A / HL / DE+HL** соответственно.

### Встроенный ассемблер

Синтаксис sjasmplus внутри `asm { … }`. Можно использовать символы `__a_N_func`, `__s_func` как адреса переменных:

```c
uint8_t __global calc(uint8_t a, uint8_t b) {
    asm {
        ld   hl, __a_1_calc
        add  (hl)
    }
}
```

Трюк «параметр зашит прямо в опкод» — через `__a_1_calc=$+1`:

```c
uint8_t __global calc(uint8_t a, uint8_t b) {
    asm {
__a_2_calc=0            ; отключить адрес для a
__a_1_calc=$+1          ; адрес b — байт следом за opcode ADI
        add  0
    }
}
```

Результат — **одна** инструкция `add` + `ret`.

---

## 6. Язык CMM (флаг `-m`)

### Что такое CMM

CMM — это микроязык, поставляемый с c8080 (заголовок `include/cmm.h`, флаг компиляции `-m`). Это «ассемблер в синтаксисе C»: регистры процессора (`a, b, c, d, e, h, l, bc, de, hl, sp`) доступны как обычные переменные, а инструкции 8080 записываются как присваивания и вызовы функций. Например:

```c
a = b;              // mov a, b
hl = 0x1234;        // lxi hl, 0x1234
a = *hl;            // mov a, m
hl += de;           // dad d
a = in(0x10);       // in 0x10
out(a, 0x10);       // out 0x10
push(bc, de);       // push bc / push de
swap(hl, de);       // xchg
if (flag_z) return; // rz
return hl();        // pchl
```

Доступны управляющие конструкции C (`if`, `while`, `for`, `do`, `break`, `continue`, `goto`), но только в формах, которые прямо отображаются в 8080-инструкции — компилятор откажется транслировать то, чему нет однозначного машинного эквивалента.

**Зачем нужен:** когда важен каждый такт и каждый байт — BIOS, обработчики прерываний, ПЗУ-резидент, плотный игровой цикл. Пример в репозитории — `examples/micro80/micro80.c` (полная прошивка Микро-80) и `examples/cmm_jp_hl/test.c` (минимальный `return hl()` → `PCHL`). Реализация кодогена — в §13.

### Справочник

Документирован прямо в `include/cmm.h` (358 строк с таблицей соответствия C ↔ инструкция ↔ флаги ↔ такты).

**Регистры — глобальные переменные:**
```c
extern uint8_t  a, b, c, d, e, h, l;   // 8-битные
extern uint16_t bc, de, hl, sp;        // 16-битные
```

**Соответствие C → 8080:**

```c
a = b;              // mov a, b          — 5 тактов
hl = 0x1234;        // lxi hl, 0x1234    — 10 тактов
a = *hl;            // mov a, m          — 7 тактов
hl += de;           // dad d             — 10 тактов
a = in(0x10);       // in 0x10           — 10 тактов
out(a, 0x10);       // out 0x10          — 10 тактов
push(bc, de, hl);   // push bc/de/hl
swap(hl, de);       // xchg              — 4 такта
invert(a);          // cma
carry_rotate_left(a, 1);  // ral
cyclic_rotate_right(a, 1); // rrc
compare(a, 5);      // cpi 5 — выставляет флаги
halt(); nop(); enable_interrupts(); disable_interrupts(); daa();
```

**Условные флаги:** `flag_z()`, `flag_nz()`, `flag_c()`, `flag_nc()`, `flag_m()`, `flag_p()`, `flag_pe()`. Конструкции `if (flag_z) …`, `while (flag_nc) …`, `do … while(flag_nz);` транслируются напрямую в `jz/jnz/jc/jnc/…`.

**Переход по HL (PCHL):** `return hl();` ↔ `pchl`. Пример — `examples/cmm_jp_hl/test.c`:
```c
#include "cmm.h"
void main() { return hl(); }
```

**Идиоматика:**
```c
if (a == 5)     ≡  if (flag_z (compare(a,5)))
if (a <  5)     ≡  if (flag_c (compare(a,5)))
do { a = in(10); } while (a != 5);  // cmp выводится автоматически
push_pop(bc) { /* тело */ }         // push/…/pop
```

Оптимизации CMM: последний `call` заменяется на `jmp`, лишний `jmp $+3` удаляется, функция автоматически дополняется `ret`.

---

## 7. Стандартная библиотека (`include/`)

Все функции помечены `__link("path/file.c")` — исходник подключается к сборке, линкуются только вызванные.

| Заголовок | Содержимое |
|---|---|
| `stdint.h` | `int8_t … uint64_t` |
| `stddef.h` | `NULL`, `size_t`, `ptrdiff_t`, `offsetof` |
| `stdbool.h` | `bool`, `true`/`false` |
| `stdarg.h` | `va_list`, `va_start/arg/end/copy` |
| `limits.h` | `CHAR_MIN … ULLONG_MAX` |
| `string.h` | `memcmp/cpy/move/set/swap`, `strchr/cmp/cpy/len/cat` (все `__global`) |
| `stdlib.h` | `rand/srand` (`RAND_MAX=0x7FFF`), `abs/labs/llabs`, `div_t`, `bdiv_t`, `ldiv_t` |
| `stdio.h` | `printf/vprintf/sprintf/snprintf/vsprintf/vsnprintf/puts/getchar/putchar` |
| `time.h` | `time_t`, `clock`, `time` |
| `unistd.h` | `sleep`, `usleep` |
| `cpm.h` | структуры DPB/FCB/PCB, системные вызовы BDOS (234 строки) |

**Дополнительно `c8080/*`:** `console.h`, `hal.h`, `countof.h`, `delay.h`, `io.h`, `remainder.h`, `tolowercase.h`, `touppercase.h`, `getbitposition.h`, `getunusedram.h`, `unpackmegalz.h` (распаковщик MegaLZ), `uint16tostring.h`, `uint32tostring.h`, `zerobitcount.h`, codepage-файлы (`codepage/micro80.h` и пр.). Компилятор генерирует внутренние рантайм-функции `__o_mul_u8`, `__o_div_u16`, `__o_shl_8`, `__o_mul_u32`, `__o_i8_to_i16`, `__o_call_hl` и т.д.

---

## 8. Примеры (`examples/`)

| Пример | Что демонстрирует | Целевые платформы |
|---|---|---|
| `cmm_jp_hl/` | минимальная CMM-программа (`return hl()`) | CMM |
| `game2048/` | игра «2048» | Specialist, 86РК, Микро-80 |
| `color_lines/` | «Цветные линии» + поиск пути + музыка | Specialist, Микро-80 |
| `kosoban/` | сокобан с уровнями | Specialist, Микро-80 |
| `micro80/` | прошивка Микро-80 (CMM, прямой ассемблер) | Микро-80 |
| `nc/` | клон Norton Commander (панели, каталог, окна) | Микро-80 |

В корне `examples/` лежат скриншоты (`.png`) и готовые образы (`.rks`, `.rk`, `.com`). Типовой build-скрипт (`examples/game2048/make_specialist`):

```bash
EXT=; ASM=
[ "$(uname -o)" = "Darwin" ] && EXT=.macos && ASM=-A../../include/sjasmplus.macos
../../c8080$EXT $ASM -ORKS -DARCH_SPECIALIST -Ispecialist -ogame2048.specialist.rks game2048.c
```

---

## 9. Оптимизатор

**AST-уровень (`prepare/`):**
- `removeuselessoperations.cpp` — удаление пустых операций;
- `replacedivmulwithshift.cpp` — `x*2^n` / `x/2^n` → сдвиги (для 8080 критично, «настоящего» `MUL` нет);
- `jump.cpp` — свёртка переходов;
- `localvariablesinit.cpp`;
- `staticstack.cpp` (≈10 000 строк) — расчёт статического стека для `__global`-функций.

**Asm-уровень (`8080/asm/optimize/`):** `deadcode` (код после безусловного `jmp`/`ret`), `jumpjump` (устранение `jmp → jmp`), `loadsave` (слияние load/save), `index`, `common`. Добавлено в коммите `b1c8026` «Optimizer».

---

## 10. i8080-специфика и трюки

- **Соглашение вызова:** параметры через статический кадр (режим `__global`) или стек (`__stack`). Возврат — в `A` (8 бит), `HL` (16 бит), `DEHL` (32 бит). Последний целочисленный параметр часто передаётся в регистрах.
- **XCHG (`ex hl, de`)** широко используется для перестановки операндов без лишних `mov`.
- **PCHL** как дешёвый косвенный переход / возврат табличных функций.
- **DAA** присутствует только в CMM-обёртке.
- **`RST 0..38h`** — TODO в `cmm.h`.
- Такты каждой инструкции документированы в `cmm.h` — полезно при ручной оптимизации.
- В CMM поддерживается смешивание выражений и побочных эффектов в условиях: `if (flag_z(a |= b)) …` — вычисление `a |= b` перед проверкой флага.
- Адрес вызова можно писать числом: `100()` — эквивалентно `((void(*)())100)()`.
- `asm(" .org 0xF800");` — глобальная директива размещения (см. `examples/micro80/micro80.c:12`).
- `extern uint8_t rst30Opcode __address(0x30);` — привязка переменной к конкретному адресу (векторы прерываний).

---

## 11. Форматы выхода

- `.asm` (sjasmplus) — **всегда**;
- `.bin` — прямой бинарь (CP/M, Искра-1080, Микро-80 `.com`);
- `.lst` — листинг sjasmplus;
- `.rks` — с 4-байтным заголовком и 2-байтной CRC (Специалист/86РК);
- Если не задан `-o`/`-a`, имена берутся от первого исходника: `<base>.asm`, `<base>.bin`.

---

## 12. Полезные мелочи

- `c8080.creator` — проектный файл Qt Creator, `c8080.includes` — список include-путей для IDE.
- `cmm.h` при компиляции не под c8080 (макрос `__C8080_COMPILER` не определён) разворачивается в C++-классы-заглушки (`class Register`) — подсказки IDE работают.
- При `-m` в `programm.cmm` выставляется флаг, включающий отдельный путь `CompileCmm` вместо обычного `Compile`.
- `c8080.14-08-2025.macos` — исторический бинарь macOS (ссылается в `make_specialist` через `EXT=.macos`).
- `.clang-format` настроен на стиль проекта — использовать `make format` перед коммитом.

---

## 13. CMM-кодоген изнутри

Реализация находится в `c8080_source/8080/cmm/` (файлы `cmm.cpp`, `names.cpp`, `arg.h`, `prepare.cpp`). Путь активируется флагом `-m` и заменяет обычный `Compile()` на `CompileCmm()` (`c8080.cpp:31, cmm.cpp:623`).

### Регистрация «встроенных имён»

При старте CMM-парсера вызывается `RegisterInternalCmmNames(p)` (`names.cpp:36`), которая добавляет в таблицу символов переменные-регистры и функции-интринсики. Каждой сущности присваивается `internal_cmm_name` (перечисление `CmmName` в `names.h`):

| Группа | Символы | Соответствие в 8080 |
|---|---|---|
| 8-бит регистры | `a, b, c, d, e, h, l` | `R8_A … R8_L` |
| 16-бит пары | `bc, de, hl, sp` | `R16_BC … R16_SP` |
| Флаги | `flag_z, flag_nz, flag_c, flag_nc, flag_m, flag_p, flag_pe, flag_po` | условия `JC_Z … JC_PO` |
| Управление | `enable_interrupts, disable_interrupts, nop, daa, set_flag_c, invert` | `EI, DI, NOP, DAA, STC, CMA` |
| Ротации | `cyclic_rotate_left/right`, `carry_rotate_left/right` | `RLCA, RRCA, RLA, RRA` |
| АЛУ | `compare, carry_add, carry_sub` | `CMP, ADC, SBC` |
| Стек | `push, pop` | `PUSH/POP r16` (variadic) |
| Порты | `in, out` | `IN port`, `OUT port` |
| Обмен | `swap` | `XCHG` или `XTHL` |

Имена регистров парсятся как обычные C-переменные, но при встрече узла `CNT_LOAD_VARIABLE` кодогенератор видит `internal_cmm_name == CMM_NAME_REG` и кладёт в `CmmArg::reg` машинный код регистра (`cmm.cpp:306-313`).

### Представление операнда — `CmmArg` (`arg.h`)

Ключевой класс. У каждого выражения есть:
- `reg` — регистр (или `REG_CONST` для константы, `REG_PORT` для результата `in()`);
- `addr` — флаг разыменования (`*hl`);
- `text` — строковое представление константы.

Предикаты: `IsA()`, `IsHl()`, `Is8M()` (любой 8-битный регистр или `M=(HL)`), `Is16Sp()` (любая 16-битная пара), `Is16Af()` (включая `AF`), `IsConst()`, `IsConstAddr()`, `IsPort()`. На основании их комбинаций выбирается конкретная 8080-инструкция.

### Таблица трансляции оператора `=` (`cmm.cpp:460-487`)

```
hl  = variable       →  LHLD variable         (IsHl + IsConstAddr)
variable = hl        →  SHLD variable         (IsConstAddr + IsHl)
a   = *bc / *de      →  LDAX B/D              (IsA + IsRegAddr)
*bc = a / *de = a    →  STAX B/D
a   = variable       →  LDA variable          (IsA + IsConstAddr)
variable = a         →  STA variable
sp  = hl             →  SPHL                  (16Sp + IsHl)
r8  = r8 / *hl       →  MOV r,r' / MOV r,M    (Is8M, кроме M,M)
r8  = const          →  MVI r,const
r16 = const          →  LXI r16,const
a   = in(const)      →  IN const              (IsA + IsPort)
```

Если ни один паттерн не подошёл — `CompileOperatorError` выдаёт сообщение «can't compile …», показывая как раз разобранную форму операнда.

### Арифметика (`CompileAlu`, `cmm.cpp:276-299`)

Только форма `(a, …)` допустима: `add/sub/and/or/xor` принимают аккумулятор слева и 8-битный регистр / `*hl` / константу справа. Исключение — `hl += r16` превращается в `DAD r16` (`cmm.cpp:488-490`).

### Условия (`CompileCond`, `cmm.cpp:79-125`)

Три разных формы в AST замыкаются на один `AsmCondition`:

```c
if (a < 5)            → CNT_OPERATOR (<): CMP + JC
if (flag_z)           → CNT_MONO_OPERATOR (&flag_z): переход по JZ
if (flag_z(a |= b))   → CNT_FUNCTION_CALL: сначала тело (|=), потом JZ
```

Оператор `==` с правым операндом `0` специально оптимизируется в `OR A` вместо `CPI 0` (`cmm.cpp:92-93`) — экономия 3 тактов.

### PCHL (`return hl()`, `cmm.cpp:335-347`)

Распознаётся как особый случай `CNT_RETURN` → дочерний `CNT_FUNCTION_CALL_ADDR` с регистром `HL`. Компилируется в одну инструкцию `PCHL`.

### Циклы (`cmm.cpp:371-413`)

- `for (init; cond; step) body` — инвертирует условие, прыгает на `break_label`, в конце — `jmp` на начало, опционально `continue_label` перед `step`.
- `do { body } while(flag);` — метка `loop`, тело, условный прыжок назад. Если используется `break`, метка `break_label` эмитируется только при `used != 0`.
- `break`/`continue` требуют активной пары `break_label`/`continue_label` и порождают `JMP` (или `J?` если в `if`).

### Tail-call и автоматический `ret` (`cmm.cpp:586-601`)

После компиляции тела функции проверяется последняя инструкция в `out.lines`:
- `CALL` → превращается в `JMP` (tail-call оптимизация);
- `PCHL` / `JMP` / `RET` — функция уже корректно завершена, `ret` не нужен;
- иначе — добавляется `RET`.

То же для цепочек «функция, за которой сразу другая» (`cmm.cpp:565-569`): последний `JMP next_function` удаляется, если `next_function` идёт сразу следом — контроль «протекает».

### Оптимизации условного `if` с одной строкой (`cmm.cpp:417-444`)

Если в блоке `if (cond)` только одна операция и нет `else`, кодогенератор избегает лишней метки:

```c
if (flag_z) return;             → RZ                 (ret_condition)
if (flag_z) return func();      → JZ func            (jmp_condition)
if (flag_z) func();             → CZ func            (call_condition)
if (flag_z) goto label;         → JZ label
if (flag_z) break;              → JZ break_label
```

### `push_pop(regs) { body }` (`cmm.cpp:170-195`)

Макрос разворачивается в `for (push(regs);; pop(regs))`, а кодоген превращает его в `PUSH …; body; POP …` в обратном порядке. Регистр `A` автоматически маппится на `AF`, чтобы вместе с ним сохранились флаги.

### Ассемблерные вставки и глобальные `asm("…")`

Узлы `CNT_ASM` (блоки `asm { … }` внутри функции и директивы `asm(" .org …")` на верхнем уровне) проталкиваются в итоговый `.asm` практически «как есть» — через `out.assembler(text)` (`cmm.cpp:537, 609`). Поэтому любой sjasmplus-синтаксис (метки, `equ`, `db`, `org`, условия `IF/ELSE/ENDIF`) доступен напрямую.

---

## 14. Формат RKS и его «загрузчик»

RKS — это формат, понимаемый магнитофонной подсистемой «Специалиста» и Радио-86РК. C8080 сначала собирает чистый бинарь с `org 0`, а затем «обёртывает» его в `MakeRKS` (`c8080.cpp:137-166`).

### Структура файла

```
┌───────── 4 байта ─────────┬──── N байт ────┬── 2 байта ──┐
│ start_lo start_hi         │                │ crc_lo crc_hi │
│ end_lo   end_hi           │  полезная нагрузка (тело)        │
└───────────────────────────┴────────────────┴─────────────┘
       заголовок                данные           CRC
```

- `start_le16` = `0` (c8080 всегда компилирует с `org 0`, см. `Compile.cpp:83-86`);
- `end_le16` = `len(data) - 1` — адрес последнего байта;
- `crc_le16` — контрольная сумма (о ней ниже).

Старшие/младшие байты в little-endian — порядок «младший, старший».

### Алгоритм CRC (`c8080.cpp:153-156`)

```cpp
uint16_t checksum = 0;
for (size_t i = 0; i < data.size() - 1; i++)
    checksum += data[i] * 257;                               // ·257 = ·(256+1) → размножает байт в обе половинки
checksum = (checksum & 0xFF00)
         + ((checksum + data.back()) & 0xFF);
```

Особенность: последний байт добавляется только в младшую половину. Эквивалентная форма:

```
hi = (Σ_{i<n-1} data[i]) mod 256     ; старший байт
lo = (Σ_{i<n}   data[i]) mod 256     ; младший байт
crc = (hi << 8) | lo
```

То есть старший байт — сумма всех байтов **кроме последнего**, младший — сумма всех байтов **включая последний**. Такая «двойная» сумма нужна, потому что при магнитофонной загрузке каждый байт проходит через тот же накапливающий сумматор дважды.

### Как это «загружается»

RKS-файл кладут в WAV-образ кассетной ленты и проигрывают в эмуляторе. Монитор «Специалиста» (ROM по адресу `0xC000`) стартует чтение, первые 4 байта задают диапазон `[start, end]`, ROM грузит этот диапазон в ОЗУ и сверяет CRC. При совпадении управление передаётся пользовательской программе — обычно командой `G start` из монитора либо, если компилятор выставил `org 0`, через заглушку загрузчика.

C8080 генерирует исполнение без SP-инициализации на старте: стек выставляется фрагментом `initstack.inc`:

```asm
ld   sp, 089FFh    ; под переменными BIOS
ld   hl, 0C800h    ; адрес «рестарта» монитора
push hl            ; чтобы ret вернулся в ROM
```

Т. е. `main()` просто делает `RET`, чтобы вернуться в монитор. Адрес `0xC800` — точка входа BIOS-подобного монитора «Специалиста».

### Прочие форматы (для сравнения)

- **CP/M (`-Ocpm`)** — `org 100h`, никакого заголовка, файл `.bin` грузится как стандартная `.COM`-программа в TPA.
- **Iskra 1080 (`-Oi1080`)** — `org 100h - 16h`, первые 16 байт — подпись `"ISKRA1080",0xD0,"A.OUT ",entry_lo,entry_hi,end_lo,end_hi,entry_lo,entry_hi` (`Compile.cpp:76-81`).
- **RKS (`-Orks`)** — `org 0`, пост-обработка `MakeRKS`.

---

## 15. Работа с консолью «Специалиста»

Платформенный слой для «Специалиста» находится в `include/arch/specialist/`. Подключается добавлением `-DARCH_SPECIALIST -Ispecialist` в командную строку — тогда заголовки из этой директории «перекрывают» общие.

### Карта памяти «Специалиста» (`halconsts.h`)

```
0x0000 – 0x8FFF  ОЗУ (пользовательская)
0x8A00 – 0x8FFF  Переменные BIOS, стек → 0x89FF (см. initstack.inc)
0x9000 – 0xBFFF  Экран (видеопамять, 12 КБ)
0xC000 – 0xC7FF  ROM: монитор «Специалиста»
0xC800           Точка входа/reset монитора
0xC803           BIOS: ввод символа (getchar)
0xC809           BIOS: вывод символа (putchar)
0xF800 – 0xF803  Порты клавиатуры (memory-mapped 8255)
0xFFFE           Регистр цвета чернил (запись)
```

Экран — 384×256 монохромный, линейная организация «колонками»: 48 тайлов по горизонтали × 256 строк, `BPL = 0x100`, общий размер `0x3000`. Макрос `TILE(X, Y)` (`halconsts.h:34`) даёт адрес байта:
```c
#define TILE(X, Y) (SCREEN + (Y) + (X) * SCREEN_BPL)
```
`SCREEN` = `(uint8_t*)0x9000`.

Цвет задаётся через запись в `0xFFFE`:
```c
#define SET_COLOR(C) do { *(uint8_t*)0xFFFE = (C) & 0xD0; } while (0)
```
Маска `0xD0` оставляет только значимые биты цвета. Доступные чернила (`colors.h`):

| Константа | Значение | Цвет |
|---|---|---|
| `COLOR_INK_WHITE` | `0x00` | белый |
| `COLOR_INK_YELLOW` | `0x10` | жёлтый |
| `COLOR_INK_VIOLET` | `0x40` | фиолетовый |
| `COLOR_INK_RED` | `0x50` | красный |
| `COLOR_INK_CYAN` | `0x80` | голубой |
| `COLOR_INK_GREEN` | `0x90` | зелёный |
| `COLOR_INK_BLUE` | `0xC0` | синий |
| `COLOR_INK_BLACK` | `0xD0` | чёрный |

### Текстовый интерфейс через BIOS (`console.h`)

Лёгкие обёртки над монитором `0xC800`:

| Функция | Что делает | Как реализовано |
|---|---|---|
| `ClearConsole()` | очистка экрана | `ld c,1Fh; jp 0C809h` |
| `ResetConsole()` | сброс + очистка | то же, `0x1F` |
| `MoveCursorHome()` | курсор в угол | `ld c,0Ch; jp 0C809h` |
| `ShowCursor()` / `HideCursor()` | no-op на «Специалисте» | курсора нет |
| `SetConsoleColor(c)` / `MoveCursor(x,y)` / `GetCursorPosition()` | управление ПК | через BIOS |

`putchar` (`stdio_h/putchar.c`) аналогично транслирует `\n` в пару `CR LF` и вызывает `0xC809`:
```asm
ld   a, l
cp   0Ah           ; '\n'
ld   c, 0Dh
call z, 0C809h     ; если перевод строки — сначала CR
ld   c, l
call 0C809h        ; сам символ
```

`getchar` (`stdio_h/getchar.c`) — вызов `0xC803` с типичным паттерном расширения знака:
```asm
call 0C803h
ld   l, a
rla
sbc  a           ; A = 0 если флаг C=0, иначе 0xFF — классический sign-extend
ld   h, a
```

### Графический вывод через HAL

Более низкоуровневый API живёт в `hal.h` / `halconsts.h` — он обходит BIOS и пишет в видеопамять напрямую.

**Рисование тайлов:**
```c
void DrawImageTileMono(void *tile, const void *image, uint16_t width_height);
```
`tile` получается как `TILE(x, y)`, `width_height` упаковывает размер 8×8 блоков.

**Текст с шрифтом 6×8 (`chargen6`):**
```c
DrawTextNormal();             // режим: наложение (AND + NOP)
DrawTextXor();                // XOR
DrawTextInverse();            // инверсия (CMA + AND)
DrawText(tile, x, color, "Hello");
```
Реализация (`hal_h/drawtext.c`) использует хитрый трюк: перед вызовом `DrawText*()` функции-переключатели **патчат машинный код** пяти точек (`drawtext_and1…5`, `drawtext_cma1…5`) внутри четырёх рисующих функций `DrawTextA/B/C/D`. `DrawTextA` работает для столбцов, выровненных по 4 (`x & 3 == 0`), а `B/C/D` — для трёх остальных фаз, потому что каждый байт экрана содержит по 4 пиксельных «кирпича». Это типичный для 8080 приём — self-modifying code ради скорости.

**Заливка прямоугольников (`fillrect.h`):**
```c
FillRectOver();       // режим: OR
FillRectInverse();    // XOR
FillRectTrue();       // замена
FillRectXY(x0, y0, x1, y1);                  // «высокий» API
FillRect(tile, centerSize, leftMask, rightMask, height);  // низкий
// или макрос FILLRECTARGS(x0, y, x1, y1), который раскроет аргументы:
FillRect(FILLRECTARGS(10, 20, 30, 40));
```

**Звук (`hal_h/sound.c`):** `Sound(period, count)` — бипер через бит порта `0xF803`, с ручным отсчётом тактов процессора (2 МГц). Используется трюк с параметром-в-опкоде (`__a_1_sound=$+1`).

### Клавиатура (`keys.h`, `hal_h/scankey.c`)

Клавиатура — матрица 8×7, опрашивается через порты `0xF800` (строка), `0xF801` (столбец-маска), `0xF803` (режим):

```c
uint8_t ScanKey(void);      // возвращает «сырой» скан-код, 0xFF если ничего
bool    IsShiftPressed(void);
uint8_t DecodeReadKey(uint8_t scan);  // с учётом Shift/Rus
uint8_t DecodeInKey(uint8_t scan);
uint8_t ReadKey(void);       // блокирующий, с антидребезгом и автоповтором
uint8_t InKey(void);         // неблокирующий (0xFF если нет)
```

`ReadKey()` реализует полную логику (`readkey.c`):
- антидребезг `KEY_DEBOUNCE_TIME = 128`;
- задержка перед автоповтором `KEY_DELAY_RATE = 512`;
- скорость автоповтора `KEY_REPEAT_RATE = 256`.

Константы клавиш из `keys.h` (основные):

| Константа | Код | Константа | Код |
|---|---|---|---|
| `KEY_F1 … KEY_F8` | 0x00–0x07 | `KEY_ESC` | 0x1B |
| `KEY_LEFT` | 0x08 | `KEY_SPACE` | 0x20 |
| `KEY_TAB` | 0x09 | `KEY_BACKSPACE` | 0x7F |
| `KEY_HOME` | 0x0C | `KEY_ENTER` | 0x0D |
| `KEY_UP` | 0x19 | `KEY_RIGHT` | 0x18 |
| `KEY_DOWN` | 0x1A | `KEY_F12` | 0x1F |
| `KEY_F9 / F10 / F11 / PV / RUS` | 0x0B/0x0E/0x0F/0x10/0x11 | `KEY_F9_BIOS` | 0x8A |

### Кодовая страница

`consoleconsts.h` подтягивает `c8080/codepage/koi7.h` — BIOS «Специалиста» использует КОИ-7. Для графического рисования текста через `DrawText` используется уже `866` (из `halconsts.h`). Путаница кодировок — реальный источник багов; при смешанном выводе (BIOS + прямой рендер) надо явно конвертировать строки.

### Тактовая частота

`cpumhz.h`:
```c
#define __C8080_CPU_MHZ 2000000   // 2 МГц
```
Используется `<c8080/delay.h>` для расчёта задержек в тактах.

### Минимальный «Hello» для Specialist

```c
#include <stdio.h>
#include <c8080/console.h>

void main(void) {
    ClearConsole();
    puts("Hello, Specialist!\n");
}
```

Сборка:
```bash
./c8080 -DARCH_SPECIALIST -Orks -ohello.rks hello.c
```
Получаем `hello.rks` — кидаем в эмулятор «Специалиста» через кассетный ввод, в мониторе пишем загрузку — программа запускается, после `RET` управление возвращается в монитор (см. `initstack.inc`).
