#include <stdio.h>

typedef signed long long __int64_t;

#define A 5

#if A == 1
#warning A1
#elif A <= 2
#warning A2
#elif A <= 3
#warning A3
#elif A <= 4
#warning A4
#else
#warning AE
#endif

#ifdef B
#warning B1
#elif defined(B)
#warning B2
#endif

int main(int, char **) {
    return 0;
}
