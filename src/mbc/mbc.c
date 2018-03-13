#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "definitions.h"
#include "debug.h"
#include "rom_only.h"
#include "mbc1.h"
#include "mbc2.h"
#include "mbc3.h"
#include "mbc5.h"
#include "mbc.h"

void mbc_init(struct Context *this) {
    off_t expected_rom_size;
    switch (this->rom_header->rom_size) {
        case 0x00:
            expected_rom_size = 0x008000; // 32k
            break;
        case 0x01:
            expected_rom_size = 0x010000; // 64k
            break;
        case 0x02:
            expected_rom_size = 0x020000; // 128k
            break;
        case 0x03:
            expected_rom_size = 0x040000; // 256k
            break;
        case 0x04:
            expected_rom_size = 0x080000; // 512k
            break;
        case 0x05:
            expected_rom_size = 0x100000; // 1m
            break;
        case 0x06:
            expected_rom_size = 0x200000; // 2m
            break;
        case 0x07:
            expected_rom_size = 0x400000; // 4m
            break;
        case 0x08:
            expected_rom_size = 0x800000; // 8m
            break;
        case 0x52:
            expected_rom_size = 0x090000; // 1.1m
            break;
        case 0x53:
            expected_rom_size = 0x0a0000; // 1.2m
            break;
        case 0x54:
            expected_rom_size = 0x0c0000; // 1.5m
            break;
        default:
            fprintf(stderr, "Unknown ROM size: 0x%02x\n", this->rom_header->rom_size);
            exit(EXIT_FAILURE);
    }

#ifdef STRICT_MODE
    if (this->rom_size != expected_rom_size) {
#else
    if (this->rom_size < expected_rom_size) {
#endif
        fprintf(stderr, "ROM size is different than expected: %llu (0x%02x)\n",
                this->rom_size, this->rom_header->rom_size);
        exit(EXIT_FAILURE);
    }

    switch (this->rom_header->catridge_type) {
        case 0x00: // ROM ONLY
            this->mbc.type = ROM_ONLY;
            rom_only_init(this);
            break;
        case 0x01: // MBC1
        case 0x02: // MBC1+RAM
        case 0x03: // MBC1+RAM+BATTERY
            this->mbc.type = MBC1;
            mbc1_init(this);
            break;
        case 0x05: // MBC2
        case 0x06: // MBC2+BATTERY
            this->mbc.type = MBC2;
            mbc2_init(this);
            break;
        case 0x0f: // MBC3+TIMER+BATTERY
        case 0x10: // MBC3+TIMER+RAM+BATTERY
        case 0x11: // MBC3
        case 0x12: // MBC3+RAM
        case 0x13: // MBC3+RAM+BATTERY
            this->mbc.type = MBC3;
            mbc3_init(this);
            break;
        case 0x19: // MBC5
        case 0x1a: // MBC5+RAM
        case 0x1b: // MBC5+RAM+BATTERY
        case 0x1c: // MBC5+RUMBLE
        case 0x1d: // MBC5+RUMBLE+RAM
        case 0x1e: // MBC5+RUMBLE+RAM+BATTERY
            this->mbc.type = MBC5;
            mbc5_init(this);
            break;
        default:
            fprintf(stderr, "Unknown ROM size (%02x), RAM size(%02x) match with type: %02x\n",
                    this->rom_header->rom_size, this->rom_header->ram_size,
                    this->rom_header->catridge_type);
            exit(EXIT_FAILURE);
    }
}

bool mbc_handle_get_u8(const struct Context *this, uint16_t address, uint8_t *ret_value) {
    switch (this->mbc.type) {
        case ROM_ONLY:
            return rom_only_handle_get_u8(this, address, ret_value);
        case MBC1:
            return mbc1_handle_get_u8(this, address, ret_value);
        case MBC2:
            return mbc2_handle_get_u8(this, address, ret_value);
        case MBC3:
            return mbc3_handle_get_u8(this, address, ret_value);
        case MBC5:
            return mbc5_handle_get_u8(this, address, ret_value);
        default:
            fprintf(stderr, "get_u8 handle not set for %02x\n", this->mbc.type);
            exit(EXIT_FAILURE);
    }
}

bool mbc_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    switch (this->mbc.type) {
        case ROM_ONLY:
            return rom_only_handle_set_u8(this, address, value);
        case MBC1:
            return mbc1_handle_set_u8(this, address, value);
        case MBC2:
            return mbc2_handle_set_u8(this, address, value);
        case MBC3:
            return mbc3_handle_set_u8(this, address, value);
        case MBC5:
            return mbc5_handle_set_u8(this, address, value);
        default:
            fprintf(stderr, "set_u8 handle not set for %02x\n", this->mbc.type);
            exit(EXIT_FAILURE);
    }
}
