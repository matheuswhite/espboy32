/**
 * @file cartridge.c
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#include "cartridge.h"

static inline size_t rom_size(uint8_t b)
{
    const size_t bank = 16384;

    switch (b) {
    case 0x00:
        return bank * 2;
    case 0x01:
        return bank * 4;
    case 0x02:
        return bank * 8;
    case 0x03:
        return bank * 16;
    case 0x04:
        return bank * 32;
    case 0x05:
        return bank * 64;
    case 0x06:
        return bank * 128;
    case 0x07:
        return bank * 256;
    case 0x08:
        return bank * 512;
    case 0x52:
        return bank * 72;
    case 0x53:
        return bank * 80;
    case 0x54:
        return bank * 96;
    default:
        printf("Unsupported rom size: 0x%02x\n", b);
        return -1;
        break;
    }
}

static const size_t ram_size_vals[6] = {
    0, 1024 * 2, 1024 * 8, 1024 * 32, 1024 * 128, 1024 * 64,
};

static inline size_t ram_size(uint8_t b)
{
    if (b > 0x05) {
        printf("Unsupported ram size: 0x%02x\n", b);
        return -1;
    }

    return ram_size_vals[b];
}

// Title of the game in UPPER CASE ASCII. If it is less than 16 characters then the
// remaining bytes are filled with 00's. When inventing the CGB, Nintendo has reduced the
// length of this area to 15 characters, and some months later they had the fantastic idea
// to reduce it to 11 characters only. The new meaning of the ex-title bytes is described
// below.
static void cartridge_title(memory_t *memory, char *title)
{
    uint16_t ic = 0x0134;
    uint16_t oc = (memory_get(memory, 0x0143) == 0x80) ? 0x013e : 0x0143;
    uint16_t v;

    for (uint16_t i = ic; i < oc - ic; i++) {
        v = memory_get(memory, i);
        if (v == 0) {
            break;
        } else {
            *title = v;
            title++;
        }
    }
}

static const char *mbc_info(uint8_t b)
{
    switch (b) {
    case 0x00:
        return "ROM ONLY";
    case 0x01:
        return "MBC1";
    case 0x02:
        return "MBC1+RAM";
    case 0x03:
        return "MBC1+RAM+BATTERY";
    case 0x05:
        return "MBC2";
    case 0x06:
        return "MBC2+BATTERY";
    case 0x08:
        return "ROM+RAM";
    case 0x09:
        return "ROM+RAM+BATTERY";
    case 0x0b:
        return "MMM01";
    case 0x0c:
        return "MMM01+RAM";
    case 0x0d:
        return "MMM01+RAM+BATTERY";
    case 0x0f:
        return "MBC3+TIMER+BATTERY";
    case 0x10:
        return "MBC3+TIMER+RAM+BATTERY";
    case 0x11:
        return "MBC3";
    case 0x12:
        return "MBC3+RAM";
    case 0x13:
        return "MBC3+RAM+BATTERY";
    case 0x15:
        return "MBC4";
    case 0x16:
        return "MBC4+RAM";
    case 0x17:
        return "MBC4+RAM+BATTERY";
    case 0x19:
        return "MBC5";
    case 0x1a:
        return "MBC5+RAM";
    case 0x1b:
        return "MBC5+RAM+BATTERY";
    case 0x1c:
        return "MBC5+RUMBLE";
    case 0x1d:
        return "MBC5+RUMBLE+RAM";
    case 0x1e:
        return "MBC5+RUMBLE+RAM+BATTERY";
    case 0xfc:
        return "POCKET CAMERA";
    case 0xfd:
        return "BANDAI TAMA5";
    case 0xfe:
        return "HuC3";
    case 0x1f:
        return "HuC1+RAM+BATTERY";
    default:
        return "Unknown";
    }
}

// These bytes define the bitmap of the Nintendo logo that is displayed when the gameboy
// gets turned on. The reason for joining is because if the pirates copy the cartridge,
// they must also copy Nintendo's LOGO, which infringes the trademark law. In the early
// days, the copyright law is not perfect for the determination of electronic data. The
// hexdump of this bitmap is:
const uint8_t NINTENDO_LOGO[48] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83,
    0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63,
    0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
};

// Ensure Nintendo Logo.
int ensure_logo(memory_t *memory)
{
    for (uint16_t i = 0; i < 48; i++) {
        if (memory_get(memory, 0x0104 + i) != NINTENDO_LOGO[i]) {
            return -EFAULT;
        }
    }

    return 0;
}

// In position 0x14d, contains an 8 bit checksum across the cartridge header bytes
// 0134-014C. The checksum is calculated as follows:
//
//   x=0:FOR i=0134h TO 014Ch:x=x-MEM[i]-1:NEXT
//
// The lower 8 bits of the result must be the same than the value in this entry. The GAME
// WON'T WORK if this checksum is incorrect.
int ensure_header_checksum(memory_t *memory)
{
    uint8_t v = 0;
    for (uint16_t i = 0x0134; i < 0x014d; i++) {
        v = v - memory_get(memory, i) - 1;
    }

    if (memory_get(memory, 0x014d) != v) {
        return -EFAULT;
    }

    return 0;
}

int cartridge_power_up(memory_t *memory, uint8_t *rom, size_t rom_len, uint8_t *cart_ram,
                       size_t cart_ram_len)
{
    if (rom_len < 0x150) {
        printf("Missing required information area which located at 0100-014F\n");
        return -ENOMEM;
    }

    size_t rom_max = rom_size(rom[0x0148]);

    if (rom_len > rom_max) {
        printf("Rom size more than %d\n", rom_max);
        return -ENOENT;
    }

    int err        = 0;
    size_t ram_max = 0;

    switch (rom[0x0147]) {
    case 0x00:
        err = rom_only_new(memory, rom, rom_len);
        if (err) {
            return err;
        }
        break;
    case 0x01:
        err = mbc1_new(memory, rom, rom_len, NULL, 0);
        if (err) {
            return err;
        }
        break;
    case 0x02:
    case 0x03:
        ram_max = ram_size(rom[0x0149]);

        if (ram_max > cart_ram_len) {
            return -EFAULT;
        }

        err = mbc1_new(memory, rom, rom_len, cart_ram, cart_ram_len);
        if (err) {
            return err;
        }
        break;
    case 0x05:
    case 0x06:
        ram_max = 512;

        if (ram_max > cart_ram_len) {
            return -EFAULT;
        }

        err = mbc2_new(memory, rom, rom_len, cart_ram, cart_ram_len);
        if (err) {
            return err;
        }
        break;
    case 0x0f:
    case 0x11:
        err = mbc3_new(memory, rom, rom_len, NULL, 0);
        if (err) {
            return err;
        }
        break;
    case 0x10:
    case 0x12:
    case 0x13:
        ram_max = ram_size(rom[0x0149]);

        if (ram_max > cart_ram_len) {
            return -EFAULT;
        }

        err = mbc3_new(memory, rom, rom_len, cart_ram, cart_ram_len);
        if (err) {
            return err;
        }
        break;
    case 0x19:
        err = mbc5_new(memory, rom, rom_len, NULL, 0);
        if (err) {
            return err;
        }
        break;
    case 0x1a:
    case 0x1b:
        ram_max = ram_size(rom[0x0149]);

        if (ram_max > cart_ram_len) {
            return -EFAULT;
        }

        err = mbc5_new(memory, rom, rom_len, cart_ram, cart_ram_len);
        if (err) {
            return err;
        }
        break;
    case 0xff:
        // huc1
        ram_max = ram_size(rom[0x0149]);

        if (ram_max > cart_ram_len) {
            return -EFAULT;
        }

        err = mbc1_new(memory, rom, rom_len, cart_ram, cart_ram_len);
        if (err) {
            return err;
        }
        break;
    default:
        printf("Unsupported cartridge type: 0x%02x\n", rom[0x0147]);
        return -ENOEXEC;
    }

    char car_title[16] = {0};
    cartridge_title(memory, car_title);

    printf("Cartridge name is %s\n", car_title);
    printf("Cartrige type is %s\n", mbc_info(memory_get(memory, 0x0147)));

    err = ensure_logo(memory);
    if (err) {
        return err;
    }

    err = ensure_header_checksum(memory);
    if (err) {
        return err;
    }

    return 0;
}
