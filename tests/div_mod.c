#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int, char **) {
    {
        volatile uint8_t a = 100, b = 3;
        volatile uint8_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%u / %u = %u mod %u check %u\n", a, b, c, d, check);
    }
    {
        volatile int8_t a = 100, b = 3;
        volatile int8_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%i / %i = %i mod %i check %i\n", a, b, c, d, check);
    }
    {
        volatile int8_t a = 100, b = -3;
        volatile int8_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%i / %i = %i mod %i check %i\n", a, b, c, d, check);
    }
    {
        volatile int8_t a = -100, b = 3;
        volatile int8_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%i / %i = %i mod %i check %i\n", a, b, c, d, check);
    }
    {
        volatile int8_t a = -100, b = -3;
        volatile int8_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%i / %i = %i mod %i check %i\n", a, b, c, d, check);
    }

#if 0
    for (uint16_t i = 0; i < 0x100; i++) {
        printf("Test %i\n", i);
        for (uint16_t j = 1; j < 0x100; j++) {
            volatile int8_t a = i, b = j;
            volatile int8_t c = a / b, d = a % b, check = c * b + d - a;
            if (check != 0) {
                printf("%i / %i = %i mod %i check %i\n", a, b, c, d, check);
                return 1;
            }
        }
    }
#endif

    // 16 bit

    {
        volatile uint16_t a = 30000, b = 51;
        volatile uint16_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%u / %u = %u mod %u check %u\n", a, b, c, d, check);
    }
    {
        volatile int16_t a = 30000, b = 51;
        volatile int16_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%i / %i = %i mod %i check %i\n", a, b, c, d, check);
    }
    {
        volatile int16_t a = 30000, b = -51;
        volatile int16_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%i / %i = %i mod %i check %i\n", a, b, c, d, check);
    }
    {
        volatile int16_t a = -30000, b = 51;
        volatile int16_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%i / %i = %i mod %i check %i\n", a, b, c, d, check);
    }
    {
        volatile int16_t a = -30000, b = -51;
        volatile int16_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%i / %i = %i mod %i check %i\n", a, b, c, d, check);
    }

#if 0
    for (uint16_t i = 0; i < 0xFFFF; i++) {
        volatile int16_t a = rand(), b = rand();
        volatile int16_t c = a / b, d = a % b, check = c * b + d - a;
        if (check != 0)
            printf("%i / %i = %i mod %i check %i\n", a, b, c, d, check);
        if (check != 0)
            return 1;
    }
#endif

    // 32 bit

    {
        volatile uint32_t a = 2000000000, b = 76543;
        volatile uint32_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%lu / %lu = %lu mod %lu check %lu\n", a, b, c, d, check);
    }
    {
        volatile int32_t a = 2000000000, b = 76543;
        volatile int32_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%li / %li = %li mod %li check %li\n", a, b, c, d, check);
    }
    {
        volatile int32_t a = -2000000000, b = 76543;
        volatile int32_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%li / %li = %li mod %li check %li\n", a, b, c, d, check);
    }
    {
        volatile int32_t a = 2000000000, b = -76543;
        volatile int32_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%li / %li = %li mod %li check %li\n", a, b, c, d, check);
    }
    {
        volatile int32_t a = -2000000000, b = -76543;
        volatile int32_t c = a / b, d = a % b, check = c * b + d - a;
        printf("%li / %li = %li mod %li check %li\n", a, b, c, d, check);
    }

#if 1
    for (uint16_t i = 0; i < 0xFFFF; i++) {
        volatile int32_t a = rand() | (rand() << 16L), b = rand() | (rand() << 16L);
        volatile int32_t c = a / b, d = a % b, check = c * b + d - a;
        //        if (check != 0)
        printf("%li / %li = %li mod %li check %li\n", a, b, c, d, check);
        if (check != 0)
            return 1;
    }
#endif

    return 0;
}
