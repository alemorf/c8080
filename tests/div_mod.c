#include <stdio.h>
#include <stdint.h>

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

    return 0;
}
