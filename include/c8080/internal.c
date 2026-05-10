/*
 * c8080 stdlib
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <stddef.h>
#include <c8080/remainder.h>

uint32_t __remainder;

int main(int argc, char **argv);

void __init() {
    /* Zeroing uninitialized variables */
    asm {
        ld   de, __bss
        xor  a
__init_loop:
        ld   (de), a
        inc  de
        ld   hl, 10000h - __end
        add  hl, de
        jp   nc, __init_loop
    }

#if __has_include(<c8080/initstack.inc>)
#include <c8080/initstack.inc>
#endif

    main(0, NULL);
}


// Example: void (*hl)(); hl();
// Input: hl

void __o_call_hl() {
    asm {
        jp   hl
    }
}

// Example: int8_t a, d; a <<= d;
// Input: a, d
// Output: a

void __o_shl_8() {
    asm {
        inc  d
__o_shl_8__l1:
        dec  d
        ret  z
        add  a
        jp   __o_shl_8__l1
    }
}

// Example: uint8_t a, d; a >>= d;
// Input: a, d
// Output: a

void __o_shr_u8() {
    asm {
        TODO
    }
}

// Example: int8_t a, d; a >>= d;
// Input: a, d
// Output: a

void __o_shr_i8() {
    asm {
        TODO
    }
}

// Example: uint8_t a, d; a *= d;
// Input: a, d
// Output: a

void __o_mul_8() {
    asm {
        ld   hl, 0
        ld   e, d  ; de=d
        ld   d, l
        ld   c, 8
__o_mul_8__l1:
        add  hl, hl
        add  a
        jp   nc, __o_mul_8__l2
        add  hl, de
__o_mul_8__l2:
        dec  c
        jp   nz, __o_mul_8__l1
        ld   a, l
    }
}

// Example: int8_t a, d; a /= d;
// Input: a, d
// Output: a

void __o_div_i8() {
    asm {
        TODO
    }
}

// Example: uint8_t a, d; a /= d;
// Input: a, d
// Output: a

void __o_div_u8() {  // TODO: Optimize ADD HL, HL
    (void)__remainder;
    asm {
        ld   e, a
        ld   hl, 0
        ld   (__remainder + 2), hl
        ld   h, 8       ; l = remain, h = loop
        ld   c, l       ; c = result
__o_div_u8_1:
        ld   a, e
        rla
        ld   e, a
        ld   a, l
        rla
        sub  d
        jp   nc, __o_div_u8_2
        add  d
__o_div_u8_2:
        ld   l, a  ; remain
        ccf
        ld   a, c  ; result
        rla
        ld   c, a
        dec  h
        jp   nz, __o_div_u8_1
        ld   (__remainder), hl  ; l = remain, h = 0
        ld   a, c
    }
}

// Example: uint8_t a, d; a %= d;
// Input: a, d
// Output: a

void __o_mod_u8() {
    (void)__o_div_u8;
    asm {
        call __o_div_u8
        ld   a, l
    }
}

// Example: int8_t a, d; a %= d;
// Input: a, d
// Output: a

void __o_mod_i8() {
    asm {
        TODO
    }
}

// Example: int8_t a; int16_t hl = a;
// Input: a
// Output: hl

void __o_i8_to_i16() {
    asm {
        ld   l, a
        rla
        sbc  a
        ld   h, a
    }
}

// Example: uint16_t hl; hl = -hl;
// Input: hl
// Output: hl

void __o_minus_16() {
    asm {
        xor  a
        sub  l
        ld   l, a
        ld   a, 0
        sbc  h
        ld  h, a
    }
}

// Example: uint16_t hl; hl = ~hl;
// Input: hl
// Output: hl

void __o_neg_16() {
    asm {
        ld   a, l
        cpl
        ld   l, a
        ld   a, h
        cpl
        ld   h, a
    }
}

// Example: uint16_t hl, de; hl -= de;
// Input: hl, de
// Output: hl

void __o_sub_16() {
    asm {
        ld   a, l
        sub  e
        ld   l, a
        ld   a, h
        sbc  d
        ld   h, a
    }
}

// Example: uint16_t hl, de; hl &= de;
// Input: hl, de
// Output: hl

void __o_and_16() {
    asm {
        ld   a, h
        and  d
        ld   h, a
        ld   a, l
        and  e
        ld   l, a
    }
}

