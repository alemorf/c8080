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
#elifndef A
#warning B2
#elifdef A
#warning B3
#elifx defined(B)
#warning B4
#else
#warning B5
#endif

int main(int, char **) {
    return 0;
}
