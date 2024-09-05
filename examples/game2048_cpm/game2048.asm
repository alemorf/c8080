    device zxspectrum48 ; It has nothing to do with ZX Spectrum 48K, it is needed for the sjasmplus compiler.
    org 100h
__begin:
__entry:
main:
; 309  main(int argc, char *argv[]) {
	ld (__a_2_main), hl

		ld sp, 9000h
		ld hl, 0
		push hl
    
; 310     asm {
; 311 		ld sp, 9000h
; 312 		ld hl, 0
; 313 		push hl
; 314     }
; 315 
; 316     char c;
; 317     bool success;
; 318 
; 319     // TODO: if (argc == 2 && strcmp(argv[1],"test")==0) {
; 320     // TODO: 	return test();
; 321     // TODO: }
; 322 
; 323     initBoard();
	call initboard
l_0:
; 324     while (true) {
; 325         c = getchar();
	call getchar
	ld a, l
	ld (__s_main + 0), a
; 326         if (c == 27) {
	cp 27
	jp nz, l_2
; 327             c = getchar();
	call getchar
	ld a, l
	ld (__s_main + 0), a
; 328             if (c == '[') {
	cp 91
	jp nz, l_4
; 329                 c = getchar();
	call getchar
	ld a, l
	ld (__s_main + 0), a
; 330                 switch (c) {
	sub 65
	jp z, l_10
	dec a
	jp z, l_9
	dec a
	jp z, l_7
	dec a
	jp z, l_8
	jp l_0
l_10:
; 331                     case 'A':
; 332                         c = 'W';
	ld a, 87
	ld (__s_main + 0), a
	jp l_6
l_9:
; 333                         break;
; 334                     case 'B':
; 335                         c = 'S';
	ld a, 83
	ld (__s_main + 0), a
	jp l_6
l_8:
; 336                         break;
; 337                     case 'D':
; 338                         c = 'A';
	ld a, 65
	ld (__s_main + 0), a
	jp l_6
l_7:
; 339                         break;
; 340                     case 'C':
; 341                         c = 'D';
	ld a, 68
	ld (__s_main + 0), a
	jp l_6
l_6:
l_4:
l_2:
; 342                         break;
; 343                     default:
; 344                         continue;
; 345                 }
; 346             }
; 347         }
; 348         switch (c) {
	sub 65
	jp z, l_19
	sub 3
	jp z, l_17
	sub 15
	jp z, l_13
	sub 4
	jp z, l_15
	sub 10
	jp z, l_20
	sub 3
	jp z, l_18
	sub 15
	jp z, l_14
	sub 4
	jp z, l_16
	jp l_21
l_20:
l_19:
; 349             case 'a':
; 350             case 'A':
; 351                 success = moveLeft();
	call moveleft
	ld (__s_main + 1), a
	jp l_12
l_18:
l_17:
; 352                 break;
; 353             case 'd':
; 354             case 'D':
; 355                 success = moveRight();
	call moveright
	ld (__s_main + 1), a
	jp l_12
l_16:
l_15:
; 356                 break;
; 357             case 'w':
; 358             case 'W':
; 359                 success = moveUp();
	call moveup
	ld (__s_main + 1), a
	jp l_12
l_14:
l_13:
; 360                 break;
; 361             case 's':
; 362             case 'S':
; 363                 success = moveDown();
	call movedown
	ld (__s_main + 1), a
	jp l_12
l_21:
; 364                 break;
; 365             default:
; 366                 success = false;
; 24 ;
	xor a
	ld (__s_main + 1), a
l_12:
; 368 ) {
	or a
	jp z, l_22
; 369             drawBoard();
	call drawboard
; 370             sleep(2);
	ld hl, 2
	call sleep
; 371             addRandom();
	call addrandom
; 372             drawBoard();
	call drawboard
; 373             if (gameEnded()) {
	call gameended
	or a
	jp z, l_24
; 374                 printf("            GAME OVER          \n");
	ld hl, __c_18
	push hl
	call printf
	pop bc
	jp l_1
l_24:
l_22:
; 375                 break;
; 376             }
; 377         }
; 378         if (c == 'q' || c == 'Q') {
	ld a, (__s_main + 0)
	cp 113
	jp z, l_28
	cp 81
	jp nz, l_26
l_28:
; 379             printf("            QUIT? (y/n)         \n");
	ld hl, __c_19
	push hl
	call printf
	pop bc
; 380             c = getchar();
	call getchar
	ld a, l
	ld (__s_main + 0), a
; 381             if (c == 'y' || c == 'Y') {
	cp 121
	jp z, l_1
	cp 89
	jp z, l_1
; 382                 break;
; 383             }
; 384             prepareScreen();
	call preparescreen
l_26:
; 385         }
; 386         if (c == 'r' || c == 'R') {
	ld a, (__s_main + 0)
	cp 114
	jp z, l_34
	cp 82
	jp nz, l_0
l_34:
; 387             printf("          RESTART? (y/n)       \n");
	ld hl, __c_20
	push hl
	call printf
	pop bc
; 388             c = getchar();
	call getchar
	ld a, l
	ld (__s_main + 0), a
; 389             if (c == 'y' || c == 'Y') {
	cp 121
	jp z, l_37
	cp 89
	jp nz, l_35
l_37:
; 390                 initBoard();
	call initboard
l_35:
; 391             }
; 392             prepareScreen();
	call preparescreen
	jp l_0
l_1:
; 393         }
; 394     }
; 395 
; 396     printf(ESC_CLEAR_SCREEN);
	ld hl, __c_7
	push hl
	call printf
	pop bc
; 397 
; 398     return EXIT_SUCCESS;
	ld hl, 0
	ret
initboard:
; 244  initBoard() {
; 245     uint8_t x, y;
; 246     for (x = 0; x < SIZE; x++) {
	xor a
	ld (__s_initboard + 0), a
l_38:
	ld a, (__s_initboard + 0)
	cp 4
	jp nc, l_40
; 247         for (y = 0; y < SIZE; y++) {
	xor a
	ld (__s_initboard + 1), a
l_41:
	ld a, (__s_initboard + 1)
	cp 4
	jp nc, l_43
; 248             board[x][y] = 0;
	ld hl, (__s_initboard + 0)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_initboard + 1)
	ld h, 0
	add hl, de
	ld (hl), 0
l_42:
	ld a, (__s_initboard + 1)
	inc a
	ld (__s_initboard + 1), a
	jp l_41
l_43:
l_39:
	ld a, (__s_initboard + 0)
	inc a
	ld (__s_initboard + 0), a
	jp l_38
l_40:
; 249         }
; 250     }
; 251     addRandom();
	call addrandom
; 252     addRandom();
	call addrandom
; 253     prepareScreen();
	call preparescreen
; 254     score = 0;
	ld de, 0
	ld hl, 0
	ld (score), hl
	ex hl, de
	ld ((score) + 2), hl
; 255     drawBoard();
	jp drawboard
getchar:
; 40  getchar() {
; 41     return cpmBiosConIn();
	call cpmbiosconin
	ld l, a
	ld h, 0
	ret
moveleft:
; 144  moveLeft() {
; 145     bool success;
; 146     rotateBoard();
	call rotateboard
; 147     success = moveUp();
	call moveup
	ld (__s_moveleft + 0), a
; 148     rotateBoard();
	call rotateboard
; 149     rotateBoard();
	call rotateboard
; 150     rotateBoard();
	call rotateboard
; 151     return success;
	ld a, (__s_moveleft + 0)
	ret
moveright:
; 164  moveRight() {
; 165     bool success;
; 166     rotateBoard();
	call rotateboard
; 167     rotateBoard();
	call rotateboard
; 168     rotateBoard();
	call rotateboard
; 169     success = moveUp();
	call moveup
	ld (__s_moveright + 0), a
; 170     rotateBoard();
	call rotateboard
; 171     return success;
	ld a, (__s_moveright + 0)
	ret
moveup:
; 135  moveUp() {
; 136     bool success = false;
; 24 ;
	xor a
	ld (__s_moveup + 0), a
; 138  = 0; x < SIZE; x++) {
	ld (__s_moveup + 1), a
l_44:
	ld a, (__s_moveup + 1)
	cp 4
	jp nc, l_46
; 139         success |= slideArray(board[x]);
	ld hl, (__s_moveup + 1)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	call slidearray
	ld hl, __s_moveup + 0
	or (hl)
	ld (__s_moveup + 0), a
l_45:
	ld a, (__s_moveup + 1)
	inc a
	ld (__s_moveup + 1), a
	jp l_44
l_46:
; 140     }
; 141     return success;
	ld a, (__s_moveup + 0)
	ret
movedown:
; 154  moveDown() {
; 155     bool success;
; 156     rotateBoard();
	call rotateboard
; 157     rotateBoard();
	call rotateboard
; 158     success = moveUp();
	call moveup
	ld (__s_movedown + 0), a
; 159     rotateBoard();
	call rotateboard
; 160     rotateBoard();
	call rotateboard
; 161     return success;
	ld a, (__s_movedown + 0)
	ret
drawboard:
; 25  drawBoard() {
; 26     uint8_t x, y;
; 27     uint8_t t;
; 28     printf(ESC_HOME_CURSOR);
	ld hl, __c_0
	push hl
	call printf
	pop bc
; 29 
; 30     printf("2048.c %23d pts", score);
	ld hl, (((score) + 2))
	ex hl, de
	ld hl, (score)
	push de
	push hl
	ld hl, __c_1
	push hl
	call printf
	pop bc
	pop bc
	pop bc
; 31     for (y = 0; y < SIZE; y++) {
	xor a
	ld (__s_drawboard + 1), a
l_47:
	ld a, (__s_drawboard + 1)
	cp 4
	jp nc, l_49
; 32         printf("\n\n\n\n");
	ld hl, __c_2
	push hl
	call printf
	pop bc
; 33         for (x = 0; x < SIZE; x++) {
	xor a
	ld (__s_drawboard + 0), a
l_50:
	ld a, (__s_drawboard + 0)
	cp 4
	jp nc, l_52
; 34             if (board[x][y] != 0) {
	ld hl, (__s_drawboard + 0)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_drawboard + 1)
	ld h, 0
	add hl, de
	ld a, (hl)
	or a
	jp z, l_53
; 35                 char s[8];
; 36                 snprintf(s, 8, "%u", 1 << board[x][y]);
	ld hl, (__s_drawboard + 0)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_drawboard + 1)
	ld h, 0
	add hl, de
	ld l, (hl)
	ld h, 0
	ld de, 1
	ex hl, de
	call __o_shl_16
	push hl
	ld hl, __c_3
	push hl
	ld hl, 8
	push hl
	ld hl, __s_drawboard + 3
	push hl
	call snprintf
	ld hl, 8
	add hl, sp
	ld sp, hl
; 37                 t = 7 - strlen(s);
	ld hl, __s_drawboard + 3
	call strlen
	ld a, l
	ld d, a
	ld a, 7
	sub d
	ld (__s_drawboard + 2), a
; 38                 printf("|%*s%s%*s", t - t / 2, "", s, t / 2, "");
	ld hl, __c_5
	push hl
	ld hl, (__s_drawboard + 2)
	ld h, 0
	ld de, 1
	call __o_shr_u16
	push hl
	ld hl, __s_drawboard + 3
	push hl
	ld hl, __c_5
	push hl
	ld hl, (__s_drawboard + 2)
	ld h, 0
	ld de, 1
	call __o_shr_u16
	ex hl, de
	ld hl, (__s_drawboard + 2)
	ld h, 0
	call __o_sub_16
	push hl
	ld hl, __c_4
	push hl
	call printf
	ld hl, 12
	add hl, sp
	ld sp, hl
	jp l_54
l_53:
; 39             } else {
; 40                 printf("|       ");
	ld hl, __c_6
	push hl
	call printf
	pop bc
l_54:
l_51:
	ld a, (__s_drawboard + 0)
	inc a
	ld (__s_drawboard + 0), a
	jp l_50
l_52:
l_48:
	ld a, (__s_drawboard + 1)
	inc a
	ld (__s_drawboard + 1), a
	jp l_47
l_49:
; 41             }
; 42         }
; 43     }
; 44     printf("\n\n\n\n");
	ld hl, __c_2
	push hl
	call printf
	pop bc
	ret
sleep:
; 25  sleep(unsigned seconds) {
	ld (__a_1_sleep), hl
l_55:
; 26     while (seconds != 0) {
	ld hl, (__a_1_sleep)
	ld a, h
	or l
	jp z, l_56
; 27         seconds--;
	dec hl
	ld (__a_1_sleep), hl
; 28         Delay(C8080_SECOND_DELAY);
	ld hl, 5000
	call delay
	jp l_55
l_56:
; 29     }
; 30     return 0;
	ld hl, 0
	ret
addrandom:
; 214  addRandom() {
; 215     static bool initialized = false;
; 216     uint8_t x, y;
; 217     uint8_t r, len = 0;
	xor a
	ld (__s_addrandom + 3), a
; 218     uint8_t n, list[SIZE * SIZE][2];
; 219 
; 220     if (!initialized) {
	ld a, (__s_addrandom_initialized)
	or a
	jp nz, l_57
; 221         // srand(time(NULL));
; 222         initialized = true;
; 25 ;
	ld a, 1
	ld (__s_addrandom_initialized), a
l_57:
; 225  = 0; x < SIZE; x++) {
	xor a
	ld (__s_addrandom + 0), a
l_59:
	ld a, (__s_addrandom + 0)
	cp 4
	jp nc, l_61
; 226         for (y = 0; y < SIZE; y++) {
	xor a
	ld (__s_addrandom + 1), a
l_62:
	ld a, (__s_addrandom + 1)
	cp 4
	jp nc, l_64
; 227             if (board[x][y] == 0) {
	ld hl, (__s_addrandom + 0)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_addrandom + 1)
	ld h, 0
	add hl, de
	ld a, (hl)
	or a
	jp nz, l_65
; 228                 list[len][0] = x;
	ld a, (__s_addrandom + 0)
	ld hl, (__s_addrandom + 3)
	ld h, 0
	add hl, hl
	ld de, __s_addrandom + 5
	add hl, de
	ld (hl), a
; 229                 list[len][1] = y;
	ld a, (__s_addrandom + 1)
	ld hl, (__s_addrandom + 3)
	ld h, 0
	add hl, hl
	add hl, de
	inc hl
	ld (hl), a
; 230                 len++;
	ld a, (__s_addrandom + 3)
	inc a
	ld (__s_addrandom + 3), a
l_65:
l_63:
	ld a, (__s_addrandom + 1)
	inc a
	ld (__s_addrandom + 1), a
	jp l_62
l_64:
l_60:
	ld a, (__s_addrandom + 0)
	inc a
	ld (__s_addrandom + 0), a
	jp l_59
l_61:
; 231             }
; 232         }
; 233     }
; 234 
; 235     if (len > 0) {
	ld a, (__s_addrandom + 3)
	or a
	ret z
; 236         r = rand() % len;
	call rand
	ld l, a
	ld h, 0
	ex hl, de
	ld hl, (__s_addrandom + 3)
	ld h, 0
	ex hl, de
	call __o_mod_u16
	ld a, l
	ld (__s_addrandom + 2), a
; 237         x = list[r][0];
	ld hl, (__s_addrandom + 2)
	ld h, 0
	add hl, hl
	ld de, __s_addrandom + 5
	add hl, de
	ld a, (hl)
	ld (__s_addrandom + 0), a
; 238         y = list[r][1];
	ld hl, (__s_addrandom + 2)
	ld h, 0
	add hl, hl
	add hl, de
	inc hl
	ld a, (hl)
	ld (__s_addrandom + 1), a
; 239         n = (rand() % 10) / 9 + 1;
	call rand
	ld l, a
	ld h, 0
	ld de, 10
	call __o_mod_u16
	ld de, 9
	call __o_div_u16
	ld a, l
	inc a
	ld (__s_addrandom + 4), a
; 240         board[x][y] = n;
	ld hl, (__s_addrandom + 0)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_addrandom + 1)
	ld h, 0
	add hl, de
	ld (hl), a
	ret
gameended:
; 199  gameEnded() {
; 200     bool ended = true;
; 25 ;
	ld a, 1
	ld (__s_gameended + 0), a
; 201 ) > 0)
	call countempty
	or a
	jp z, l_69
; 24 ;
	xor a
	ret
l_69:
; 203 ))
	call findpairdown
	or a
	jp z, l_71
; 24 ;
	xor a
	ret
l_71:
; 205 );
	call rotateboard
; 206     if (findPairDown())
	call findpairdown
	or a
	jp z, l_73
; 207         ended = false;
; 24 ;
	xor a
	ld (__s_gameended + 0), a
l_73:
; 208 );
	call rotateboard
