#ifndef definitions_h
#define definitions_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "flags.h"
#include "cpu.h"
#include "timer.h"
#include "interrupts.h"
#include "video.h"
#include "sound.h"
#include "serial.h"
#include "joypad.h"
#include "mbc.h"

struct __attribute__((packed)) ROM_HEADER {
    uint8_t _[0x100];
    uint8_t entry_point[4];
    uint8_t logo[0x30];
    union {
        char title[0x10];
        struct {
            uint8_t __[11];
            uint8_t manufacturer_code[4];
            uint8_t cgb_flag;
        };
    };
    uint8_t license_code[2];
    uint8_t sgb_flag;
    uint8_t catridge_type;
    uint8_t rom_size;
    uint8_t ram_size;
    uint8_t destination_code;
    uint8_t old_license_code;
    uint8_t mask_rom_version_number;
    uint8_t header_checksum;
    uint8_t global_checksum[2];
};

struct __attribute__((packed)) Sprite {
    uint8_t y_pos;
    uint8_t x_pos;
    uint8_t tile_number;
    union {
        uint8_t raw;
        struct {
            uint8_t : 4;
            uint8_t palette_number: 1;
            uint8_t x_flip: 1;
            uint8_t y_flip: 1;
            uint8_t obj_priority: 1;
        };
    } flags;
};

struct Context {
    uint8_t *bios_rom;
    off_t rom_size;
    union {
        uint8_t *rom_data;
        struct ROM_HEADER *rom_header;
    };
    uint64_t cpu_timing;
    struct CPU cpu;
    bool cpu_halted;
    MBC mbc;
    Interrupts interrupts;
    struct Video video;
    struct Sound sound;
    Timer timer;
    Serial serial;
    Joypad joypad;
    uint8_t vram[0x2000]; // 8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
    uint8_t wram_bank_0[0x1000]; // C000-CFFF   4KB Work RAM Bank 0 (WRAM)
    uint8_t wram_bank_1[0x1000]; // D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
    union {
        uint8_t oam[0xa0]; // FE00-FE9F   Sprite Attribute Table (OAM)
        struct Sprite sprites[0x40];
    };
    uint8_t hram[0xFFFF - 0xFF80]; // FF80-FFFE   High RAM (HRAM)
};

#endif /* definitions_h */
