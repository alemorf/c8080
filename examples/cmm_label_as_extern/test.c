// License: Apache License Version 2.0

#include "cmm.h"

extern int some_label;
extern int some_label_1 __address("some_label + 1");

void test() {
    hl = some_label;
    de = &some_label_1;
}

void main() {
some_label:
    bc = 1;
}
