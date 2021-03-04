/**
 * @file mbc3.c
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#include "mbc3.h"

static mbc3_t mbc3_ctx;

// todo
static uint32_t timestamp()
{
    return 0;
}

static void mbc3_rtc_tic(mbc3_rtc_t *rtc)
{
    uint32_t tm = timestamp() - rtc->zero;

    rtc->s        = tm % 60;
    rtc->m        = tm / 60 % 60;
    rtc->h        = tm / 3600 % 24;
    uint16_t days = tm / 3600 / 24;
    rtc->dl       = days % 256;

    if (0x0000 <= days && days <= 0x00ff) {
    } else if (0x0100 <= days && days <= 0x01ff) {
        rtc->dh |= 0x01;
    } else {
        rtc->dh |= 0x01;
        rtc->dh |= 0x80;
    }
}

static uint8_t mbc3_rtc_get(mbc3_rtc_t *rtc, uint16_t idx)
{
    switch (idx) {
    case 0x08:
        return rtc->s;
    case 0x09:
        return rtc->m;
    case 0x0a:
        return rtc->h;
    case 0x0b:
        return rtc->dl;
    case 0x0c:
        return rtc->dh;
    default:
        printf("No entry\n");
        return 0xff;
    }
}

static void mbc3_rtc_set(mbc3_rtc_t *rtc, uint16_t idx, uint8_t val)
{
    switch (idx) {
    case 0x08:
        rtc->s = val;
        break;
    case 0x09:
        rtc->m = val;
        break;
    case 0x0a:
        rtc->h = val;
        break;
    case 0x0b:
        rtc->dl = val;
        break;
    case 0x0c:
        rtc->dh = val;
        break;
    default:
        printf("No entry\n");
    }
}

static uint8_t mbc3_get(memory_t *this, uint16_t idx)
{
    mbc3_t *mbc = (mbc3_t *) this->ctx;

    uint8_t result;

    if (idx <= 0x3fFF) {
        result = mbc->rom[idx];
    } else if (0x4000 <= idx && idx <= 0x7fFF) {
        uint16_t i = mbc->rom_bank * 0x4000 + ((size_t) idx) - 0x4000;
        result     = mbc->rom[i];
    } else if (0xa000 <= idx && idx <= 0xbfFF) {
        if (mbc->ram_enable) {
            if (mbc->ram_bank <= 0x03) {
                uint16_t i = mbc->ram_bank * 0x2000 + ((size_t) idx) - 0xa000;
                result     = mbc->ram[i];
            } else {
                result = mbc3_rtc_get(&mbc->rtc, mbc->ram_bank);
            }
        } else {
            result = 0x00;
        }
    } else {
        result = 0x00;
    }

    return result;
}

static void mbc3_set(memory_t *this, uint16_t idx, uint8_t val)
{
    mbc3_t *mbc = (mbc3_t *) this->ctx;

    if (0xa000 <= idx && idx <= 0xbfFF) {
        if (mbc->ram_enable) {
            if (mbc->ram_bank <= 0x03) {
                uint16_t i  = mbc->ram_bank * 0x2000 + ((size_t) idx) - 0xa000;
                mbc->ram[i] = val;
            } else {
                mbc3_rtc_set(&mbc->rtc, mbc->ram_bank, val);
            }
        }
    } else if (idx <= 0x1fFF) {
        mbc->ram_enable = (val & 0x0f) == 0x0a;
    } else if (0x2000 <= idx && idx <= 0x3fFF) {
        size_t n      = val & 0x7f;
        n             = (n == 0) ? 0x01 : n;
        mbc->rom_bank = n;
    } else if (0x4000 <= idx && idx <= 0x5fFF) {
        size_t n      = val & 0x0f;
        mbc->ram_bank = n;
    } else if (0x6000 <= idx && idx <= 0x7fFF) {
        if ((val & 0x01) != 0) {
            mbc3_rtc_tic(&mbc->rtc);
        }
    }
}

int mbc3_new(memory_t *memory, uint8_t *rom, uint16_t rom_len, uint8_t *ram,
             uint16_t ram_len)
{
    if (memory == NULL || rom == NULL) {
        return -ENODEV;
    }

    if (rom_len == 0) {
        return -ENOENT;
    }

    memset(&mbc3_ctx.rtc, 0, sizeof(mbc3_rtc_t));

    mbc3_ctx.rom        = rom;
    mbc3_ctx.rom_len    = rom_len;
    mbc3_ctx.ram        = ram;
    mbc3_ctx.ram_len    = ram_len;
    mbc3_ctx.rom_bank   = 1;
    mbc3_ctx.ram_bank   = 0;
    mbc3_ctx.ram_enable = false;

    memory->set = mbc3_set;
    memory->get = mbc3_get;
    memory->ctx = &mbc3_ctx;

    return 0;
}
