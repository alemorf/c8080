// c8080 stdlib
// Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>

struct DPB {
    uint16_t spt;  // Number of 128-byte records per track
    uint8_t bsh;   // Block shift. 3 => 1k, 4 => 2k, 5 => 4k....
    uint8_t blm;   // Block mask. 7 => 1k, 0Fh => 2k, 1Fh => 4k...
    uint8_t exm;   // Extent mask, see later
    uint16_t dsm;  // (no. of blocks on the disc)-1
    uint16_t drm;  // (no. of directory entries)-1
    uint8_t al0;   // Directory allocation bitmap, first byte
    uint8_t al1;   // Directory allocation bitmap, second byte
    uint16_t cks;  // Checksum vector size, 0 for a fixed disc
                   // No. directory entries/4, rounded up.
    uint16_t off;  // Offset, number of reserved tracks
};

struct FCB {
    uint8_t drive;         // 0 = default, 1 = A, 2 = B
    char name83[8 + 3];    // File name and extension
    uint8_t ex;            // current extent, ie (file pointer / 16384) % 32
    uint8_t s1;            //
    uint8_t s2;            // extent high byte, ie (file pointer / 524288)
    uint8_t rc;            // number of 128 record used in this extent
    struct FCB rename[0];  // For CpmParseName() + CpmRename()
    uint16_t al16[0];      // TODO: union
    uint8_t al8[16];       //
    uint8_t cr;            // current record, ie (file pointer % 16384) / 128
    uint8_t r0;
    uint8_t r1;
    uint8_t r2;
};

// *** CONSTS ***

static const uint8_t CPM_MAX_USERS = 16;
static const uint8_t CPM_MAX_DRIVES = 16;
static const uint8_t CPM_128_BLOCK = 128;

// *** VARIABLES ***

extern uint8_t DEFAULT_DMA[0x80] __address(0x80);

// *** BIOS ***

// TODO
uint8_t __global CpmBiosConSt(void) __link("cpm_h/cpmbiosconst.c");

// TODO
char __global CpmBiosConIn(void) __link("cpm_h/cpmbiosconin.c");

// TODO
void __global CpmBiosConOut(char c) __link("cpm_h/cpmbiosconout.c");

// *** BDOS ***

// 0 P_TERMCPM System Reset
void __global CpmTerminate(void) __link("cpm_h/bdos.c");

// 1 C_READ Console input, return data
char __global CpmConsoleRead(void) __link("cpm_h/bdos.c");

// 2 C_WRITE Console output
void __global CpmConsoleWrite(char) __link("cpm_h/bdos.c");

// 3 A_READ Auxiliary (Reader) input, return data
char __global CpmAuxiliaryRead(void) __link("cpm_h/bdos.c");

// 4 A_WRITE Auxiliary (Punch) output
void __global CpmAuxiliaryWrite(char) __link("cpm_h/bdos.c");

// 5 L_WRITE Printer output
void __global CpmPrinterWrite(char) __link("cpm_h/bdos.c");

// 6 C_RAWIO Direct console I/O
// If arg is 0xFF, then return a character without echoing if one is waiting;
// zero if none is available.
// If arg is not 0xFF, 0xFE, system will output the character
// and return value no meaning.
char __global CpmConsoleDirect(char) __link("cpm_h/bdos.c");

// 7 Get I/O byte
uint8_t __global CpmGetIoByte(void) __link("cpm_h/bdos.c");

// 8 Set I/O byte
void __global CpmSetIoByte(uint8_t byte) __link("cpm_h/bdos.c");

// 9 C_WRITESTR Output string
void __global CpmConsoleWriteString(const char *text) __link("cpm_h/bdos.c");

// 10 C_READSTR Buffered console input
void __global CpmConsoleReadString(char *buffer) __link("cpm_h/bdos.c");

// 11 C_STAT Console status
uint8_t __global CpmConsoleStatus(void) __link("cpm_h/bdos.c");

// 12 S_BDOSVER Return version number
uint16_t __global CpmBdosVersion(void) __link("cpm_h/bdos.c");

// 13 DRV_ALLRESET Reset discs
void __global CpmResetAllDrives(void) __link("cpm_h/bdos.c");

// 14 DRV_SET Select drive
uint8_t __global CpmSetDrive(uint8_t drive) __link("cpm_h/bdos.c");

// 15 F_OPEN Open file
// Returns A=0-3 if successful, A=0FFh if error
uint8_t __global CpmOpen(struct FCB *fcb) __link("cpm_h/bdos.c");

// 16 F_CLOSE Close file
// Returns A=0-3 if successful, A=0FFh if error
uint8_t __global CpmClose(struct FCB *fcb) __link("cpm_h/bdos.c");

