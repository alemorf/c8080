#if !defined(ARCH_CPM_BIOS) && !defined(ARCH_CPM_BDOS)
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

    /* Command processor is removed from memory. */
#if defined(ARCH_CPM_BDOS) 
    asm {
        ld   a, (7)
        ld   h, a
        ld   l, 0
        ld   sp, hl
        
        ld   hl, 0
        push hl
    }
#endif

    /* BDOS and command processor are removed from memory. */
#if defined(ARCH_CPM_BIOS) 
#error TODO
#endif

    static char argv0[1];
    static char *argv[] = { argv0, (char*)0x81 };
    while (argv[1][0] == ' ')
        argv[1]++;
    main(argv[1][0] != 0 ? 2 : 1, argv);

#if !defined(ARCH_CPM_BIOS) && !defined(ARCH_CPM_BDOS)
    asm {
        pop  hl
        ld   sp, hl
    }
#endif
    return;