; 209     rotateBoard();
	call rotateboard
; 210     rotateBoard();
	call rotateboard
; 211     return ended;
	ld a, (__s_gameended + 0)
	ret
printf:
; 21  __stdcall printf(const char *format, ...) {
	push bc
; 22     va_list va;
; 23     va_start(va, format);
	ld hl, 6
	add hl, sp
	ex hl, de
	ld hl, 0
	add hl, sp
	ld (hl), e
	inc hl
	ld (hl), d
; 24     __printf_out_pointer = NULL;
	ld hl, 0
	ld (__printf_out_pointer), hl
; 25     __printf(format, va);
	ld hl, 4
	add hl, sp
	ld e, (hl)
	inc hl
	ld d, (hl)
	ex hl, de
	ld (__a_1___printf), hl
	ld hl, 0
	add hl, sp
	ld e, (hl)
	inc hl
	ld d, (hl)
	ex hl, de
	call __printf
; 26     va_end(va);
; 27     return __printf_out_total;
	ld hl, (__printf_out_total)
	pop bc
	ret
preparescreen:
; 47  prepareScreen() {
; 48     uint8_t x, y, i;
; 49     printf(ESC_CLEAR_SCREEN);
	ld hl, __c_7
	push hl
	call printf
	pop bc
; 50 
; 51     printf("2048.c %17d pts\n\n", score);
	ld hl, (((score) + 2))
	ex hl, de
	ld hl, (score)
	push de
	push hl
	ld hl, __c_8
	push hl
	call printf
	pop bc
	pop bc
	pop bc
; 52     for (x = 0; x < SIZE; x++)
	xor a
	ld (__s_preparescreen + 0), a
l_76:
	ld a, (__s_preparescreen + 0)
	cp 4
	jp nc, l_78
; 53         printf("+-------");
	ld hl, __c_9
	push hl
	call printf
	pop bc
l_77:
	ld a, (__s_preparescreen + 0)
	inc a
	ld (__s_preparescreen + 0), a
	jp l_76
l_78:
; 54     printf("+\n");
	ld hl, __c_10
	push hl
	call printf
	pop bc
; 55     for (y = 0; y < SIZE; y++) {
	xor a
	ld (__s_preparescreen + 1), a
l_79:
	ld a, (__s_preparescreen + 1)
	cp 4
	jp nc, l_81
; 56         for (i = 0; i < 3; i++) {
	xor a
	ld (__s_preparescreen + 2), a
l_82:
	ld a, (__s_preparescreen + 2)
	cp 3
	jp nc, l_84
; 57             for (x = 0; x < SIZE; x++)
	xor a
	ld (__s_preparescreen + 0), a
l_85:
	ld a, (__s_preparescreen + 0)
	cp 4
	jp nc, l_87
; 58                 printf("|       ");
	ld hl, __c_6
	push hl
	call printf
	pop bc
l_86:
	ld a, (__s_preparescreen + 0)
	inc a
	ld (__s_preparescreen + 0), a
	jp l_85
l_87:
; 59             printf("|\n");
	ld hl, __c_11
	push hl
	call printf
	pop bc
l_83:
	ld a, (__s_preparescreen + 2)
	inc a
	ld (__s_preparescreen + 2), a
	jp l_82
l_84:
; 60         }
; 61         for (x = 0; x < SIZE; x++)
	xor a
	ld (__s_preparescreen + 0), a
l_88:
	ld a, (__s_preparescreen + 0)
	cp 4
	jp nc, l_90
; 62             printf("+-------");
	ld hl, __c_9
	push hl
	call printf
	pop bc
l_89:
	ld a, (__s_preparescreen + 0)
	inc a
	ld (__s_preparescreen + 0), a
	jp l_88
l_90:
; 63         printf("+\n");
	ld hl, __c_10
	push hl
	call printf
	pop bc
l_80:
	ld a, (__s_preparescreen + 1)
	inc a
	ld (__s_preparescreen + 1), a
	jp l_79
l_81:
; 64     }
; 65     printf("\n          w,a,s,d or r,q       \n");
	ld hl, __c_12
	push hl
	call printf
	pop bc
; 66     drawBoard();
	jp drawboard
cpmbiosconin:
; 32  uint8_t __fastcall cpmBiosConIn() {

        ld hl, (1)
        ld l, 09h
        jp hl
    
	ret
rotateboard:
; 121  rotateBoard() {
; 122     uint8_t i, j, n = SIZE;
	ld a, 4
	ld (__s_rotateboard + 2), a
; 123     uint8_t tmp;
; 124     for (i = 0; i < n / 2; i++) {
	xor a
	ld (__s_rotateboard + 0), a
l_91:
	ld hl, (__s_rotateboard + 2)
	ld h, 0
	ld de, 1
	call __o_shr_u16
	ex hl, de
	ld hl, (__s_rotateboard + 0)
	ld h, 0
	call __o_sub_16
	ret nc
; 125         for (j = i; j < n - i - 1; j++) {
	ld a, (__s_rotateboard + 0)
	ld (__s_rotateboard + 1), a
l_94:
	ld hl, (__s_rotateboard + 0)
	ld h, 0
	ex hl, de
	ld hl, (__s_rotateboard + 2)
	ld h, 0
	call __o_sub_16
	dec hl
	ex hl, de
	ld hl, (__s_rotateboard + 1)
	ld h, 0
	call __o_sub_16
	jp nc, l_96
; 126             tmp = board[i][j];
	ld hl, (__s_rotateboard + 0)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_rotateboard + 1)
	ld h, 0
	add hl, de
	ld a, (hl)
	ld (__s_rotateboard + 3), a
; 127             board[i][j] = board[j][n - i - 1];
	ld hl, (__s_rotateboard + 0)
	ld h, 0
	ex hl, de
	ld hl, (__s_rotateboard + 2)
	ld h, 0
	call __o_sub_16
	dec hl
	push hl
	ld hl, (__s_rotateboard + 1)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	pop de
	add hl, de
	ld a, (hl)
	ld hl, (__s_rotateboard + 0)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_rotateboard + 1)
	ld h, 0
	add hl, de
	ld (hl), a
; 128             board[j][n - i - 1] = board[n - i - 1][n - j - 1];
	ld hl, (__s_rotateboard + 0)
	ld h, 0
	ex hl, de
	ld hl, (__s_rotateboard + 2)
	ld h, 0
	call __o_sub_16
	dec hl
	push hl
	ld hl, (__s_rotateboard + 1)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	pop de
	add hl, de
	push hl
	ld hl, (__s_rotateboard + 1)
	ld h, 0
	ex hl, de
	ld hl, (__s_rotateboard + 2)
	ld h, 0
	call __o_sub_16
	dec hl
	push hl
	ld hl, (__s_rotateboard + 0)
	ld h, 0
	ex hl, de
	ld hl, (__s_rotateboard + 2)
	ld h, 0
	call __o_sub_16
	dec hl
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	pop de
	add hl, de
	ld a, (hl)
	pop hl
	ld (hl), a
; 129             board[n - i - 1][n - j - 1] = board[n - j - 1][i];
	ld hl, (__s_rotateboard + 1)
	ld h, 0
	ex hl, de
	ld hl, (__s_rotateboard + 2)
	ld h, 0
	call __o_sub_16
	dec hl
	push hl
	ld hl, (__s_rotateboard + 0)
	ld h, 0
	ex hl, de
	ld hl, (__s_rotateboard + 2)
	ld h, 0
	call __o_sub_16
	dec hl
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	pop de
	add hl, de
	push hl
	ld hl, (__s_rotateboard + 1)
	ld h, 0
	ex hl, de
	ld hl, (__s_rotateboard + 2)
	ld h, 0
	call __o_sub_16
	dec hl
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_rotateboard + 0)
	ld h, 0
	add hl, de
	ld a, (hl)
	pop hl
	ld (hl), a
; 130             board[n - j - 1][i] = tmp;
	ld hl, (__s_rotateboard + 1)
	ld h, 0
	ex hl, de
	ld hl, (__s_rotateboard + 2)
	ld h, 0
	call __o_sub_16
	dec hl
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_rotateboard + 0)
	ld h, 0
	add hl, de
	ld a, (__s_rotateboard + 3)
	ld (hl), a
l_95:
	ld a, (__s_rotateboard + 1)
	inc a
	ld (__s_rotateboard + 1), a
	jp l_94
l_96:
l_92:
	ld a, (__s_rotateboard + 0)
	inc a
	ld (__s_rotateboard + 0), a
	jp l_91
slidearray:
; 93  slideArray(uint8_t array[SIZE]) {
	ld (__a_1_slidearray), hl
; 94     bool success = false;
; 24 ;
	xor a
	ld (__s_slidearray + 0), a
; 95  x, t, stop = 0;
	ld (__s_slidearray + 3), a
; 96 
; 97     for (x = 0; x < SIZE; x++) {
	ld (__s_slidearray + 1), a
l_97:
	ld a, (__s_slidearray + 1)
	cp 4
	jp nc, l_99
; 98         if (array[x] != 0) {
	ld hl, (__s_slidearray + 1)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_slidearray)
	add hl, de
	ld a, (hl)
	or a
	jp z, l_100
; 99             t = findTarget(array, x, stop);
	ld hl, (__a_1_slidearray)
	ld (__a_1_findtarget), hl
	ld a, (__s_slidearray + 1)
	ld (__a_2_findtarget), a
	ld a, (__s_slidearray + 3)
	call findtarget
	ld (__s_slidearray + 2), a
; 100             // if target is not original position, then move or merge
; 101             if (t != x) {
	ld a, (__s_slidearray + 1)
	ld hl, __s_slidearray + 2
	cp (hl)
	jp z, l_102
; 102                 // if target is zero, this is a move
; 103                 if (array[t] == 0) {
	ld hl, (__s_slidearray + 2)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_slidearray)
	add hl, de
	ld a, (hl)
	or a
	jp nz, l_104
; 104                     array[t] = array[x];
	ld hl, (__s_slidearray + 1)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_slidearray)
	add hl, de
	ld a, (hl)
	ld hl, (__s_slidearray + 2)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_slidearray)
	add hl, de
	ld (hl), a
	jp l_105
l_104:
; 105                 } else if (array[t] == array[x]) {
	ld hl, (__s_slidearray + 1)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_slidearray)
	add hl, de
	ld a, (hl)
	ld hl, (__s_slidearray + 2)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_slidearray)
	add hl, de
	cp (hl)
	jp nz, l_106
; 106                     // merge (increase power of two)
; 107                     array[t]++;
	ld hl, (__s_slidearray + 2)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_slidearray)
	add hl, de
	ld a, (hl)
	inc a
	ld (hl), a
