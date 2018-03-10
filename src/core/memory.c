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
    } else if (address >= 0x8000 && address <= 0x9fff) {
        return this->vram[address - 0x8000];
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
    if (mbc_handle_get_u8(this, address, &ret_value)) {
        return ret_value;
    } else if (interrupts_handle_get_u8(this, address, &ret_value)) {
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
#ifdef STRICT_MODE
    print_debug(this, EXIT_FAILURE);
#else
    return 0xff;
#endif
}

uint16_t get_mem_u16(struct Context *this, uint16_t address) {
    uint8_t low_byte = get_mem_u8(this, address);
    return (get_mem_u8(this, address + 1) << 8) | low_byte;
}

void set_mem_u8(struct Context *this, uint16_t address, uint8_t value) {
    if (address >= 0x8000 && address <= 0x9fff) {
        this->vram[address - 0x8000] = value;
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

    if (mbc_handle_set_u8(this, address, value)) {
        return;
    } else if (interrupts_handle_set_u8(this, address, value)) {
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

    switch (address) {
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

    fprintf(stderr, "%llu Weird u8 write address: %04x value(%02x)\n", this->cpu_timing, address, value);
#ifdef STRICT_MODE
    print_debug(this, EXIT_FAILURE);
#endif
}

void set_mem_u16(struct Context *this, uint16_t address, uint16_t value) {
    set_mem_u8(this, address, value);
    set_mem_u8(this, address + 1, value >> 8);
}
