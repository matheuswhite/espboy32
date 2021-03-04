/**
 * @file rom_only.h
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#ifndef __ROM_ONLY_H__
#define __ROM_ONLY_H__

#include "memory.h"

typedef struct rom_only {
    uint8_t *rom;
    uint16_t rom_len;
} rom_only_t;

int rom_only_new(memory_t *memory, uint8_t *rom, uint16_t rom_len);

#endif  //__ROM_ONLY_H__
