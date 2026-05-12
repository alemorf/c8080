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

#include <cpm.h>
#include <string.h>

static uint8_t CpmCall(void) __address(5);
static uint8_t CpmCallA(void) __address(5);
static void *CpmCallPtr(void) __address(5);
static uint16_t CpmCallHl(void) __address(5);

void __global CpmTerminate(void) {
    asm {
        ld   c, 0
    }
    CpmCall();
}

char __global CpmConsoleRead(void) {
    asm {
        ld   c, 1
    }
    return CpmCallA();
}

void __global CpmConsoleWrite(char) {
    asm {
__a_1_cpmconsolewrite = 0
        ld   e, a
        ld   c, 2
    }
    CpmCall();
}

char __global CpmAuxiliaryRead(void) {
    asm {
        ld   c, 3
    }
    return CpmCallA();
}

void __global CpmAuxiliaryWrite(char) {
    asm {
__a_1_cpmauxiliarywrite = 0
        ld   e, a
        ld   c, 4
    }
    CpmCallA();
}

void __global CpmPrinterWrite(char) {
    asm {
__a_1_cpmprinterwrite = 5
        ld   e, a
        ld   c, 5
    }
    CpmCall();
}

char __global CpmConsoleDirect(char) {
    asm {
__a_1_cpmconsoledirect = 0
        ld   e, a
        ld   c, 6
    }
    return CpmCallA();
}

uint8_t __global CpmGetIoByte(void) {
    asm {
        ld   c, 7
    }
    return CpmCallA();
}

void __global CpmSetIoByte(uint8_t) {
    asm {
__a_1_cpmsetiobyte = 0
        ld   e, a
        ld   c, 8
    }
    CpmCall();
}

void __global CpmConsoleWriteString(const char *) {
    asm {
__a_1_cpmconsolewritestring = 0
        ex   hl, de
        ld   c, 9
    }
    CpmCall();
}

void __global CpmConsoleReadString(char *) {
    asm {
__a_1_cpmconsolereadstring = 0
        ex   hl, de
        ld   c, 0Ah
    }
    CpmCall();
}

uint8_t __global CpmConsoleStatus(void) {
    asm {
        ld   c, 0Bh
    }
    CpmCall();
}

uint16_t __global CpmBdosVersion(void) {
    asm {
        ld   c, 0Ch
    }
    return CpmCallHl();
}

void __global CpmResetAllDrives(void) {
    asm {
        ld   c, 0Dh
    }
    CpmCallA();
}

uint8_t __global CpmSetDrive(uint8_t) {
    asm {
__a_1_cmdselectdrive = 0
        ld   e, a
        ld   c, 0Eh
    }
    return CpmCallA();
}

uint8_t __global CpmOpen(struct FCB *fcb) {
    // The FCB should have its DR, Fn and Tn fields filled in, and the four
    // fields EX, S1, S2 and RC set to zero.
    asm {
__a_1_cpmopen = 0
        ex   hl, de
        ld   hl, 12
        add  hl, de
        ld   c, 24
cpmopen_1:
        ld   (hl), 0
        inc  hl
        dec  c
        jp   nz, cpmopen_1

        ld   c, 0Fh
    }
    return CpmCallA();
}

uint8_t __global CpmClose(struct FCB *fcb) {
    asm {
__a_1_cpmclose = 0
        ex   hl, de
        ld   c, 10h
    }
    return CpmCallA();
}

static struct FCB *_CpmSearch(void);
static uint8_t cpm_search_buffer[1 + 8 + 3 + 1];

struct FCB *__global CpmSearchFirst(uint8_t, const char *, char) {
    (void)cpm_search_buffer;
    asm {
        ; all_extents
__a_3_cpmsearchfirst = 0
        ld (cpm_search_buffer + 1 + 8 + 3), a

        ; drive
__a_1_cpmsearchfirst = cpm_search_buffer

        ; name83
__a_2_cpmsearchfirst = $ + 1
        ld   hl, 0
        ld   de, cpm_search_buffer + 1
        ld   c, 8 + 3
CpmSearchFirst_1:
        ld   a, h
        or   l
        ld   a, '?'
        jp   z, CpmSearchFirst_2
        ld   a, (hl)
        inc  hl
CpmSearchFirst_2:
        ld   (de), a
        inc  de
        dec  c
        jp   nz, CpmSearchFirst_1

        ; Search
        ld   c, 11h
_cpmsearch:
        ld   de, cpm_search_buffer
        call 5

        ; Result
        ld   hl, 0
        rrca
        rrca
        rrca
        ret  c
        add  80h ; Default DMA
        ld   l, a
    }
}

