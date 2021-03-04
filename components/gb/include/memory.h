/**
 * @file memory.h
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// General Memory Map
// 0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
// 4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
// 8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
// A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
// C000-CFFF   4KB Work RAM Bank 0 (WRAM)
// D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
// E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
// FE00-FE9F   Sprite Attribute Table (OAM)
// FEA0-FEFF   Not Usable
// FF00-FF7F   I/O Ports
// FF80-FFFE   High RAM (HRAM)
// FFFF        Interrupt Enable Register
//
// See: http://bgb.bircd.org/pandocs.htm#cgbregisters

// struct memory;

typedef struct memory {
    uint8_t (*get)(struct memory *this, uint16_t idx);
    void (*set)(struct memory *this, uint16_t idx, uint8_t val);

    void *ctx;
} memory_t;

uint8_t memory_get(memory_t *m, uint16_t idx);

void memory_set(memory_t *m, uint16_t idx, uint8_t val);

uint16_t memory_get_word(memory_t *m, uint16_t idx);

void memory_set_word(memory_t *m, uint16_t idx, uint16_t val);

#endif  // __MEMORY_H__
