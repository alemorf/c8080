#ifdef __C8080_COMPILER
#define static_assert(X) typedef char[(X) ? 1 : -1];
#else
#include <assert.h>
#endif

// В gcc добавление префикса U не обнуляет старшие разряды и не уменьшает разрядность
#ifdef __C8080_COMPILER
static_assert((unsigned)(0xFFFF) == 0xFFFFU);
static_assert((unsigned)(0x1FFFF) != 0x1FFFFU);
static_assert(sizeof(0x1FFFFU) == 4);
#else
static_assert((unsigned)(0xFFFFFFFF) == 0xFFFFFFFFU);
static_assert((unsigned)(0x1FFFFFFFF) != 0x1FFFFFFFFU);
static_assert(sizeof(0x1FFFFFFFFU) == 8);
#endif

// Добавление префикса L увеличивает разрядность.
#ifdef __C8080_COMPILER
static_assert(sizeof(0x1FFFL) == 4);
#endif
static_assert(sizeof(0x1FFFFFFFLL) == 8);

// Сравнение int и unsigned
#ifdef __C8080_COMPILER
static_assert(0xFFFF == -1);
#else
static_assert(0xFFFFFFFF == -1);
#endif

// Префикс позволяет в некоторых случаях указать разрядность.
#ifdef __C8080_COMPILER
static_assert(sizeof(0xFFFF + 0xFFFF) == 2);
static_assert(sizeof(0xFFFFL + 0xFFFF) == 4);
#else
static_assert(sizeof(0xFFFFFFFF + 0xFFFFFFFF) == 4);
static_assert(sizeof(0xFFFFFFFFLL + 0xFFFFFFFF) == 8);
#endif

// Не вызывает предупреждений при переполнении, т.к. минус выполяется после.
static_assert(sizeof(-0xFFFFFFFFFFFFFFFFU) == 8);

// А еще минус не меняет тип
#ifndef __C8080_COMPILER
static_assert(_Generic(-0xFFFFFFFFFFFFFFFFU, long unsigned int : 1));
#endif

// Очевидный случай

#ifdef __C8080_COMPILER
static_assert(sizeof(0x7FFF) == sizeof(int));
static_assert(0x7FFF > 0);
#else
static_assert(sizeof(0x7FFFFFFF) == sizeof(int));
static_assert(0x7FFFFFFF > 0);
static_assert(_Generic(0x7FFFFFFF, int : 1));
#endif

// Указание типа unsigned

#ifdef __C8080_COMPILER
static_assert(sizeof(0x7FFFU) == sizeof(int));
static_assert(0x7FFFU > 0);
#else
static_assert(sizeof(0x7FFFFFFFU) == sizeof(int));
static_assert(0x7FFFFFFFU > 0);
static_assert(_Generic(0x7FFFFFFFU, unsigned : 1));
#endif

// INT_MAX+1 ... UINT_MAX десятичные автоматически не приводится к unsigned

#ifdef __C8080_COMPILER
static_assert(65535 == 0xFFFF);
static_assert(sizeof(65535) == sizeof(long));
static_assert(sizeof(0xFFFF) == sizeof(int));
#else
static_assert(4294967295 == 0xFFFFFFFF);
static_assert(sizeof(4294967295) == sizeof(long long));
static_assert(sizeof(0xFFFFFFFF) == sizeof(int));
#endif

// INT_MAX+1 ... UINT_MAX автоматически приводится к unsigned

#ifdef __C8080_COMPILER
static_assert(sizeof(0xFFFF) == sizeof(int));
static_assert(sizeof(0xFFFFU) == sizeof(int));
static_assert(0xFFFF > 0) static_assert(0xFFFFU > 0)
#else
static_assert(sizeof(0xFFFFFFFF) == sizeof(unsigned));
static_assert(sizeof(0xFFFFFFFFU) == sizeof(unsigned));
static_assert(_Generic(0xFFFFFFFF, unsigned : 1));
static_assert(_Generic(0xFFFFFFFFU, unsigned : 1));
static_assert(0xFFFFFFFF > 0);
static_assert(0xFFFFFFFFU > 0);
#endif

    // UINT_MAX ... LONG_MAX опять signed

    static_assert(sizeof(0x7FFFFFFFFFFFFFFF) == sizeof(long long));
static_assert(0x7FFFFFFFFFFFFFFF > 0);
#ifndef __C8080_COMPILER
static_assert(_Generic(0x7FFFFFFFFFFFFFFF, long int : 1));
#endif

// Указание типа unsigned

static_assert(sizeof(0x7FFFFFFFFFFFFFFFU) == sizeof(long long));
static_assert(0x7FFFFFFFFFFFFFFFU > 0);
#ifndef __C8080_COMPILER
static_assert(_Generic(0x7FFFFFFFFFFFFFFFU, long unsigned int : 1));
#endif

// >LONG_MAX автоматически приводится к unsigned

static_assert(sizeof(0xFFFFFFFFFFFFFFFF) == sizeof(long long));
static_assert(sizeof(0xFFFFFFFFFFFFFFFFU) == sizeof(long long));
static_assert(0xFFFFFFFFFFFFFFFF > 0);
static_assert(0xFFFFFFFFFFFFFFFFU > 0);
#ifndef __C8080_COMPILER
static_assert(_Generic(0xFFFFFFFFFFFFFFFF, long unsigned int : 1));
static_assert(_Generic(0xFFFFFFFFFFFFFFFFU, long unsigned int : 1));
#endif

// Переполнение десятичного знакового числа

static_assert(18446744073709551615 == 18446744073709551615U);
// warning: integer constant is so large that it is unsigned

// Самое большое число компилятора

static_assert(0x1FFFFFFFFFFFFFFFF == 0xFFFFFFFFFFFFFFFF);
// warning: integer constant 0x1FFFFFFFFFFFFFFFF is too large

// Варианты

static_assert(sizeof(1LU) == sizeof(long));
static_assert(sizeof(1UL) == sizeof(long));
static_assert(sizeof(1ULL) == sizeof(long long));
#ifdef __C8080_COMPILER
static_assert(sizeof(1LUL) == sizeof(long long));
#endif
static_assert(sizeof(1LLU) == sizeof(long long));

// Плавающая точка

static_assert(sizeof(123.456f) == sizeof(float));
static_assert(sizeof(123.456) == sizeof(double));
static_assert(sizeof(123.456l) == sizeof(long double));

int main(int, char **) {
    return 0;
}
