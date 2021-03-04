/**
 * @file mbc5.c
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#include "mbc5.h"

static mbc5_t mbc5_ctx;

static uint8_t mbc5_get(memory_t *this, uint16_t idx)
{
    mbc5_t *mbc = (mbc5_t *) this->ctx;

    uint8_t result;

    if (idx <= 0x3fFF) {
        result = mbc->rom[idx];
    } else if (0x4000 <= idx && idx <= 0x7fFF) {
        uint16_t i = mbc->rom_bank * 0x4000 + ((size_t) idx) - 0x4000;
        result     = mbc->rom[i];
    } else if (0xa000 <= idx && idx <= 0xbfFF) {
        if (mbc->ram_enable) {
            uint16_t i = mbc->ram_bank * 0x2000 + ((size_t) idx) - 0xa000;
            result     = mbc->ram[i];
        } else {
            result = 0x00;
        }
    } else {
        result = 0x00;
    }

    return result;
}

static void mbc5_set(memory_t *this, uint16_t idx, uint8_t val)
{
    mbc5_t *mbc = (mbc5_t *) this->ctx;

    if (0xa000 <= idx && idx <= 0xbfFF) {
        if (mbc->ram_enable) {
            uint16_t i  = mbc->ram_bank * 0x2000 + ((size_t) idx) - 0xa000;
            mbc->ram[i] = val;
        }
    } else if (idx <= 0x1fFF) {
        mbc->ram_enable = (val & 0x0f) == 0x0a;
    } else if (0x2000 <= idx && idx <= 0x2fFF) {
        mbc->rom_bank = (mbc->rom_bank & 0x100) | ((size_t) val);
    } else if (0x3000 <= idx && idx <= 0x3fFF) {
        mbc->rom_bank = (mbc->rom_bank & 0x0ff) | (((size_t)(val & 0x01)) << 8);
    } else if (0x4000 <= idx && idx <= 0x5fFF) {
        mbc->ram_bank = (size_t)(val & 0x0f);
    }
}

int mbc5_new(memory_t *memory, uint8_t *rom, uint16_t rom_len, uint8_t *ram,
             uint16_t ram_len)
{
    if (memory == NULL || rom == NULL) {
        return -ENODEV;
    }

    if (rom_len == 0) {
        return -ENOENT;
    }

    mbc5_ctx.rom        = rom;
    mbc5_ctx.rom_len    = rom_len;
    mbc5_ctx.ram        = ram;
    mbc5_ctx.ram_len    = ram_len;
    mbc5_ctx.rom_bank   = 1;
    mbc5_ctx.ram_bank   = 0;
    mbc5_ctx.ram_enable = false;

    memory->set = mbc5_set;
    memory->get = mbc5_get;
    memory->ctx = &mbc5_ctx;

    return 0;
}
