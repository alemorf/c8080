#ifdef __C8080_COMPILER
#define static_assert(X) typedef char[(X) ? 1 : -1];
#else
#include <assert.h>
#endif

#define A1(B) B*B
#define A2(A) A1(A1(A))

static_assert(A2(2) == 16);

#define MACRO(X) ((X)*(X))

static_assert(MACRO(MACRO(MACRO(2))) == 256);

#define LOOP LOOP

int LOOP;

#define LOOP2 LOOP1
#define LOOP1 LOOP2

int LOOP1;

int main(int, char**) {
    return 0;
}
