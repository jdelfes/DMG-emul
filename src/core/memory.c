#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "definitions.h"
#include "interrupts.h"
#include "video.h"
#include "timer.h"
#include "serial.h"
#include "joypad.h"
#include "sound.h"
#include "debug.h"
#include "memory.h"

uint8_t get_mem_u8(struct Context *this, uint16_t address) {
    if (this->bios_rom && address < 0x0100) {
        return this->bios_rom[address];
    } if (address < 0x4000) {
        return this->rom_data[address];
    } else if (address >= 0x4000 && address <= 0x7fff) {
        if (this->rom_extra_bank == NULL)
            this->rom_extra_bank = this->rom_data + 0x4000;

        return this->rom_extra_bank[address - 0x4000];
    } else if (address >= 0x8000 && address <= 0x9fff) {
        return this->vram[address - 0x8000];
    } else if (address >= 0xa000 && address <= 0xbfff) {
        return this->eram[address - 0xa000];
    } else if (address >= 0xc000 && address <= 0xcfff) {
        return this->wram_bank_0[address - 0xc000];
    } else if (address >= 0xd000 && address <= 0xdfff) {
        return this->wram_bank_1[address - 0xd000];
    } else if (address >= 0xfe00 && address <= 0xfe9f) {
        return this->oam[address - 0xfe00];
    } else if (address >= 0xff80 && address <= 0xfffe) {
        return this->hram[address - 0xff80];
    }

    uint8_t ret_value = 0xff;
    if (interrupts_handle_get_u8(this, address, &ret_value)) {
        return ret_value;
    } else if (video_handle_get_u8(this, address, &ret_value)) {
        return ret_value;
    } else if (timer_handle_get_u8(this, address, &ret_value)) {
        return ret_value;
    } else if (serial_handle_get_u8(this, address, &ret_value)) {
        return ret_value;
    } else if (joypad_handle_get_u8(this, address, &ret_value)) {
        return ret_value;
    } else if (sound_handle_get_u8(this, address, &ret_value)) {
        return ret_value;
    }

    switch (address) {
        case 0xff4d: // KEY1 - CGB Mode Only - Prepare Speed Switch
            return 0xff;
    }

    fprintf(stderr, "Weird u8 read address: %04x\n", address);
    print_debug(this, EXIT_FAILURE);
}

uint16_t get_mem_u16(struct Context *this, uint16_t address) {
    if (this->bios_rom && address < 0x00FF) {
        return (this->bios_rom[address + 1] << 8) | this->bios_rom[address];
    } if (address < 0x3fff) {
        return (this->rom_data[address + 1] << 8) | this->rom_data[address];
    } else if (address <= 0x7ffe) {
        if (this->rom_extra_bank == NULL)
            this->rom_extra_bank = this->rom_data + 0x4000;

        uint32_t offset = address - 0x4000;
        return (this->rom_extra_bank[offset + 1] << 8) | this->rom_extra_bank[offset];
    } else if (address >= 0xc000 && address <= 0xcffe) {
        return (this->wram_bank_0[address - 0xc000 + 1] << 8) | this->wram_bank_0[address - 0xc000];
    } else if (address >= 0xd000 && address <= 0xdffe) {
        return (this->wram_bank_1[address - 0xd000 + 1] << 8) | this->wram_bank_1[address - 0xd000];
    } else if (address >= 0xff80 && address <= 0xfffd) {
        return (this->hram[address - 0xff80 + 1] << 8) | this->hram[address - 0xff80];
    }

    fprintf(stderr, "Weird u16 read address: %04x\n", address);
    print_debug(this, EXIT_FAILURE);
}

void set_mem_u8(struct Context *this, uint16_t address, uint8_t value) {
    if (address >= 0x8000 && address <= 0x9fff) {
        this->vram[address - 0x8000] = value;
        return;
    } else if (address >= 0xa000 && address <= 0xbfff) {
        this->eram[address - 0xa000] = value;
        return;
    } else if (address >= 0xc000 && address <= 0xcfff) {
        this->wram_bank_0[address - 0xc000] = value;
        return;
    } else if (address >= 0xd000 && address <= 0xdfff) {
        this->wram_bank_1[address - 0xd000] = value;
        return;
    } else if (address >= 0xe000 && address <= 0xfdff) {
        // Mirror of C000~DDFF (ECHO RAM)
        set_mem_u8(this, address - 0x2000, value);
        return;
    } else if (address >= 0xfe00 && address <= 0xfe9f) {
        // FE00-FE9F   Sprite Attribute Table (OAM)
        this->oam[address - 0xfe00] = value;
        return;
    } else if (address >= 0xfea0 && address <= 0xfeff) {
        // FEA0-FEFF   Not Usable
        return;
    } else if (address >= 0xff80 && address <= 0xfffe) {
        this->hram[address - 0xff80] = value;
        return;
    }

    if (interrupts_handle_set_u8(this, address, value)) {
        return;
    } else if (video_handle_set_u8(this, address, value)) {
        return;
    } else if (timer_handle_set_u8(this, address, value)) {
        return;
    } else if (serial_handle_set_u8(this, address, value)) {
        return;
    } else if (joypad_handle_set_u8(this, address, value)) {
        return;
    } else if (sound_handle_set_u8(this, address, value)) {
        return;
    }

    if (address >= 0x2000 && address <= 0x3fff) { // switch bank
        this->rom_extra_bank = this->rom_data + (0x4000 * (value == 0 ? 1 : value));
        if ((this->rom_extra_bank + 0x4000) > (this->rom_data + this->rom_size)) {
            fprintf(stderr, "wrong bank change: %02x\n", value);
            print_debug(this, EXIT_FAILURE);
        }
        return;
    }

    switch (address) {
        case 0x0000: // MBC1 enable external ram
            return;
        case 0x00ff:
            return;
        case 0xff50: // turn off DMG bios rom
            if (this->bios_rom && value != 1) {
                fprintf(stderr, "weird value for disable bios_rom: %02x\n", value);
                exit(EXIT_FAILURE);
            }
            this->bios_rom = NULL;
            return;
        case 0xff4d: // KEY1 - CGB Mode Only - Prepare Speed Switch
            d_printf("KEY1 %02x\n", value);
            return;
        case 0xff7f: // ???
            return;
    }

    fprintf(stderr, "Weird u8 write address: %04x value(%02x)\n", address, value);
    print_debug(this, EXIT_FAILURE);
}

void set_mem_u16(struct Context *this, uint16_t address, uint16_t value) {
    if (address >= 0xc000 && address <= 0xcfff) {
        this->wram_bank_0[address - 0xc000] = value & 0xff;
        this->wram_bank_0[address - 0xc000 + 1] = value >> 8;
        return;
    } else if (address >= 0xd000 && address <= 0xdfff) {
        this->wram_bank_1[address - 0xd000] = value & 0xff;
        this->wram_bank_1[address - 0xd000 + 1] = value >> 8;
        return;
    } else if (address >= 0xff80 && address <= 0xfffd) {
        this->hram[address - 0xff80] = value & 0xff;
        this->hram[address - 0xff80 + 1] = value >> 8;
        return;
    }

    fprintf(stderr, "Weird u16 write address: %04x value(%04x)\n", address, value);
    print_debug(this, EXIT_FAILURE);
}
