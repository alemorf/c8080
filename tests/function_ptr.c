#include <stdio.h>

extern int (*ptr)(int);

int test(int a) {
    printf("test %i\n", a);
    if (a < 0)
        test(-a);
}

int (*a)(int) = test;
int (*b)(int) = &test;

int main(int, char **) {
    int (*ptr)(int);
    ptr = test;
    ptr = &test;
#ifndef __GNU__
    ptr = &(&test);
#endif

    ptr(-5);
    (*ptr)(-11);
    a(-25);
    (*b)(-110);
#ifndef __GNU__
    (**b)(-512);
#endif
    return 0;
}
