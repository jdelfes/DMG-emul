#include <stdio.h>
#include <stdbool.h>

#include "definitions.h"
#include "debug.h"
#include "mbc3.h"

static void set_rom_bank(struct Context *this);

void mbc3_init(struct Context *this) {
    bool has_ram = false;
    switch (this->rom_header->catridge_type) {
        case 0x10: // MBC3+TIMER+RAM+BATTERY
        case 0x12: // MBC3+RAM
        case 0x13: // MBC3+RAM+BATTERY
            has_ram = true;
        case 0x0f: // MBC3+TIMER+BATTERY
        case 0x11: // MBC3
            break;
        default:
            fprintf(stderr, "Unknown MBC3 type: %02x\n", this->rom_header->catridge_type);
            exit(EXIT_FAILURE);
    }

    if ((has_ram && this->rom_header->ram_size == 0) ||
        (!has_ram && this->rom_header->ram_size > 0)) {
        fprintf(stderr, "Weird RAM size match for type: %02x (%02x)\n",
                this->rom_header->catridge_type, this->rom_header->ram_size);
        exit(EXIT_FAILURE);
    }

    MBC3Data *mbc_data = calloc(1, sizeof(MBC3Data));
    this->mbc.data = mbc_data;
    mbc_data->rom_extra_bank = this->rom_data + 0x4000;

    switch (this->rom_header->ram_size) {
        case 0x00: // None
            break;
        case 0x01: // 2k
            mbc_data->eram_data = calloc(1, 0x00800);
            break;
        case 0x02: // 8k
            mbc_data->eram_data = calloc(1, 0x02000);
            break;
        case 0x03: // 32k
            mbc_data->eram_data = calloc(1, 0x08000);
            break;
        case 0x04: // 128k
            mbc_data->eram_data = calloc(1, 0x20000);
            break;
        case 0x05: // 64k
            mbc_data->eram_data = calloc(1, 0x10000);
            break;
        default:
            fprintf(stderr, "Unknown RAM size: 0x%02x\n", this->rom_header->ram_size);
            exit(EXIT_FAILURE);
    }
    mbc_data->eram_ptr = mbc_data->eram_data;
}

bool mbc3_handle_get_u8(const struct Context *this, uint16_t address, uint8_t *ret_value) {
    MBC3Data *mbc_data = this->mbc.data;

    if (address < 0x4000) {
        *ret_value = this->rom_data[address];
        return true;
    } else if (address >= 0x4000 && address <= 0x7fff) {
        *ret_value = mbc_data->rom_extra_bank[address - 0x4000];
        return true;
    } else if (address >= 0xa000 && address <= 0xbfff) {
        if (mbc_data->rtc_mode) {
            // TODO
            *ret_value = 0;
        } else if (mbc_data->eram_enabled && mbc_data->eram_ptr != NULL) {
            *ret_value = mbc_data->eram_ptr[address - 0xa000];
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

bool mbc3_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    MBC3Data *mbc_data = this->mbc.data;

    if (address <= 0x1fff) { // enable external ram
        mbc_data->eram_enabled = (value & 0x0f) == 0x0a;
        return true;
    } else if (address >= 0x2000 && address <= 0x3fff) { // switch bank
        if (value & 0x80) {
            fprintf(stderr, "Invalid bank number %02x\n", value);
            exit(EXIT_FAILURE);
        }
        mbc_data->rom_bank_number = value;
        set_rom_bank(this);
        return true;
    } else if (address >= 0x4000 && address <= 0x5fff) {
        if (value <= 3) {
            mbc_data->eram_ptr = mbc_data->eram_data + (value * 0x2000);
            mbc_data->rtc_mode = false;
        } else if (value >= 0x08 && value <= 0x0c) {
            mbc_data->rtc_register = value;
            mbc_data->rtc_mode = true;
        } else {
            fprintf(stderr, "Weird RAM bank/RTC register: %02x\n", value);
            exit(EXIT_FAILURE);
        }
        return true;
    } else if (address >= 0x6000 && address <= 0x7fff) {
        // TODO
        return true;
    } else if (address >= 0xa000 && address <= 0xbfff) {
        if (mbc_data->rtc_mode) {
            // TODO
        } else if (mbc_data->eram_enabled && mbc_data->eram_ptr != NULL) {
            mbc_data->eram_ptr[address - 0xa000] = value;
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
    MBC3Data *mbc_data = this->mbc.data;

    if (mbc_data->rom_bank_number == 0) {
        mbc_data->rom_bank_number = 1;
    }

    mbc_data->rom_extra_bank = this->rom_data + (0x4000 * mbc_data->rom_bank_number);
    if ((mbc_data->rom_extra_bank + 0x4000) > (this->rom_data + this->rom_size)) {
        fprintf(stderr, "wrong bank change: %02x\n", mbc_data->rom_bank_number);
        print_debug(this, EXIT_FAILURE);
    }
}