// 17 F_SFIRST Search for first
// If "drive" is 0, then selected drive is used.
// If "drive" is '?', then any directory entry for all users (including disc labels, date stamps etc.) will match.
// If "name83" is 0, then any directory entry will match.
// "name83 can include ? marks, which match anything on disc.
// If "all_extents" is '?', then all suitable extents are matched.
// Normally "all_extents" should be set to zero.
struct FCB *__global CpmSearchFirst(uint8_t drive, const char *name83, char all_extents) __link("cpm_h/bdos.c");

// 18 F_SNEXT Search for next
struct FCB *__global CpmSearchNext(void) __link("cpm_h/bdos.c");

// 19 F_DELETE delete file
uint8_t __global CpmDelete(struct FCB *fcb) __link("cpm_h/bdos.c");

// 20 F_READ read next record
uint8_t __global CpmRead(struct FCB *fcb) __link("cpm_h/bdos.c");
static const uint8_t CPM_READ_EOF = 1;

// 21 F_WRITE write next record
uint8_t __global CpmWrite(struct FCB *fcb) __link("cpm_h/bdos.c");

// 22 F_MAKE create file
uint8_t __global CpmCreate(struct FCB *fcb) __link("cpm_h/bdos.c");

// 23 F_RENAME Rename file
// Returns A=0-3 if successful, A=0FFh if error
uint8_t __global CpmRename(struct FCB *fcb) __link("cpm_h/bdos.c");

// 24 DRV_LOGINVEC Return bitmap of logged-in drives
uint16_t __global CpmGetLoggedDrives(void) __link("cpm_h/bdos.c");

// 25 DRV_GET Return current drive
uint8_t __global CpmGetDrive(void) __link("cpm_h/bdos.c");

// 26 F_DMAOFF Set DMA address
void __global CpmSetDma(void *buffer) __link("cpm_h/bdos.c");

// 27 DRV_ALLOCVEC Return address of allocation map
const void *__global CpmGetAllocationBitmap(void) __link("cpm_h/bdos.c");

// 28 DRV_SETRO Software write-protect current disc
void __global CpmSetCurrentDriveReadOnly(void) __link("cpm_h/bdos.c");

// 29 DRV_ROVEC Return bitmap of read-only drives
uint16_t __global CpmGetReadOnlyDrives(void) __link("cpm_h/bdos.c");

// 30 F_ATTRIB Set file attributes
// Returns A=0-3 if successful, A=0FFh if error
uint8_t __global CpmSetFileAttributes(struct FCB *fcb) __link("cpm_h/bdos.c");

// 31 DRV_DPB Get DPB address
const struct DPB *__global CpmGetDpb(void);

// 32 F_USERNUM get user number
uint8_t __global CpmGetUser(void) __link("cpm_h/bdos.c");

// 32 F_USERNUM set user number
void __global CpmSetUser(uint8_t user) __link("cpm_h/bdos.c");

// 33 F_READRAND Random access read record
uint8_t __global CpmRandomRead(struct FCB *fcb) __link("cpm_h/bdos.c");

// 34 F_WRITERAND Random access write record
uint8_t __global CpmRandomWrite(struct FCB *fcb) __link("cpm_h/bdos.c");

// 35 F_SIZE Compute file size
uint8_t __global CpmComputeFileSize(struct FCB *fcb) __link("cpm_h/cpmsize.c");

// 36 F_RANDREC Update random access pointer
void __global CpmUpdateRandomAccessPointer(struct FCB *fcb) __link("cpm_h/cpmsize.c");

// 37 DRV_RESET Selectively reset disc drives
// Returns A=0 if OK, 0FFh if error
uint8_t __global CpmResetDrives(uint16_t bitmap) __link("cpm_h/bdos.c");

// *** CCP ***

#if !defined(ARCH_CPM_BIOS) && !defined(ARCH_CPM_BDOS)

// Execute command
void __global CpmCommand(uint8_t drive_user, const char *text) __link("cpm_h/cpmcommand.c");

#endif

// *** Extended ***

// Parse name and save result into FCB
uint8_t __global CpmParseName(struct FCB *fcb, const char *name) __link("cpm_h/cpmparsename.c");

// Set the 11 attributes to the 7th bits of the file name.
void __global CpmSetAttrib(char *name83, uint16_t attrib) __link("cpm_h/cpmsetattrib.c");

// Get 11 attributes from 7 bits of file name.
uint16_t __global CpmGetAttrib(const char *) __link("cpm_h/cpmgetattrib.c");

// Convert the name83 stored in the FСB to a file name without
// spaces at the end and with a terminating null.
// The function resets 7 bit.
// { 'F','I','L','E','1',' ','T' | 0x80,'X' | 0x80,'T' ] -> "FILE1.TXT"
void __global CpmConvertFromName83(char *out_name, const char *name83) __link("cpm_h/cpmconvertfromname83.c");

// Get 11 attributes from 7 bits of file name, and copy name83.
// The function resets 7 bit.
uint16_t __global CpmGetNameAndAttrib(void *dest_name83, const void *src_name83) __link("cpm_h/cpmgetnameandattrib.c");
