#if defined(ARCH_CPM_BIOS)
    /* BDOS and command processor are removed from memory. */
#error TODO
#else
#if defined(ARCH_CPM_BDOS)
    /* Command processor is removed from memory. */
    asm {
        ld   a, (7)
        ld   h, a
        ld   l, 0
        ld   sp, hl

        ld   hl, 0
        push hl
    }
#else
    /* clang-format on */
    asm {
        ld   hl, 0
        add  hl, sp
        ex   hl, de
        
        ld   a, (7)
        sub  8
        ld   h, a
        ld   l, 0
        ld   sp, hl

        push de
    }
    /* clang-format off */
#endif
#endif

    __HEAP_INIT

    static char argv0[1];
    static char *argv[] = { argv0, 0, 0 };

    asm {
        ld   hl, 81h
        ld   a, ' '
__cpm_initstack_0:
        cp   (hl)
        ld   (__s_argv + 2), hl
        inc  hl
        jp   nz, __cpm_initstack_0
    }

    main(argv[1][0] != 0 ? 2 : 1, argv);

#if !defined(ARCH_CPM_BIOS) && !defined(ARCH_CPM_BDOS)
    asm {
        pop  hl
        ld   sp, hl
    }
#endif
    return;
