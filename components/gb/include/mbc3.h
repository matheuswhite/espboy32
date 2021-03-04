/**
 * @file mbc3.h
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#ifndef __MBC3_H__
#define __MBC3_H__

#include <string.h>
#include "memory.h"

typedef struct mbc3_rtc {
    uint8_t s;
    uint8_t m;
    uint8_t h;
    uint8_t dl;
    uint8_t dh;
    uint32_t zero;
} mbc3_rtc_t;

/* Beside for the ability to access up to 2MB ROM (128 banks), and 64KB RAM (8 banks), the
MBC3 also includes a built-in Real Time Clock (RTC). The RTC requires an external 32.768
kHz Quartz Oscillator, and an external battery (if it should continue to tick when the
gameboy is turned off). 0000-3FFF - ROM Bank 00 (Read Only) Same as for MBC1.

4000-7FFF - ROM Bank 01-7F (Read Only)
Same as for MBC1, except that accessing banks 20h, 40h, and 60h is supported now.

A000-BFFF - RAM Bank 00-03, if any (Read/Write)
A000-BFFF - RTC Register 08-0C (Read/Write)
Depending on the current Bank Number/RTC Register selection (see below), this memory space
is used to access an 8KByte external RAM Bank, or a single RTC Register.

0000-1FFF - RAM and Timer Enable (Write Only)
Mostly the same as for MBC1, a value of 0Ah will enable reading and writing to external
RAM - and to the RTC Registers! A value of 00h will disable either.

2000-3FFF - ROM Bank Number (Write Only)
Same as for MBC1, except that the whole 7 bits of the RAM Bank Number are written directly
to this address. As for the MBC1, writing a value of 00h, will select Bank 01h instead.
All other values 01-7Fh select the corresponding ROM Banks.

4000-5FFF - RAM Bank Number - or - RTC Register Select (Write Only)
As for the MBC1s RAM Banking Mode, writing a value in range for 00h-07h maps the
corresponding external RAM Bank ( if any) into memory at A000-BFFF. When writing a value
of 08h-0Ch, this will map the corresponding RTC register into memory at A000-BFFF. That
register could then be read/written by accessing any address in that area, typically that
is done by using address A000.

6000-7FFF - Latch Clock Data (Write Only)
When writing 00h, and then 01h to this register, the current time becomes latched into the
RTC registers. The latched data will not change until it becomes latched again, by
repeating the write 00h->01h procedure. This is supposed for <reading> from the RTC
registers. This can be proven by reading the latched (frozen) time from the RTC registers,
and then unlatch the registers to show the clock itself continues to tick in background.

The Clock Counter Registers
 08h  RTC S   Seconds   0-59 (0-3Bh)
 09h  RTC M   Minutes   0-59 (0-3Bh)
 0Ah  RTC H   Hours     0-23 (0-17h)
 0Bh  RTC DL  Lower 8 bits of Day Counter (0-FFh)
 0Ch  RTC DH  Upper 1 bit of Day Counter, Carry Bit, Halt Flag
       Bit 0  Most significant bit of Day Counter (Bit 8)
       Bit 6  Halt (0=Active, 1=Stop Timer)
       Bit 7  Day Counter Carry Bit (1=Counter Overflow)
The Halt Flag is supposed to be set before <writing> to the RTC Registers.

The Day Counter
The total 9 bits of the Day Counter allow to count days in range from 0-511 (0-1FFh). The
Day Counter Carry Bit becomes set when this value overflows. In that case the Carry Bit
remains set until the program does reset it. Note that you can store an offset to the Day
Counter in battery RAM. For example, every time you read a non-zero Day Counter, add this
Counter to the offset in RAM, and reset the Counter to zero. This method allows to count
any number of days, making your program Year-10000-Proof, provided that the cartridge gets
used at least every 511 days.

Delays
When accessing the RTC Registers it is recommended to execute a 4ms delay (4 Cycles in
Normal Speed Mode) between the separate accesses. */
typedef struct mbc3 {
    uint8_t *rom;
    uint16_t rom_len;
    uint8_t *ram;
    uint16_t ram_len;
    mbc3_rtc_t rtc;
    size_t rom_bank;
    size_t ram_bank;
    bool ram_enable;
} mbc3_t;

int mbc3_new(memory_t *memory, uint8_t *rom, uint16_t rom_len, uint8_t *ram,
             uint16_t ram_len);

#endif  // __MBC3_H__