// Example: uint16_t hl, de; hl |= de;
// Input: hl, de
// Output: hl

void __o_or_16() {
    asm {
        ld   a, h
        or   d
        ld   h, a
        ld   a, l
        or   e
        ld   l, a
    }
}

// Example: uint16_t hl, de; hl ^= de;
// Input: hl, de
// Output: hl

void __o_xor_16() {
    asm {
        ld   a, h
        xor  d
        ld   h, a
        ld   a, l
        xor  e
        ld   l, a
        or   h         ; Flag Z used for compare
    }
}

// Example: uint16_t hl, de; hl *= de;
// Input: hl, de
// Output: hl

void __o_mul_16() {
    asm {
        ld   b, h
        ld   c, l
        ld   hl, 0
        ld   a, 17
__o_mul_16_l1:
        dec  a
        ret  z
        add  hl, hl
        ex   hl, de
        add  hl, hl
        ex   hl, de
        jp   nc, __o_mul_16_l1
        add  hl, bc
        jp   __o_mul_16_l1
    }
}

// Example: uint16_t hl, de; hl /= de;
// Input: hl, de
// Output: hl

void __o_div_u16() {
    (void)__remainder;
    asm {
        call __o_div_u16__l0
        ex   hl, de
        ld   (__remainder), hl
        ld   hl, 0
        ld   (__remainder + 2), hl
        ex   hl, de
        ret

__o_div_u16__l0:
        ex   hl, de
__o_div_u16__l:
        ld   a, h
        or   l
        ret  z
        ld   bc, 0
        push bc
__o_div_u16__l1:
        ld   a, e
        sub  l
        ld   a, d
        sbc  h
        jp   c, __o_div_u16__l2
        push hl
        add  hl, hl
        jp   nc, __o_div_u16__l1
__o_div_u16__l2:
        ld   hl, 0
__o_div_u16__l3:
        pop  bc
        ld   a, b
        or   c
        ret  z
        add  hl, hl
        push de
        ld   a, e
        sub  c
        ld   e, a
        ld   a, d
        sbc  b
        ld   d, a
        jp   c, __o_div_u16__l4
        inc  hl
        pop  bc
        jp   __o_div_u16__l3
__o_div_u16__l4:
        pop  de
        jp   __o_div_u16__l3
    }
}

// Example: int16_t hl, de; hl /= de;
// Input: hl, de
// Output: hl

void __o_div_i16() {
    (void)__o_minus_16;
    (void)__o_div_u16;
    asm {
        ld   a, h
        add  a
        jp   nc, __o_div_i16_1  ; hl - positive

        call __o_minus_16

        ld   a, d
        add  a
        jp   nc, __o_div_i16_2  ; hl - negative, de - positive

        ex   hl, de
        call __o_minus_16
        ex   hl, de

        jp   __o_div_u16        ; hl & de - negative

__o_div_i16_1:
        ld   a, d
        add  a
        jp   nc, __o_div_u16    ; hl & de - positive

        ex   hl, de
        call __o_minus_16
        ex   hl, de

__o_div_i16_2:
        call __o_div_u16
        jp   __o_minus_16
    }
}

// Example: uint16_t hl, de; hl %= de;
// Input: hl, de
// Output: hl

void __o_mod_u16() {
    (void)__o_div_u16;
    asm {
        call __o_div_u16__l0
        ex hl, de
    }
}

// Example: int16_t hl, de; hl %= de;
// Input: hl, de
// Output: hl

void __o_mod_i16() {
    asm {
        TODO
    }
}

// Example: uint16_t hl, de; hl <<= de;
// Input: hl, de
// Output: hl

void __o_shl_16() {
    asm {
        inc  e
__o_shl_16__l1:
        dec  e
        ret  z
        add  hl, hl
        jp   __o_shl_16__l1
    }
}

// Example: uint16_t hl, de; hl >>= de;
// Input: hl, de
// Output: hl

void __o_shr_u16() {
    asm {
        inc  e
__o_shr_u16__l1:
        dec  e
        ret  z
        ld   a, h
        or   a    ; cf = 0
        rra
        ld   h, a
        ld   a, l
        rra
        ld   l, a
        jp   __o_shr_u16__l1
1
    }
}

