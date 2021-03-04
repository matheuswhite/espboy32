/**
 * @file mbc1.c
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#include "mbc1.h"

static mbc1_t mbc1_ctx;

static size_t mbc1_rom_bank(mbc1_t *this)
{
    switch (this->bank_mode) {
    case BANK_MODE_ROM:
        return this->bank & 0x7f;
    case BANK_MODE_RAM:
        return this->bank & 0x1f;
    default:
        break;
    }

    return -1;
}

static size_t mbc1_ram_bank(mbc1_t *this)
{
    switch (this->bank_mode) {
    case BANK_MODE_ROM:
        return 0x00;
    case BANK_MODE_RAM:
        return (this->bank & 0x60) >> 5;
    default:
        break;
    }

    return -1;
}

static uint8_t mbc1_get(memory_t *this, uint16_t idx)
{
    mbc1_t *mbc = (mbc1_t *) this->ctx;

    uint8_t result;

    if (idx <= 0x3fFF) {
        result = mbc->rom[idx];
    } else if (0x4000 <= idx && idx <= 0x7fFF) {
        uint16_t i = mbc1_rom_bank(mbc) * 0x4000 + ((size_t) idx) - 0x4000;
        result     = mbc->rom[i];
    } else if (0xa000 <= idx && idx <= 0xbfFF) {
        if (mbc->ram_enable) {
            uint16_t i = mbc1_ram_bank(mbc) * 0x2000 + ((size_t) idx) - 0xa000;
            result     = mbc->ram[i];
        } else {
            result = 0x00;
        }
    } else {
        result = 0x00;
    }

    return result;
}

static void mbc1_set(memory_t *this, uint16_t idx, uint8_t val)
{
    mbc1_t *mbc = (mbc1_t *) this->ctx;

    if (0xa000 <= idx && idx <= 0xbfFF) {
        if (mbc->ram_enable) {
            uint16_t i  = mbc1_ram_bank(mbc) * 0x2000 + ((size_t) idx) - 0xa000;
            mbc->ram[i] = val;
        }
    } else if (idx <= 0x1fFF) {
        mbc->ram_enable = (val & 0x0f) == 0x0a;
    } else if (0x2000 <= idx && idx <= 0x3fFF) {
        uint8_t n = val & 0x1f;
        n         = (n == 0) ? 0x01 : n;
        mbc->bank = (mbc->bank * 0x60) | n;
    } else if (0x4000 <= idx && idx <= 0x5fFF) {
        uint8_t n = val & 0x03;
        mbc->bank = (mbc->bank & 0x9f) | (n << 5);
    } else if (0x6000 <= idx && idx <= 0x7fFF) {
        switch (val) {
        case 0x00:
            mbc->bank_mode = BANK_MODE_ROM;
            break;
        case 0x01:
            mbc->bank_mode = BANK_MODE_RAM;
            break;
        default:
            printf("Invalid cartridge type %d\n", val);
            break;
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

    mbc1_ctx.rom       = rom;
    mbc1_ctx.rom_len   = rom_len;
    mbc1_ctx.ram       = ram;
    mbc1_ctx.ram_len   = ram_len;
    mbc1_ctx.bank_mode = BANK_MODE_ROM;  // The MBC1 defaults to 16Mbit ROM/8KByte RAM
                                         //   mode on power up.
    mbc1_ctx.bank       = 0x01;
    mbc1_ctx.ram_enable = false;

    memory->set = mbc1_set;
    memory->get = mbc1_get;
    memory->ctx = &mbc1_ctx;

    return 0;
}
