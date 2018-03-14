#include <stdio.h>
#include <stdbool.h>

#include "definitions.h"
#include "debug.h"
#include "mbc2.h"

static void set_rom_bank(struct Context *this);

void mbc2_init(struct Context *this) {
    MBC2Data *mbc_data = calloc(1, sizeof(MBC2Data));
    this->mbc.data = mbc_data;
    mbc_data->rom_extra_bank = this->rom_data + 0x4000;

#ifdef STRICT_MODE
    if (this->rom_header->ram_size > 0) {
        fprintf(stderr, "Weird ram size for MBC2: %02x\n", this->rom_header->ram_size);
        exit(EXIT_FAILURE);
    }
#endif
}

bool mbc2_handle_get_u8(const struct Context *this, uint16_t address, uint8_t *ret_value) {
    MBC2Data *mbc_data = this->mbc.data;

    if (address < 0x4000) {
        *ret_value = this->rom_data[address];
        return true;
    } else if (address >= 0x4000 && address <= 0x7fff) {
        *ret_value = mbc_data->rom_extra_bank[address - 0x4000];
        return true;
    } else if (address >= 0xa000 && address <= 0xa1ff) {
        if (mbc_data->ram_enabled) {
            *ret_value = mbc_data->ram_data[address - 0xa000].n0;
        } else {
#ifdef STRICT_MODE
            fprintf(stderr, "Trying to read eRAM in disabled mode\n");
            print_debug(this, EXIT_FAILURE);
#else
            *ret_value = 0xff;
#endif
        }
        return true;
    }

    return false;
}

bool mbc2_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    MBC2Data *mbc_data = this->mbc.data;

    if (address <= 0x1fff) { // enable external ram
        if (address & 0x0100) {
#ifdef STRICT_MODE
            fprintf(stderr, "Invalid address for enable external ram: %04x (%02x)\n", address, value);
            print_debug(this, EXIT_SUCCESS);
#endif
        } else {
            mbc_data->ram_enabled = (value & 0x0f) == 0x0a;
        }
        return true;
    } else if (address >= 0x2000 && address <= 0x3fff) { // switch bank
#ifdef STRICT_MODE
        if (value > 0x0f) {
            fprintf(stderr, "Weird bank selection for MBC2: %02x\n", value);
            print_debug(this, EXIT_FAILURE);
        }
#endif
        if (address & 0x0100) {
            mbc_data->rom_bank_number = value & 0x0f;
            set_rom_bank(this);
        } else {
#ifdef STRICT_MODE
            fprintf(stderr, "Invalid address for bank switch: %04x (%02x)\n", address, value);
            print_debug(this, EXIT_SUCCESS);
#endif
        }
        return true;
    } else if (address >= 0xa000 && address <= 0xa1ff) {
        if (mbc_data->ram_enabled) {
            mbc_data->ram_data[address - 0xa000].byte = value;
#ifdef STRICT_MODE
            if (mbc_data->ram_data[address - 0xa000].n1 > 0) {
                fprintf(stderr, "Weird write value for MBC2 %04x (%02x)\n", address, value);
                print_debug(this, EXIT_FAILURE);
            }
#endif
        } else {
#ifdef STRICT_MODE
            fprintf(stderr, "Trying to write eRAM in disabled mode\n");
            print_debug(this, EXIT_FAILURE);
#endif
        }
        return true;
    }

    return false;
}

static void set_rom_bank(struct Context *this) {
    MBC2Data *mbc_data = this->mbc.data;

    if (mbc_data->rom_bank_number == 0) {
        mbc_data->rom_bank_number = 1;
    }

    mbc_data->rom_extra_bank = this->rom_data + (0x4000 * mbc_data->rom_bank_number);
    if ((mbc_data->rom_extra_bank + 0x4000) > (this->rom_data + this->rom_size)) {
        fprintf(stderr, "wrong bank change: %02x\n", mbc_data->rom_bank_number);
        print_debug(this, EXIT_FAILURE);
    }
}

