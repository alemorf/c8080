#include <sys/tree.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

struct my_item {
    RB_ENTRY(my_item) rb;
    int i;
};

static int my_item_compare(struct my_item *a, struct my_item *b) {
    return a->i < b->i ? -1 : a->i > b->i;
}

RB_HEAD(my_tree, my_item);
RB_GENERATE(my_tree, my_item, rb, my_item_compare);

static bool my_tree_add(struct my_tree *t, int i) {
    struct my_item *a = malloc(sizeof(struct my_item));
    if (a == NULL)
        return false;
    a->i = i;
    RB_INSERT(my_tree, t, a);
    printf("add 0x%p\n", a);
    return true;
}

static struct my_item *my_tree_find(struct my_tree *t, int i) {
    struct my_item what;
    what.i = i;
    return RB_FIND(my_tree, t, &what);
}

static void my_tree_destroy(struct my_tree *t) {
    struct my_item *i, *tmp;
    RB_FOREACH_SAFE(i, my_tree, t, tmp) {
        RB_REMOVE(my_tree, t, i);
        free(i);
        printf("free 0x%p\n", i);
    }
}

int main(int, char **) {
    static struct my_tree t = RB_INITIALIZER(my_tree);

    my_tree_add(&t, 10);
    my_tree_add(&t, 2);
    my_tree_add(&t, 14);
    my_tree_add(&t, 5);
    my_tree_add(&t, 1);

    for (int i = 0; i < 16; i++)
        printf("find %i = 0x%p\n", i, my_tree_find(&t, i));

    my_tree_destroy(&t);

    return 0;
}
