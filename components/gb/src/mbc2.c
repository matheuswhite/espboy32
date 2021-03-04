/**
 * @file mbc2.c
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#include "mbc2.h"

static mbc2_t mbc2_ctx;

static uint8_t mbc2_get(memory_t *this, uint16_t idx)
{
    mbc2_t *mbc = (mbc2_t *) this->ctx;

    uint8_t result;

    if (idx <= 0x3fFF) {
        result = mbc->rom[idx];
    } else if (0x4000 <= idx && idx <= 0x7fFF) {
        uint16_t i = mbc->rom_bank * 0x4000 + ((size_t) idx) - 0x4000;
        result     = mbc->rom[i];
    } else if (0xa000 <= idx && idx <= 0xa1FF) {
        if (mbc->ram_enable) {
            result = mbc->ram[((size_t)(idx - 0xa000))];
        } else {
            result = 0x00;
        }
    } else {
        result = 0x00;
    }

    return result;
}

static void mbc2_set(memory_t *this, uint16_t idx, uint8_t val)
{
    mbc2_t *mbc = (mbc2_t *) this->ctx;

    // Only the lower 4 bits of the "bytes" in this memory area are used.
    val = val & 0x0f;

    if (0xa000 <= idx && idx <= 0xa1FF) {
        if (mbc->ram_enable) {
            mbc->ram[((size_t)(idx - 0xa000))] = val;
        }
    } else if (idx <= 0x1fFF) {
        if ((idx & 0x0100) == 0) {
            mbc->ram_enable = val == 0x0a;
        }
    } else if (0x2000 <= idx && idx <= 0x3fFF) {
        if ((idx & 0x0100) != 0) {
            mbc->rom_bank = val;
        }
    }
}

int mbc1_new(memory_t *memory, uint8_t *rom, uint16_t rom_len, uint8_t *ram,
             uint16_t ram_len)
{
    if (memory == NULL || rom == NULL) {
        return -ENODEV;
    }

    if (rom_len == 0) {
        return -ENOENT;
    }

    mbc2_ctx.rom        = rom;
    mbc2_ctx.rom_len    = rom_len;
    mbc2_ctx.ram        = ram;
    mbc2_ctx.ram_len    = ram_len;
    mbc2_ctx.rom_bank   = 1;
    mbc2_ctx.ram_enable = false;

    memory->set = mbc2_set;
    memory->get = mbc2_get;
    memory->ctx = &mbc2_ctx;

    return 0;
}