// Example: int16_t hl, de; hl >>= de;
// Input: hl, de
// Output: hl

void __o_shr_i16() {
    asm {
        inc  e
__o_shr_i16__l1:
        dec  e
        ret  z
        ld   a, h
        rla
        ld   a, h
        rra
        ld   h, a
        ld   a, l
        rra
        ld   l, a
        jp   __o_shr_i16__l1
    }
}

// Example: int16_t hl; int32_t dehl = hl;
// Input: hl
// Output: de:hl

void __o_i16_to_i32() {
    asm {
        ld   de, 0
        ld   a, h
        and  80h
        ret  z
        dec  de
    }
}

// Example: uint32_t dehl; dehl = -dehl;
// Input: de:hl
// Output: de:hl

void __o_minus_32() {
    asm {
        xor  a
        sub  l
        ld   l, a
        ld   a, 0
        sbc  h
        ld   h, a
        ld   a, 0
        sbc  e
        ld   e, a
        ld   a, 0
        sbc  d
        ld   d, a
    }
}

// Example: uint32_t dehl; dehl = ~dehl;
// Input: de:hl
// Output: de:hl

void __o_neg_32() {
    asm {
        ld   a, l
        cpl
        ld   l, a
        ld   a, h
        cpl
        ld   h, a
        ld   a, e
        cpl
        ld   e, a
        ld   a, d
        cpl
        ld   d, a
    }
}

// Example: uint32_t dehl, stack; dehl += stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_add_32() {
    asm {
        ld   bc, hl   ; bc = v1l
        pop  hl       ; hl = ret, stack = v2l
        ex   (sp), hl ; hl = v2l, stack = ret
        ld   a, c
        add  l
        ld   c, a
        ld   a, b
        adc  h
        ld   b, a     ; bc - result
        pop  hl       ; hl = ret, stack = v2h
        ex   (sp), hl ; hl = v2h, stack = ret
        ld   a, e
        adc  l
        ld   e, a
        ld   a, d
        adc  h
        ld   d, a     ; de - result
        ld   hl, bc
    }
}

// Example: uint32_t dehl, stack; dehl -= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_sub_32() {
    asm {
        ld   bc, hl    ; bc = v1l
        pop  hl        ; hl = ret, stack = v2l
        ex   (sp), hl  ; hl = v2l, stack = ret
        ld   a, l
        sub  c
        ld   c, a
        ld   a, h
        sbc  b
        ld   b, a      ; bc - result
        pop  hl        ; hl = ret, stack = v2h
        ex  (sp), hl   ; hl = v2h, stack = ret
        ld   a, l
        sbc  e
        ld   e, a
        ld   a, h
        sbc  d
        ld   d, a      ; de - result
        ld   hl, bc
    }
}

// Example: uint32_t *hl, dehl; dehl = *hl;
// Input: hl
// Output: de:hl

void __o_load_32() {
    asm {
        ld   c, (hl)
        inc  hl
        ld   b, (hl)
        inc  hl
        ld   e, (hl)
        inc  hl
        ld   d, (hl)
        ld   hl, bc
    }
}

// Example: uint32_t *stack, dehl; *stack = dehl;
// Input: de:hl, word in stack
// Output: de:hl

void __o_set_32() {
    asm {
        ld   bc, hl
        pop  hl        ; hl = ret, stack = adddress
        ex   (sp), hl  ; hl = address, stack = ret
        ld   (hl), c
        inc  hl
        ld   (hl), b
        inc  hl
        ld   (hl), e
        inc  hl
        ld   (hl), d
        ld   hl, bc
    }
}

// Example: uint32_t dehl, stack; dehl &= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_and_32() {
    asm {
        ld   bc, hl    ; bc = v1l
        pop  hl        ; hl = ret, stack = v2l
        ex   (sp), hl  ; hl = v2l, stack = ret
        ld   a, c
        and  l
        ld   c, a
        ld   a, b
        and  h
        ld   b, a      ; bc - result
        pop  hl        ; hl = ret, stack = v2h
        ex   (sp), hl  ; hl = v2h, stack = ret
        ld   a, e
        and  l
        ld   e, a
        ld   a, d
        and  h
        ld   d, a      ; de - result
        ld   hl, bc
    }
}

