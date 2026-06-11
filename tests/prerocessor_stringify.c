#include <stdio.h>

#define N(S) S

#define A(S) #S

#define C(S) #S #S
#define B(S) C(S) #S

#ifdef FAIL
#define E1(S) #
#define E2(S) #s
#endif

int main(int, char **) {
    puts(A(  \x21  ) "TAB");
    puts(A(N(3)));
    puts(B(Hello));
#ifdef FAIL
    puts(#Hello);
    E1(Hello)
    E2(Hello)
#endif
    return 0;
}
