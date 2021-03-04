/**
 * @file mbc5.h
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#ifndef __MBC5_H__
#define __MBC5_H__

#include "memory.h"

typedef struct mbc5 {
    uint8_t *rom;
    uint16_t rom_len;
    uint8_t *ram;
    uint16_t ram_len;
    size_t rom_bank;
    size_t ram_bank;
    bool ram_enable;
} mbc5_t;

int mbc5_new(memory_t *memory, uint8_t *rom, uint16_t rom_len, uint8_t *ram,
             uint16_t ram_len);

#endif  // __MBC5_H__
