#include <stdlib.h>
#include <stdio.h>

int main(int, char **) {
    printf("Start\n");
    char *a = malloc(128);
    printf("A %p\n", a);
    char *b = malloc(256);
    printf("B %p\n", b);
    char *c = malloc(512);
    printf("C %p\n", c);
    char *d = malloc(1024);
    printf("D %p\n", d);

    free(a);
    free(b);
    free(c);

    char *e = malloc(768);
    printf("E %p\n", e);
    char *f = malloc(128);
    printf("F %p\n", f);

    free(d);
    free(e);
    free(f);

    char *g = malloc(4096);
    printf("G %p\n", g);

    char *h = malloc(65000);
    printf("H %p\n", h);

    return 0;
}