// Example: uint32_t dehl, stack; dehl |= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_or_32() {
    asm {
        ld   bc, hl    ; bc = v1l
        pop  hl        ; hl = ret, stack = v2l
        ex   (sp), hl  ; hl = v2l, stack = ret
        ld   a, c
        or   l
        ld   c, a
        ld   a, b
        or   h
        ld   b, a      ; bc - result
        pop  hl        ; hl = ret, stack = v2h
        ex   (sp), hl  ; hl = v2h, stack = ret
        ld   a, e
        or   l
        ld   e, a
        ld   a, d
        or   h
        ld   d, a      ; de - result
        ld   hl, bc
    }
}

// Example: uint32_t dehl, stack; dehl ^= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_xor_32() {
    asm {
        ld   bc, hl    ; bc = v1l
        pop  hl        ; hl = ret, stack = v2l
        ex   (sp), hl  ; hl = v2l, stack = ret
        ld   a, c
        xor  l
        ld   c, a
        ld   a, b
        xor  h
        ld   b, a      ; bc - result
        pop  hl        ; hl = ret, stack = v2h
        ex   (sp), hl  ; hl = v2h, stack = ret
        ld   a, e
        xor  l
        ld   e, a
        ld   a, d
        xor  h
        ld   d, a      ; de - result
        ld   hl, bc
    }
}

// Example: uint32_t dehl, stack; dehl *= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_mul_32() {
    asm {
        ; save arg
        push de
        push hl

        ; result
        ld   hl, 0
        ld   d, h
        ld   e, l

        ; 32 bits
        ld   a, 32
__o_mul_32_l0:

        ; result *= 2
        add  hl, hl
        ex   hl, de
        jp   nc, __o_mul_32_l2
        add  hl, hl
        inc  hl
        jp   __o_mul_32_l3
__o_mul_32_l2:
        add  hl, hl
__o_mul_32_l3:
        ex   hl, de

        ; arg *= 2
        push af
        push hl
        ld   hl, 10 ; af, hl in stack
        add  hl, sp
        ld   a, (hl)
        add  a
        ld   (hl), a
        inc  hl
        ld   a, (hl)
        adc  a
        ld   (hl), a
        inc  hl
        ld   a, (hl)
        adc  a
        ld   (hl), a
        inc  hl
        ld   a, (hl)
        adc  a
        ld   (hl), a
        pop  hl

        jp   nc, __o_mul_32_l1

        ; result += (stack)
        pop  af
        pop  bc
        add  hl, bc
        jp   nc, __o_mul_32_l4
        inc  de
__o_mul_32_l4:
        ex   (sp), hl
        ex   hl, de
        add  hl, de
        ex   hl, de
        ex   (sp), hl
        push bc
        push af

__o_mul_32_l1:
        pop  af
        dec  a
        jp   nz, __o_mul_32_l0

        pop  bc
        pop  bc
        pop  bc
        inc  sp
        inc  sp
        inc  sp
        inc  sp
        push bc
    }
}

// Example: uint32_t dehl, stack; dehl /= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_div_u32() {
    (void)__remainder;
    asm {
        ld   bc, hl                    ; __remainder = a
        pop  hl
        ex   (sp), hl
        ld   (__remainder+0), hl
        pop  hl
        ex   (sp), hl
        ld   (__remainder+2), hl
        ld   hl, __o_div_u32__ret
__o_div_u32__com:
        ld   (__o_div_u32__ra), hl
        ld   hl, bc
        ld   a, h                      ; if (b == 0) return;
        or   l
        or   d
        or   e
        ret  z
        ld   c, 1                      ; c = 1;
__o_div_u32__l1:                       ; do
        ld   a, (__remainder+0)       ; if (a < b) break;
        sub  l
        ld   a, (__remainder+1)
        sbc  h
        ld   a, (__remainder+2)
        sbc  e
        ld   a, (__remainder+3)
        sbc  d
        jp   c, __o_div_u32__l2
        inc  c
        push hl                        ; push(b);
        push de
        call __o_div_u32__shl_dehl     ; b <<= 1;
        jp   nc, __o_div_u32__l1       ; ) while(flag_nc);
__o_div_u32__l2:
        ld   a, c
        ld   (__o_div_u32__lc), a
        ld   hl, 0                     ; result = 0
        ld   (__o_div_u32__result), hl
        ld   de, hl
