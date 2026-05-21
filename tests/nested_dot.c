// Regression test for nested struct member access.
//
// Before the ctokenizer.cpp:202 fix, the tokenizer treated any `.X.`
// sequence as the `...` operator, so `o.i.a` was lexed as the single
// token `.i.` and the parser reported "unexpected '.i.'".
//
// Expected behaviour: compiles without error.

struct Inner {
    int a;
};

struct Outer {
    struct Inner i;
};

void __main() {
    struct Outer o;
    o.i.a = 42;
}