; 108                     // increase score
; 109                     score += (uint32_t)1 << array[t];
	ld hl, (((score) + 2))
	ex hl, de
	ld hl, (score)
	push de
	push hl
	ld de, 0
	ld hl, 1
	push de
	push hl
	ld hl, (__s_slidearray + 2)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_slidearray)
	add hl, de
	ld l, (hl)
	ld de, 0
	ld h, d
	call __o_shl_32
	call __o_add_32
	ld (score), hl
	ex hl, de
	ld ((score) + 2), hl
; 110                     // set stop to avoid double merge
; 111                     stop = t + 1;
	ld a, (__s_slidearray + 2)
	inc a
	ld (__s_slidearray + 3), a
l_106:
l_105:
; 112                 }
; 113                 array[x] = 0;
	ld hl, (__s_slidearray + 1)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_slidearray)
	add hl, de
	ld (hl), 0
; 114                 success = true;
; 25 ;
	ld a, 1
	ld (__s_slidearray + 0), a
l_102:
l_100:
l_98:
; 97 ) {
	ld a, (__s_slidearray + 1)
	inc a
	ld (__s_slidearray + 1), a
	jp l_97
l_99:
; 98         if (array[x] != 0) {
; 99             t = findTarget(array, x, stop);
; 100             // if target is not original position, then move or merge
; 101             if (t != x) {
; 102                 // if target is zero, this is a move
; 103                 if (array[t] == 0) {
; 104                     array[t] = array[x];
; 105                 } else if (array[t] == array[x]) {
; 106                     // merge (increase power of two)
; 107                     array[t]++;
; 108                     // increase score
; 109                     score += (uint32_t)1 << array[t];
; 110                     // set stop to avoid double merge
; 111                     stop = t + 1;
; 112                 }
; 113                 array[x] = 0;
; 114                 success = true;
; 115             }
; 116         }
; 117     }
; 118     return success;
	ld a, (__s_slidearray + 0)
	ret
