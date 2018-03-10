#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "definitions.h"
#include "debug.h"
#include "mbc.h"

void mbc_init(struct Context *this) {
    switch (this->rom_header->catridge_type) {
        case 0x00: // ROM ONLY
        case 0x08: // ROM+RAM
        case 0x09: // ROM+RAM+BATTERY
            this->mbc.rom_extra_bank = this->rom_data + 0x4000;
            this->mbc.mbc_type = RomOnly;
            break;
        case 0x01: // MBC1
        case 0x02: // MBC1+RAM
        case 0x03: // MBC1+RAM+BATTERY
        case 0x04: // MBC1+RAM+BATTERY ???
            this->mbc.rom_extra_bank = this->rom_data + 0x4000;
            this->mbc.mbc_type = MBC1;
            break;
        default:
            fprintf(stderr, "Unknown ROM size (%02x), RAM size(%02x) match with type: %02x\n",
                    this->rom_header->rom_size, this->rom_header->ram_size,
                    this->rom_header->catridge_type);
            exit(EXIT_FAILURE);
    }

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

    if (this->rom_size != expected_rom_size) {
        fprintf(stderr, "ROM size is different than expected: %llu (0x%02x)\n",
                this->rom_size, this->rom_header->rom_size);
        exit(EXIT_FAILURE);
    }

    switch (this->rom_header->ram_size) {
        case 0x00: // None
            break;
        case 0x01: // 2k
            this->mbc.eram_data = malloc(0x00800);
            break;
        case 0x02: // 8k
            this->mbc.eram_data = malloc(0x02000);
            break;
        case 0x03: // 32k
            this->mbc.eram_data = malloc(0x08000);
            break;
        case 0x04: // 128k
            this->mbc.eram_data = malloc(0x20000);
            break;
        case 0x05: // 64k
            this->mbc.eram_data = malloc(0x10000);
            break;
        default:
            fprintf(stderr, "Unknown RAM size: 0x%02x\n", this->rom_header->ram_size);
            exit(EXIT_FAILURE);
    }
}

bool mbc_handle_get_u8(const struct Context *this, uint16_t address, uint8_t *ret_value) {
    if (address < 0x4000) {
        *ret_value = this->rom_data[address];
        return true;
    } else if (address >= 0x4000 && address <= 0x7fff) {
        *ret_value = this->mbc.rom_extra_bank[address - 0x4000];
        return true;
    } else if (address >= 0xa000 && address <= 0xbfff) {
        if (this->mbc.eram_enabled) {
            *ret_value = this->mbc.eram_ptr[address - 0xa000];
        } else {
            fprintf(stderr, "Trying to read eRAM in disabled mode\n");
#ifdef STRICT_MODE
            print_debug(this, EXIT_FAILURE);
#else
            *ret_value = 0xff;
#endif
        }
        return true;
    }

    return false;
}

bool mbc_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    if (address <= 0x1fff) { // enable external ram
        this->mbc.eram_enabled = (value & 0x0f) == 0x0a;
        this->mbc.eram_ptr = this->mbc.eram_data;
        return true;
    } else if (address >= 0x2000 && address <= 0x3fff) { // switch bank
        this->mbc.rom_bank_number.lower_bits = value;
        this->mbc.rom_extra_bank = this->rom_data +
            (0x4000 * (this->mbc.rom_bank_number.raw == 0 ? 1 : this->mbc.rom_bank_number.raw));
        if ((this->mbc.rom_extra_bank + 0x4000) > (this->rom_data + this->rom_size)) {
            fprintf(stderr, "wrong bank change: %02x\n", value);
            print_debug(this, EXIT_FAILURE);
        }
        return true;
    } else if (address >= 0x4000 && address <= 0x5fff) {
        if (this->mbc.rom_ram_mode_select) {
            if (value > 3) {
                fprintf(stderr, "wrong RAM bank change: %02x\n", value);
                print_debug(this, EXIT_FAILURE);
            }
            this->mbc.eram_ptr = this->mbc.eram_data + (value * 0x2000);
        } else {
            uint8_t bank_value = (value >> 4) & 3;
            this->mbc.rom_bank_number.higher_bits = bank_value;
            this->mbc.rom_extra_bank = this->rom_data +
                (0x4000 * (this->mbc.rom_bank_number.raw == 0 ? 1 : this->mbc.rom_bank_number.raw));
            if ((this->mbc.rom_extra_bank + 0x4000) > (this->rom_data + this->rom_size)) {
                fprintf(stderr, "wrong bank change: %02x\n", value);
                print_debug(this, EXIT_FAILURE);
            }
        }
        return true;
    } else if (address >= 0x6000 && address <= 0x7fff) {
        if (value > 1) {
            fprintf(stderr, "Weird ROM/RAM mode select: %02x\n", value);
#ifdef STRICT_MODE
            print_debug(this, EXIT_FAILURE);
#endif
        }
        this->mbc.rom_ram_mode_select = value & 1;
        return true;
    } else if (address >= 0xa000 && address <= 0xbfff) {
        if (this->mbc.eram_enabled) {
            this->mbc.eram_ptr[address - 0xa000] = value;
        } else {
            fprintf(stderr, "Trying to write eRAM in disabled mode\n");
#ifdef STRICT_MODE
            print_debug(this, EXIT_FAILURE);
#endif
        }
        return true;
    }

    return false;
}