__o_div_u32__l4:                       ; while(--c != 0) (
__o_div_u32__lc=$+1
        ld   a, 0
        dec  a
__o_div_u32__ra=$+1
        jp   z, __o_div_u32__ret
        ld   (__o_div_u32__lc), a

__o_div_u32__result = $+1
        ld   hl, 0                     ; result <<= 1
        call __o_div_u32__shl_dehl
        ld   (__o_div_u32__result), hl

        pop  bc                        ; pop(x)
        pop  hl

        ld   a, (__remainder+0)       ; if (x < __remainder) continue;
        sub  l
        ld   a, (__remainder+1)
        sbc  h
        ld   a, (__remainder+2)
        sbc  c
        ld   a, (__remainder+3)
        sbc  b
        jp   c, __o_div_u32__l4

        ld   (__remainder+3), a       ; __remainder -= x
        ld   a, (__remainder+0)
        sub  l
        ld   (__remainder+0), a
        ld   a, (__remainder+1)
        sbc  h
        ld   (__remainder+1), a
        ld   a, (__remainder+2)
        sbc  c
        ld   (__remainder+2), a

        ld   hl, (__o_div_u32__result) ; result++;
        inc  hl
        ld   (__o_div_u32__result), hl
        jp   __o_div_u32__l4           ; )

__o_div_u32__ret:                      ; return result
        ld   hl, (__o_div_u32__result)
        ret

__o_div_u32__shl_dehl:
        ex   hl, de
        add  hl, hl
        ex   hl, de
        push af
        add  hl, hl
        jp   nc, __o_div_u32__l5
        inc  de
__o_div_u32__l5:
        pop  af
    }
}

// Example: int32_t dehl, stack; dehl /= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_div_i32() {
    asm {
        TODO
    }
}

// Example: uint32_t dehl, stack; dehl %= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_mod_u32() {
    (void)__o_div_u32;
    asm {
        ld   bc, hl                ; bc = v1l
        pop  hl                    ; hl = ret, stack = v2l
        ex   (sp), hl              ; hl = v2l, stack = ret
        ld   (__remainder+0), hl
        pop  hl                    ; hl = ret, stack = v2h
        ex   (sp), hl              ; hl = v2l, stack = ret
        ld   (__remainder+2), hl
        ld   hl, __o_mod_u32__ret
        jp   __o_div_u32__com

__o_mod_u32__ret:
        ld   hl, (__remainder+2)
        ex   hl, de
        ld   hl, (__remainder+0)
    }
}

// Example: int32_t dehl, stack; dehl %= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_mod_i32() {
    asm {
        TODO
    }
}

// Example: uint32_t dehl, stack; dehl <<= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_shl_32() {
    asm {
        ld   a, l
        pop  hl         ; hl = ret, stack = v2l
        ex   (sp), hl   ; hl = v2l, stack = ret
        ld   bc, hl
        pop  hl         ; hl = ret, stack = v2l
        ex   (sp), hl   ; hl = v2h, stack = ret
        ex   hl, de
        ld   hl, bc
        and  31
__o_shl_32_1:
        ret  z
        ex   hl, de
        add  hl, hl
        ex   hl, de
        add  hl, hl
        jp   nc, __o_shl_32_2
        inc  de
__o_shl_32_2:
        dec  a
        jp   __o_shl_32_1
    }
}

// Example: uint32_t dehl, stack; dehl >>= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_shr_u32() {
    asm {
        ld   a, l
        pop  hl          ; hl = ret, stack = v2l
        ex   (sp), hl    ; hl = v2l, stack = ret
        ld   bc, hl
        pop  hl          ; hl = ret, stack = v2l
        ex   (sp), hl    ; hl = v2h, stack = ret
        ex   hl, de
        ld   hl, bc
        and  31
        ld   c, a
__o_shr_u32_1:
        ret  z
        ld   a, d
        or   a
        rra
        ld   d, a
        ld   a, e
        rra
        ld   e, a
        ld   a, h
        rra
        ld   h, a
        ld   a, l
        rra
        ld   l, a
        dec  c
        jp   __o_shr_u32_1
    }
}

// Example: int32_t dehl, stack; dehl >>= stack;
// Input: de:hl, dword in stack
// Output: de:hl

void __o_shr_i32() {
    asm {
        TODO
    }
}