snprintf:
; 32  __stdcall snprintf(char *buffer, size_t buffer_size, const char *format, ...) {
	push bc
; 33     va_list va;
; 34     va_start(va, format);
	ld hl, 10
	add hl, sp
	ex hl, de
	ld hl, 0
	add hl, sp
	ld (hl), e
	inc hl
	ld (hl), d
; 35     __snprintf(buffer, buffer_size, format, va);
	ld hl, 4
	add hl, sp
	ld e, (hl)
	inc hl
	ld d, (hl)
	ex hl, de
	ld (__a_1___snprintf), hl
	ld hl, 6
	add hl, sp
	ld e, (hl)
	inc hl
	ld d, (hl)
	ex hl, de
	ld (__a_2___snprintf), hl
	ld hl, 8
	add hl, sp
	ld e, (hl)
	inc hl
	ld d, (hl)
	ex hl, de
	ld (__a_3___snprintf), hl
	ld hl, 0
	add hl, sp
	ld e, (hl)
	inc hl
	ld d, (hl)
	ex hl, de
	call __snprintf
; 36     va_end(va);
; 37     return __printf_out_total;
	ld hl, (__printf_out_total)
	pop bc
	ret
strlen:
; 20  __fastcall strlen(const char *string) {
	ld (__a_1_strlen), hl
; 21     (void)string;

        ld de, -1
        xor a
strlen_1:
        cp (hl)
        inc de
        inc hl
        jp nz, strlen_1
        ex hl, de
    
	ret
delay:
; 22  Delay(uint16_t n) {
	ld (__a_1_delay), hl
l_109:
; 23     while (--n != 0) {
	ld hl, (__a_1_delay)
	dec hl
	ld (__a_1_delay), hl
	ld a, h
	or l
	jp nz, l_109
	ret
rand:
; 22  rand() {
; 23     rand_seed *= 5;
	ld a, (rand_seed)
	ld d, a
	add a
	add a
	add d
; 24     rand_seed++;
	inc a
	ld (rand_seed), a
; 25     return rand_seed;
	ret
countempty:
; 186  countEmpty() {
; 187     uint8_t x, y;
; 188     uint8_t count = 0;
	xor a
	ld (__s_countempty + 2), a
; 189     for (x = 0; x < SIZE; x++) {
	ld (__s_countempty + 0), a
l_111:
	ld a, (__s_countempty + 0)
	cp 4
	jp nc, l_113
; 190         for (y = 0; y < SIZE; y++) {
	xor a
	ld (__s_countempty + 1), a
l_114:
	ld a, (__s_countempty + 1)
	cp 4
	jp nc, l_116
; 191             if (board[x][y] == 0) {
	ld hl, (__s_countempty + 0)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_countempty + 1)
	ld h, 0
	add hl, de
	ld a, (hl)
	or a
	jp nz, l_117
; 192                 count++;
	ld a, (__s_countempty + 2)
	inc a
	ld (__s_countempty + 2), a
l_117:
l_115:
	ld a, (__s_countempty + 1)
	inc a
	ld (__s_countempty + 1), a
	jp l_114
l_116:
l_112:
	ld a, (__s_countempty + 0)
	inc a
	ld (__s_countempty + 0), a
	jp l_111
l_113:
; 193             }
; 194         }
; 195     }
; 196     return count;
	ld a, (__s_countempty + 2)
	ret
findpairdown:
; 174  findPairDown() {
; 175     bool success = false;
; 24 ;
	xor a
	ld (__s_findpairdown + 0), a
; 177  = 0; x < SIZE; x++) {
	ld (__s_findpairdown + 1), a
l_119:
	ld a, (__s_findpairdown + 1)
	cp 4
	jp nc, l_121
; 178         for (y = 0; y < SIZE - 1; y++) {
	xor a
	ld (__s_findpairdown + 2), a
l_122:
	ld a, (__s_findpairdown + 2)
	cp 3
	jp nc, l_124
; 179             if (board[x][y] == board[x][y + 1])
	ld hl, (__s_findpairdown + 1)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_findpairdown + 2)
	ld h, 0
	inc hl
	add hl, de
	ld a, (hl)
	ld hl, (__s_findpairdown + 1)
	ld h, 0
	add hl, hl
	add hl, hl
	ld de, board
	add hl, de
	ex hl, de
	ld hl, (__s_findpairdown + 2)
	ld h, 0
	add hl, de
	cp (hl)
	jp nz, l_125
; 25 ;
	ld a, 1
	ret
l_125:
l_123:
; 178 ) {
	ld a, (__s_findpairdown + 2)
	inc a
	ld (__s_findpairdown + 2), a
	jp l_122
l_124:
l_120:
	ld a, (__s_findpairdown + 1)
	inc a
	ld (__s_findpairdown + 1), a
	jp l_119
l_121:
; 179             if (board[x][y] == board[x][y + 1])
; 180                 return true;
; 181         }
; 182     }
; 183     return success;
	ld a, (__s_findpairdown + 0)
	ret
__printf:
; 73  __printf(const char *format, va_list va) {
	ld (__a_2___printf), hl
; 74     __printf_out_total = 0;
	ld hl, 0
	ld (__printf_out_total), hl
l_127:
; 75     for (;;) {
; 76         uint8_t c = *format;
	ld hl, (__a_1___printf)
	ld a, (hl)
	ld (__s___printf + 0), a
; 77         switch (c) {
	or a
	ret z
	sub 37
	jp z, l_131
	jp l_133
l_131:
; 78             case '\0':
; 79                 return;
; 80             case '%':
; 81                 unsigned width = 0;
	ld hl, 0
	ld (__s___printf + 1), hl
; 82                 format++;
	ld hl, (__a_1___printf)
	inc hl
	ld (__a_1___printf), hl
; 83                 c = *format;
	ld a, (hl)
	ld (__s___printf + 0), a
; 84                 char fill_char = c == '0' ? '0' : ' ';
	cp 48
	jp nz, l_134
	ld hl, 48
	jp l_135
l_134:
	ld hl, 32
l_135:
	ld a, l
	ld (__s___printf + 3), a
; 85                 if (c == '*') {
	ld a, (__s___printf + 0)
	cp 42
	jp nz, l_136
; 86                     width = va_arg(va, unsigned);
	ld hl, (__a_2___printf)
	inc hl
	inc hl
	ld (__a_2___printf), hl
	dec hl
	dec hl
	ld e, (hl)
	inc hl
	ld d, (hl)
	ex hl, de
	ld (__s___printf + 1), hl
; 87                     format++;
	ld hl, (__a_1___printf)
	inc hl
	ld (__a_1___printf), hl
; 88                     c = *format;
	ld a, (hl)
	ld (__s___printf + 0), a
	jp l_137
l_136:
l_138:
; 89                 } else {
; 90                     while (c >= '0' && c <= '9') {
	ld a, (__s___printf + 0)
	cp 48
	jp c, l_139
	ld a, 57
	ld hl, __s___printf + 0
	cp (hl)
	jp c, l_139
; 91                         width = width * 10 + (c - '0');
	ld hl, (__s___printf + 0)
	ld h, 0
	ld de, 65488
	add hl, de
	push hl
	ld hl, (__s___printf + 1)
	ld d, h
	ld e, l
	add hl, hl
	add hl, hl
	add hl, de
	add hl, hl
	pop de
	add hl, de
	ld (__s___printf + 1), hl
; 92                         format++;
	ld hl, (__a_1___printf)
	inc hl
	ld (__a_1___printf), hl
; 93                         c = *format;
	ld a, (hl)
	ld (__s___printf + 0), a
	jp l_138
l_139:
l_137:
; 94                     }
; 95                 }
; 96 #ifndef C8080_PRINTF_NO_LONG
; 97                 uint8_t longMode = 0;
	xor a
	ld (__s___printf + 4), a
; 98                 if (c == 'l') {
	ld a, (__s___printf + 0)
	cp 108
	jp nz, l_140
; 99                     longMode = 1;
	ld a, 1
	ld (__s___printf + 4), a
; 100                     format++;
	ld hl, (__a_1___printf)
	inc hl
	ld (__a_1___printf), hl
; 101                     c = *format;
	ld a, (hl)
	ld (__s___printf + 0), a
l_140:
; 102                 }
; 103 #endif
; 104                 uint8_t radix = 10;
	ld a, 10
	ld (__s___printf + 5), a
; 105                 switch (c) {
	ld a, (__s___printf + 0)
	or a
	ret z
	sub 37
	jp z, l_144
	sub 51
	jp z, l_151
	sub 11
	jp z, l_146
	dec a
	jp z, l_148
	sub 5
	jp z, l_147
	sub 5
	jp z, l_145
	sub 2
	jp z, l_150
	sub 3
	jp z, l_143
	sub 2
	jp z, l_149
	sub 3
	jp z, l_152
	jp l_130
l_152:
l_151:
l_150:
l_149:
l_148:
l_147:
; 106                     case 0:
; 107                         return;
; 108                     // TODO: o  radix = 8
; 109                     case 'x':
; 110                     case 'X':
; 111                     case 'p':
; 112                     case 'u':
; 113                     case 'd':
; 114                     case 'i': {
; 115 #ifndef C8080_PRINTF_NO_LONG
; 116                         char buf[UINT32_TO_STRING_SIZE];
; 117                         uint32_t value32 = longMode ? va_arg(va, uint32_t) : va_arg(va, uint16_t);
	ld a, (__s___printf + 4)
	or a
	jp z, l_154
	ld hl, (__a_2___printf)
	ld de, 4
	add hl, de
	ld (__a_2___printf), hl
	ld de, 65532
	add hl, de
	call __o_load_32
	jp l_155
l_154:
	ld hl, (__a_2___printf)
	inc hl
	inc hl
	ld (__a_2___printf), hl
	dec hl
	dec hl
	ld e, (hl)
	inc hl
	ld d, (hl)
	ex hl, de
	ld de, 0
l_155:
	ld (__s___printf + 17), hl
	ex hl, de
	ld ((__s___printf + 17) + 2), hl
; 118                         if (c == 'i' || c == 'd') {
	ld a, (__s___printf + 0)
	cp 105
	jp z, l_158
	cp 100
	jp nz, l_156
l_158:
; 119                             if (!longMode) {
	ld a, (__s___printf + 4)
	or a
	jp nz, l_159
; 120                                 int16_t value = value32;  // TODO: OPTIMIZE
	ld hl, (__s___printf + 17)
	ld (__s___printf + 21), hl
; 121                                 value32 = value;
	call __o_i16_to_i32
	ld (__s___printf + 17), hl
	ex hl, de
	ld ((__s___printf + 17) + 2), hl
l_159:
; 122                             }
; 123                             if ((int32_t)value32 < 0) {
	ld hl, (((__s___printf + 17) + 2))
	ex hl, de
	push de
	push hl
	ld de, 0
	ld hl, 0
	call __o_sub_32
	jp p, l_161
; 124                                 __printf_out('-');
	ld a, 45
	call __printf_out
; 125                                 value32 = -value32;
	ld hl, (((__s___printf + 17) + 2))
	ex hl, de
	ld hl, (__s___printf + 17)
	call __o_minus_32
	ld (__s___printf + 17), hl
	ex hl, de
	ld ((__s___printf + 17) + 2), hl
l_161:
l_156:
; 126                             }
; 127                         }
; 128                         char *text = Uint32ToString(buf, value32, (c == 'x' || c == 'p') ? 16 : 10);
	ld hl, __s___printf + 6
	ld (__a_1_uint32tostring), hl
	ld hl, (((__s___printf + 17) + 2))
	ex hl, de
	ld hl, (__s___printf + 17)
	ld (__a_2_uint32tostring), hl
	ex hl, de
	ld ((__a_2_uint32tostring) + 2), hl
	ld a, (__s___printf + 0)
	cp 120
	jp z, l_165
	cp 112
	jp nz, l_163
l_165:
	ld hl, 16
	jp l_164
l_163:
	ld hl, 10
l_164:
	ld a, l
	call uint32tostring
	ld (__s___printf + 21), hl
; 129 #else
; 130                         char buf[UINT16_TO_STRING_SIZE];
; 131                         uint16_t value16 = va_arg(va, uint16_t);
; 132                         if ((c == 'i' || c == 'd') && (int16_t)value16 < 0) {
; 133                             __printf_out('-');
; 134                             value16 = 0 - value16;
; 135                         }
; 136                         char *text = Uint16ToString(buf, value16, (c == 'x' || c == 'p') ? 16 : 10);
; 137 #endif
; 138                         __printf_spaces(width, strlen(text), fill_char);
	ld hl, (__s___printf + 1)
	ld (__a_1___printf_spaces), hl
	ld hl, (__s___printf + 21)
	call strlen
	ld (__a_2___printf_spaces), hl
	ld a, (__s___printf + 3)
	call __printf_spaces
; 139                         __printf_text(text);
	ld hl, (__s___printf + 21)
	call __printf_text
	jp l_130
l_146:
; 140                         break;
; 141                     }
; 142                     case 'c': {
; 143                         __printf_out(va_arg(va, uint16_t) & 0xFF);
	ld hl, (__a_2___printf)
	inc hl
	inc hl
	ld (__a_2___printf), hl
	dec hl
	dec hl
	ld e, (hl)
	inc hl
	ld d, (hl)
	ex hl, de
	ld a, l
	and 0
	call __printf_out
	jp l_130
l_145:
; 144                         break;
; 145                     }
; 146                     case 'n': {
; 147                         size_t *out = va_arg(va, size_t *);
	ld hl, (__a_2___printf)
	inc hl
	inc hl
	ld (__a_2___printf), hl
	dec hl
	dec hl
	ld e, (hl)
	inc hl
	ld d, (hl)
	ex hl, de
	ld (__s___printf + 6), hl
; 148                         *out = __printf_out_total;
	ld hl, (__printf_out_total)
	ex hl, de
	ld hl, (__s___printf + 6)
	ld (hl), e
	inc hl
	ld (hl), d
	jp l_130
l_144:
; 149                         break;
; 150                     }
; 151                     case '%':
; 152                         __printf_out('%');
	ld a, 37
	call __printf_out
	jp l_130
l_143:
; 153                         break;
; 154                     case 's': {
; 155                         uint16_t prevTotal = __printf_out_total;
	ld hl, (__printf_out_total)
	ld (__s___printf + 6), hl
; 156                         __printf_text(va_arg(va, char *));
	ld hl, (__a_2___printf)
	inc hl
	inc hl
	ld (__a_2___printf), hl
	dec hl
	dec hl
	ld e, (hl)
	inc hl
	ld d, (hl)
	ex hl, de
	call __printf_text
; 157                         __printf_spaces(width, __printf_out_total - prevTotal, ' ');
	ld hl, (__s___printf + 1)
	ld (__a_1___printf_spaces), hl
	ld hl, (__s___printf + 6)
	ex hl, de
	ld hl, (__printf_out_total)
	call __o_sub_16
	ld (__a_2___printf_spaces), hl
	ld a, 32
	call __printf_spaces
	jp l_130
l_133:
; 158                         break;
; 159                     }
; 160                 }
; 161                 break;
; 162             default:
; 163                 __printf_out(c);
	ld a, (__s___printf + 0)
	call __printf_out
l_130:
; 164         }
; 165         format++;
	ld hl, (__a_1___printf)
	inc hl
	ld (__a_1___printf), hl
l_128:
	jp l_127
l_129:
	ret
findtarget:
; 69  findTarget(uint8_t array[SIZE], uint8_t x, uint8_t stop) {
	ld (__a_3_findtarget), a
; 70     uint8_t t;
; 71     // if the position is already on the first, don't evaluate
; 72     if (x == 0) {
	ld a, (__a_2_findtarget)
	or a
	jp nz, l_166
; 73         return x;
	ret
l_166:
; 74     }
; 75     for (t = x - 1;; t--) {
	dec a
	ld (__s_findtarget + 0), a
l_168:
; 76         if (array[t] != 0) {
	ld hl, (__s_findtarget + 0)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_findtarget)
	add hl, de
	ld a, (hl)
	or a
	jp z, l_171
; 77             if (array[t] != array[x]) {
	ld hl, (__a_2_findtarget)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_findtarget)
	add hl, de
	ld a, (hl)
	ld hl, (__s_findtarget + 0)
	ld h, 0
	ex hl, de
	ld hl, (__a_1_findtarget)
	add hl, de
	cp (hl)
	jp z, l_173
; 78                 // merge is not possible, take next position
; 79                 return t + 1;
	ld a, (__s_findtarget + 0)
	inc a
	ret
l_173:
; 80             }
; 81             return t;
	ld a, (__s_findtarget + 0)
	ret
l_171:
; 82         } else {
; 83             // we should not slide further, return this one
; 84             if (t == stop) {
	ld a, (__a_3_findtarget)
	ld hl, __s_findtarget + 0
	cp (hl)
	jp nz, l_175
; 85                 return t;
	ld a, (__s_findtarget + 0)
	ret
l_175:
l_169:
	ld a, (__s_findtarget + 0)
	dec a
	ld (__s_findtarget + 0), a
	jp l_168
l_170:
; 86             }
; 87         }
; 88     }
; 89     // we did not find a
; 90     return x;
	ld a, (__a_2_findtarget)
	ret
__snprintf:
; 21  void __snprintf(char *buffer, size_t buffer_size, const char *format, va_list va) {
	ld (__a_4___snprintf), hl
; 22     __printf_out_pointer = buffer;
	ld hl, (__a_1___snprintf)
	ld (__printf_out_pointer), hl
; 23     size_t s = buffer_size;
	ld hl, (__a_2___snprintf)
	ld (__s___snprintf + 0), hl
; 24     if (s > 0)
	ld a, h
	or l
	jp z, l_177
; 25         s--;
	dec hl
	ld (__s___snprintf + 0), hl
l_177:
; 26     __printf_out_end = buffer + s;
	ex hl, de
	ld hl, (__a_1___snprintf)
	add hl, de
	ld (__printf_out_end), hl
; 27     __printf(format, va);
	ld hl, (__a_3___snprintf)
	ld (__a_1___printf), hl
	ld hl, (__a_4___snprintf)
	call __printf
; 28     if (buffer_size != 0)
	ld hl, (__a_2___snprintf)
	ld a, h
	or l
	ret z
; 29         *__printf_out_pointer = '\0';
	ld hl, (__printf_out_pointer)
	ld (hl), 0
	ret
__printf_out:
	ld (__a_1___printf_out), a
; 30 ;
	ld hl, (__printf_out_total)
	inc hl
	ld (__printf_out_total), hl
; 31     if (__printf_out_pointer == NULL) {
	ld hl, (__printf_out_pointer)
	ld a, h
	or l
	jp nz, l_181
; 32         putchar((uint8_t)c);
	ld a, (__a_1___printf_out)
	call __o_i8_to_i16
	jp putchar
l_181:
; 33         return;
; 34     }
; 35     if (__printf_out_pointer == __printf_out_end)
	ld hl, (__printf_out_end)
	ex hl, de
	ld hl, (__printf_out_pointer)
	call __o_xor_16
	ret z
; 36         return;
; 37     *__printf_out_pointer = c;
	ld a, (__a_1___printf_out)
	ld hl, (__printf_out_pointer)
	ld (hl), a
; 38     __printf_out_pointer++;
	inc hl
	ld (__printf_out_pointer), hl
	ret
uint32tostring:
; 21  *Uint32ToString(char *outputBuffer, uint32_t value, uint8_t radix) {
	ld (__a_3_uint32tostring), a
; 22     outputBuffer += UINT32_TO_STRING_SIZE - 1;
	ld hl, (__a_1_uint32tostring)
	ld de, 10
	add hl, de
	ld (__a_1_uint32tostring), hl
; 23     *outputBuffer = 0;
	ld (hl), 0
l_185:
; 24     do {
; 25         value /= radix;
	ld hl, (((__a_2_uint32tostring) + 2))
	ex hl, de
	ld hl, (__a_2_uint32tostring)
	push de
	push hl
	ld de, 0
	ld hl, (__a_3_uint32tostring)
	ld h, d
	call __o_div_u32
	ld (__a_2_uint32tostring), hl
	ex hl, de
	ld ((__a_2_uint32tostring) + 2), hl
; 26         --outputBuffer;
	ld hl, (__a_1_uint32tostring)
	dec hl
	ld (__a_1_uint32tostring), hl
; 27         char c = (char)__div_32_mod + '0';
	ld a, (__div_32_mod)
	add 48
	ld (__s_uint32tostring + 0), a
; 28         if (c > '9') c += 'A' - '0' - 10;
	cp 57
	jp m, l_188
	add 7
	ld (__s_uint32tostring + 0), a
l_188:
; 29         *outputBuffer = c;
	ld (hl), a
l_186:
; 30     } while (value != 0);
	ld hl, (((__a_2_uint32tostring) + 2))
	ex hl, de
	ld hl, (__a_2_uint32tostring)
	ld a, l
	or h
	or d
	or e
	jp nz, l_185
l_187:
; 31     return outputBuffer;
	ld hl, (__a_1_uint32tostring)
	ret
__printf_spaces:
; 51  void __printf_spaces(unsigned need, unsigned ready, char fill_char) {
	ld (__a_3___printf_spaces), a
; 52     if (ready >= need)
	ld hl, (__a_1___printf_spaces)
	ex hl, de
	ld hl, (__a_2___printf_spaces)
	call __o_sub_16
	ret nc
; 53         return;
; 54     need -= ready;
	ld hl, (__a_2___printf_spaces)
	ex hl, de
	ld hl, (__a_1___printf_spaces)
	call __o_sub_16
	ld (__a_1___printf_spaces), hl
l_192:
; 55     do {
; 56         __printf_out(fill_char);
	ld a, (__a_3___printf_spaces)
	call __printf_out
; 57         need--;
	ld hl, (__a_1___printf_spaces)
	dec hl
	ld (__a_1___printf_spaces), hl
l_193:
; 58     } while (need != 0);
	ld hl, (__a_1___printf_spaces)
	ld a, h
	or l
	jp nz, l_192
l_194:
	ret
__printf_text:
; 41  void __printf_text(const char *text) {
	ld (__a_1___printf_text), hl
l_195:
; 42     for (;;) {
; 43         uint8_t c = *text;
	ld hl, (__a_1___printf_text)
	ld a, (hl)
	ld (__s___printf_text + 0), a
; 44         if (c == '\0')
	or a
	ret z
; 45             break;
; 46         __printf_out(c);
	call __printf_out
; 47         text++;
	ld hl, (__a_1___printf_text)
	inc hl
	ld (__a_1___printf_text), hl
l_196:
	jp l_195
putchar:
; 45  putchar(int c) {
	ld (__a_1_putchar), hl
; 46     if (c == 0x0A)
	ld de, 10
	call __o_xor_16
	jp nz, l_200
; 47         cpmBiosConOut(0x0D);
	ld a, 13
	call cpmbiosconout
l_200:
; 48     cpmBiosConOut(c);
	ld a, (__a_1_putchar)
	call cpmbiosconout
; 49     return 0;
	ld hl, 0
	ret
cpmbiosconout:
; 36  void __fastcall cpmBiosConOut(uint8_t c) {
	ld (__a_1_cpmbiosconout), a

        ld c, a
        ld hl, (1)
        ld l, 0Ch
        jp hl
    
	ret
__o_sub_16:
    ld a, l
    sub e
    ld l, a
    ld a, h
    sbc d
    ld h, a
    ret
; Input: hl - value 1, de - value 2
; Output: hl - result

__o_xor_16:
    ld a, h
    xor d
    ld h, a
    ld a, l
    xor e
    ld l, a
    or h     ; Flag Z used for compare
    ret
__div_16_mod dw 0

__o_div_u16:
    push bc
    ex hl, de
    call __o_div_u16__l0
    ex hl, de
    ld (__div_16_mod), hl
    ex hl, de
    pop bc
    ret

__o_div_u16__l0:
__o_div_u16__l:
    ld a, h
    or l
    ret z
    ld bc, 0
    push bc
__o_div_u16__l1:
    ld a, e
    sub l
    ld a, d
    sbc h
    jp c, __o_div_u16__l2
    push hl
    add hl, hl
    jp nc, __o_div_u16__l1
__o_div_u16__l2:
    ld hl, 0
__o_div_u16__l3:
    pop bc
    ld a, b
    or c
    ret z
    add hl, hl
    push de
    ld a, e
    sub c
    ld e, a
    ld a, d
    sbc b
    ld d, a
    jp c, __o_div_u16__l4
    inc hl
    pop bc
    jp __o_div_u16__l3
__o_div_u16__l4:
    pop de
    jp __o_div_u16__l3
__o_mod_u16:
    push bc
    ex hl, de
    call __o_div_u16__l0
    ex hl, de
    pop bc
    ret
__o_shl_16:
    inc e
__o_shl_16__l1:
    dec e
    ret z
    add hl, hl
    jp __o_shl_16__l1
__o_shr_u16:
    inc e
__o_shr_u16__l1:
    dec e
    ret z
    ld a, h
    or a  ; cf = 0
    rra
    ld h, a
    ld a, l
    rra
    ld l, a
    jp __o_shr_u16__l1
; Input: de:hl - value 1, stack - 32 bit value 2
; Output: de:hl - result

__o_add_32:
    ld bc, hl ; bc = v1l
    pop hl ; hl = ret, stack = v2l
    ex (sp), hl ; hl = v2l, stack = ret
    ld a, c
    add l
    ld c, a
    ld a, b
    adc h
    ld b, a ; bc - result
    pop hl ; hl = ret, stack = v2h
    ex (sp), hl ; hl = v2h, stack = ret
    ld a, e
    adc l
    ld e, a
    ld a, d
    adc h
    ld d, a ; de - result
    ld hl, bc
    ret
; Input: de:hl - value 1, stack - 32 bit value 2
; Output: de:hl - result

__o_sub_32:
    ld bc, hl ; bc = v1l
    pop hl ; hl = ret, stack = v2l
    ex (sp), hl ; hl = v2l, stack = ret
    ld a, l
    sub c
    ld c, a
    ld a, h
    sbc b
    ld b, a ; bc - result
    pop hl ; hl = ret, stack = v2h
    ex (sp), hl ; hl = v2h, stack = ret
    ld a, l
    sbc e
    ld e, a
    ld a, h
    sbc d
    ld d, a ; de - result
    ld hl, bc
    ret
__o_i8_to_i16:
    ld l, a
    rla
    sbc a
    ld h, a
    ret
__o_i16_to_i32:
    ld de, 0
    ld a, h
    and 80h
    ret z
    dec de
    ret; Input: hl - address
; Output: de:hl - result

__o_load_32:
    ld c, (hl)
    inc hl
    ld b, (hl)
    inc hl
    ld e, (hl)
    inc hl
    ld d, (hl)
    ld hl, bc
    ret
__o_div_u32__result dw 0
__div_32_mod dd 0

__o_div_u32:
    ld bc, hl                    ; __div_32_mod = a
    pop hl
    ex (sp), hl
    ld (__div_32_mod+0), hl
    pop hl
    ex (sp), hl
    ld (__div_32_mod+2), hl
    ld hl, __o_div_u32__ret
__o_div_u32__com:
    ld (__o_div_u32__ra), hl
    ld hl, bc
    ld a, h                      ; if (b == 0) return;
    or l
    or d
    or e
    ret z
    ld c, 1                      ; c = 1;
__o_div_u32__l1:                 ; do
    ld a, (__div_32_mod+0)       ; if (a < b) break;
    sub l
    ld a, (__div_32_mod+1)
    sbc h
    ld a, (__div_32_mod+2)
    sbc e
    ld a, (__div_32_mod+3)
    sbc d
    jp c, __o_div_u32__l2
    inc c
    push hl                      ; push(b);
    push de
    call __o_div_u32__shl_dehl   ; b <<= 1;
    jp nc, __o_div_u32__l1       ; } while(flag_nc);
__o_div_u32__l2:
    ld a, c
    ld (__o_div_u32__lc), a
    ld hl, 0                     ; result = 0
    ld (__o_div_u32__result), hl
    ld de, hl
__o_div_u32__l4:                 ; while(--c != 0) {
__o_div_u32__lc=$+1
    ld a, 0
    dec a
__o_div_u32__ra=$+1
    jp z, __o_div_u32__ret
    ld (__o_div_u32__lc), a

    ld hl, (__o_div_u32__result) ; result <<= 1
    call __o_div_u32__shl_dehl
    ld (__o_div_u32__result), hl

    pop bc                       ; pop(x)
    pop hl

    ld a, (__div_32_mod+0)       ; if (x < __div_32_mod) continue;
    sub l
    ld a, (__div_32_mod+1)
    sbc h
    ld a, (__div_32_mod+2)
    sbc c
    ld a, (__div_32_mod+3)
    sbc b
    jp c, __o_div_u32__l4
    
    ld (__div_32_mod+3), a       ; __div_32_mod -= x
    ld a, (__div_32_mod+0)
    sub l
    ld (__div_32_mod+0), a
    ld a, (__div_32_mod+1)
    sbc h
    ld (__div_32_mod+1), a
    ld a, (__div_32_mod+2)
    sbc c
    ld (__div_32_mod+2), a

    ld hl, (__o_div_u32__result) ; result++;
    inc hl
    ld (__o_div_u32__result), hl
    jp __o_div_u32__l4           ; }

__o_div_u32__ret:                ; return result
    ld hl, (__o_div_u32__result)
    ret

__o_div_u32__shl_dehl:
    ex hl, de
    add hl, hl
    ex hl, de
    push af
    add hl, hl
    jp nc, __o_div_u32__l5
    inc de
__o_div_u32__l5:
    pop af
    ret
    
; Input: de:hl - value 1, stack - 32 bit value 2
; Output: de:hl - result

__o_shl_32:
    ld a, l
    pop hl ; hl = ret, stack = v2l
    ex (sp), hl ; hl = v2l, stack = ret
    ld bc, hl
    pop hl ; hl = ret, stack = v2l
    ex (sp), hl ; hl = v2h, stack = ret
    ex hl, de
    ld hl, bc
    and 31
__o_shl_32_1:
    ret z
    ex hl, de
    add hl, hl
    ex hl, de
    add hl, hl
    jp nc, __o_shl_32_2
    inc de
__o_shl_32_2:
    dec a
    jp __o_shl_32_1
__o_minus_32:
    xor a
    sub l
    ld l, a
    ld a, 0
    sbc h
    ld h, a
    ld a, 0
    sbc e
    ld e, a
    ld a, 0
    sbc d
    ld d, a
    ret
__c_5: db 0
__c_2: db 10, 10, 10, 10, 0
__c_12: db 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 119, 44, 97, 44, 115, 44, 100, 32, 111, 114, 32, 114, 44, 113, 32, 32, 32, 32, 32, 32, 32, 10, 0
__c_0: db 27, 91, 72, 0
__c_7: db 27, 91, 72, 27, 91, 50, 74, 0
__c_18: db 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 71, 65, 77, 69, 32, 79, 86, 69, 82, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10, 0
__c_19: db 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 81, 85, 73, 84, 63, 32, 40, 121, 47, 110, 41, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10, 0
__c_20: db 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 82, 69, 83, 84, 65, 82, 84, 63, 32, 40, 121, 47, 110, 41, 32, 32, 32, 32, 32, 32, 32, 10, 0
__c_3: db 37, 117, 0
__c_10: db 43, 10, 0
__c_9: db 43, 45, 45, 45, 45, 45, 45, 45, 0
__c_8: db 50, 48, 52, 56, 46, 99, 32, 37, 49, 55, 100, 32, 112, 116, 115, 10, 10, 0
__c_1: db 50, 48, 52, 56, 46, 99, 32, 37, 50, 51, 100, 32, 112, 116, 115, 0
__c_11: db 124, 10, 0
__c_6: db 124, 32, 32, 32, 32, 32, 32, 32, 0
__c_4: db 124, 37, 42, 115, 37, 115, 37, 42, 115, 0
score:
	dd 0
board:
	ds 16
__s_addrandom_initialized:
	db 0
__printf_out_pointer:
	ds 2
__printf_out_end:
	ds 2
__printf_out_total:
	ds 2
rand_seed:
	db 250
__static_stack:
	ds 70
__s_drawboard equ __static_stack + 48
__s_strlen equ __static_stack + 0
__a_1_strlen equ __s_strlen + 0
__s_preparescreen equ __static_stack + 59
__s_findtarget equ __static_stack + 0
__a_1_findtarget equ __s_findtarget + 1
__a_2_findtarget equ __s_findtarget + 3
__a_3_findtarget equ __s_findtarget + 4
__s_slidearray equ __static_stack + 5
__a_1_slidearray equ __s_slidearray + 4
__s_rotateboard equ __static_stack + 0
__s_moveup equ __static_stack + 11
__s_moveleft equ __static_stack + 13
__s_movedown equ __static_stack + 13
__s_moveright equ __static_stack + 13
__s_findpairdown equ __static_stack + 0
__s_countempty equ __static_stack + 0
__s_gameended equ __static_stack + 4
__s_addrandom equ __static_stack + 0
__s_initboard equ __static_stack + 62
__s_test equ __static_stack + 0
__s_main equ __static_stack + 64
__a_1_main equ __s_main + 2
__a_2_main equ __s_main + 4
__s_sleep equ __static_stack + 2
__a_1_sleep equ __s_sleep + 0
__s___printf equ __static_stack + 9
__a_1___printf equ __s___printf + 23
__a_2___printf equ __s___printf + 25
__s___snprintf equ __static_stack + 36
__a_1___snprintf equ __s___snprintf + 2
__a_2___snprintf equ __s___snprintf + 4
__a_3___snprintf equ __s___snprintf + 6
__a_4___snprintf equ __s___snprintf + 8
__s_delay equ __static_stack + 0
__a_1_delay equ __s_delay + 0
__s___printf_out equ __static_stack + 3
__a_1___printf_out equ __s___printf_out + 0
__s_putchar equ __static_stack + 1
__a_1_putchar equ __s_putchar + 0
__s___printf_text equ __static_stack + 4
__a_1___printf_text equ __s___printf_text + 1
__s___printf_spaces equ __static_stack + 4
__a_1___printf_spaces equ __s___printf_spaces + 0
__a_2___printf_spaces equ __s___printf_spaces + 2
__a_3___printf_spaces equ __s___printf_spaces + 4
__s_uint32tostring equ __static_stack + 0
__a_1_uint32tostring equ __s_uint32tostring + 1
__a_2_uint32tostring equ __s_uint32tostring + 3
__a_3_uint32tostring equ __s_uint32tostring + 7
__s_cpmbiosconout equ __static_stack + 0
__a_1_cpmbiosconout equ __s_cpmbiosconout + 0
__end:
    savebin "game2048.com", __begin, __end - __begin
