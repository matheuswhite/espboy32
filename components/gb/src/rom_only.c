/**
 * @file rom_only.c
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#include "rom_only.h"

static rom_only_t rom_only_ctx;

static uint8_t rom_only_get(memory_t *this, uint16_t idx)
{
    rom_only_t *rom = (rom_only_t *) this->ctx;

    return rom->rom[idx];
}

static void rom_only_set(memory_t *this, uint16_t idx, uint8_t val)
{
}

int rom_only_new(memory_t *memory, uint8_t *rom, uint16_t rom_len)
{
    if (memory == NULL || rom == NULL) {
        return -ENODEV;
    }

    if (rom_len == 0) {
        return -ENOENT;
    }

    rom_only_ctx.rom     = rom;
    rom_only_ctx.rom_len = rom_len;

    memory->get = rom_only_get;
    memory->set = rom_only_set;
    memory->ctx = &rom_only_ctx;

    return 0;
}
