/**
 * @file cartridge.h
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#ifndef __CARTRIDGE_H__
#define __CARTRIDGE_H__

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include "mbc1.h"
#include "mbc2.h"
#include "mbc3.h"
#include "mbc5.h"
#include "memory.h"
#include "rom_only.h"

int cartridge_power_up(memory_t *memory, uint8_t *rom, size_t rom_len, uint8_t *cart_ram,
                       size_t cart_ram_len);

#endif  // __CARTRIDGE_H__