struct FCB *__global CpmSearchNext(void) {
    asm {
        ld   c, 12h
    }
    return _CpmSearch();
}

uint8_t __global CpmDelete(struct FCB *fcb) {
    asm {
__a_1_cpmdelete = 0
        ex   hl, de
        ld   c, 13h
    }
    return CpmCallA();
}

uint8_t __global CpmRead(struct FCB *fcb) {
    asm {
__a_1_cpmwrite = 0
        ex   hl, de
        ld   c, 14h
    }
    return CpmCallA();
}

uint8_t __global CpmWrite(struct FCB *fcb) {
    asm {
__a_1_cpmwrite = 0
        ex   hl, de
        ld   c, 15h
    }
    return CpmCallA();
}

uint8_t __global CpmCreate(struct FCB *fcb) {
    asm {
__a_1_cpmcreate = 0
        ex   hl, de
        ld   c, 16h
    }
    return CpmCallA();
}

uint8_t __global CpmRename(struct FCB *fcb) {
    asm {
__a_1_cpmrename = 0
        ex   hl, de
        ld   c, 17h
    }
    return CpmCallA();
}

uint16_t __global CpmGetLoggedDrives(void) {
    asm {
        ld   c, 18h
    }
    return CpmCallHl();
}

uint8_t __global CpmGetDrive(void) {
    asm {
        ld   c, 19h
    }
    return CpmCallA();
}

void __global CpmSetDma(void *buffer) {
    asm {
__a_1_cpmsetdma = 0
        ex   hl, de
        ld   c, 1Ah
    }
    CpmCallA();
}

const void *__global CpmGetAllocationBitmap(void) {
    asm {
        ld   c, 1Bh
    }
    return CpmCallPtr();
}

void __global CpmSetCurrentDriveReadOnly(void) {
    asm {
        ld   c, 1Ch
    }
    CpmCallA();
}

uint16_t __global CpmGetReadOnlyDrives(void) {
    asm {
        ld   c, 1Dh
    }
    return CpmCallHl();
}

uint8_t __global CpmSetFileAttributes(struct FCB *fcb) {
    asm {
__a_1_cpmsetfileattributes = 0
        ex   hl, de
        ld   c, 1Eh
    }
    return CpmCallA();
}

const struct DPB *__global CpmGetDpb(void) {
    asm {
        ld   c, 1Fh
    }
    return CpmCallPtr();
}

uint8_t __global CpmGetUser(void) {
    asm {
        ld   e, 0FFh
        ld   c, 20h
    }
    return CpmCallA();
}

void __global CpmSetUser(uint8_t user) {
    asm {
__a_1_cpmsetuser = 0
        ld   e, a
        ld   c, 20h
    }
    CpmCall();
}

uint8_t __global CpmRandomRead(struct FCB *fcb) {
    asm {
__a_1_cpmrandomread = 0
        ex   hl, de
        ld   c, 21h
    }
    return CpmCallA();
}

uint8_t __global CpmRandomWrite(struct FCB *fcb) {
    asm {
__a_1_cpmrandomwrite = 0
        ex   hl, de
        ld   c, 22h
    }
    return CpmCallA();
}

uint8_t __global CpmComputeFileSize(struct FCB *fcb) {
    asm {
__a_1_cpmcomputefilesize = 0
        ex   hl, de
        ld   c, 23h
    }
    return CpmCallA();
}

void __global CpmUpdateRandomAccessPointer(struct FCB *fcb) {
    asm {
__a_1_cpmupdaterandomaccesspointer = 0
        ex   hl, de
        ld   c, 24h
    }
    CpmCallA();
}

uint8_t __global CpmResetDrives(uint16_t bitmap) {
    asm {
__a_1_cpmresetdrives = 0
        ex   hl, de
        ld   c, 25h
    }
    return CpmCallA();
}
