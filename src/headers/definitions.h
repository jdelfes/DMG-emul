#ifndef definitions_h
#define definitions_h

#include <stdint.h>

#include "cpu.h"

struct __attribute__((packed)) ROM_HEADER {
    uint8_t _[0x100];
    uint8_t entry_point[4];
    uint8_t logo[0x30];
    char title[0x10];
//    uint8_t cgb_flag;
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

struct __attribute__((packed)) Pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
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
    uint8_t *rom_extra_bank;
    uint64_t cpu_timing;
    struct CPU cpu;
    bool cpu_halted;
    struct {
        bool IME;
        union {
            uint8_t raw;
            struct {
                uint8_t v_blank: 1;
                uint8_t lcd_stat: 1;
                uint8_t timer: 1;
                uint8_t serial: 1;
                uint8_t joypad: 1;
            };
        } IF;
        union {
            uint8_t raw;
            struct {
                uint8_t v_blank: 1;
                uint8_t lcd_stat: 1;
                uint8_t timer: 1;
                uint8_t serial: 1;
                uint8_t joypad: 1;
            };
        } IE;
    } interrupts;
    struct {
        uint64_t last_cpu_timing;
        uint64_t timing;
        struct Pixel screen[144][160];
        union {
            uint8_t raw;
            struct {
                uint8_t mode_flag: 2;
                uint8_t coincidence_flag: 1;
                uint8_t mode_0_h_blank_interrupt: 1;
                uint8_t mode_1_v_blank_interrupt: 1;
                uint8_t mode_2_oam_interrupt: 1;
                uint8_t lyc_ly_coincidence_interrupt: 1;
            };
        } STAT;
        union {
            uint8_t raw;
            struct {
                uint8_t bg_window_display_priority: 1;
                uint8_t obj_display_enable: 1;
                uint8_t obj_size: 1;
                uint8_t bg_tile_map_display_select: 1;
                uint8_t bg_window_tile_data_select: 1;
                uint8_t window_display_enable: 1;
                uint8_t window_tile_map_display_select: 1;
                uint8_t lcd_display_enable: 1;
            };
        } LCDC;
        uint8_t SCY;
        uint8_t SCX;
        uint8_t LY;
        uint8_t LYC;
        union {
            uint8_t raw;
            union {
                uint8_t shade_color_number0: 2;
                uint8_t shade_color_number1: 2;
                uint8_t shade_color_number2: 2;
                uint8_t shade_color_number3: 2;
            };
        } BGP;
        union {
            uint8_t raw;
            union {
                uint8_t : 2;
                uint8_t shade_color_number1: 2;
                uint8_t shade_color_number2: 2;
                uint8_t shade_color_number3: 2;
            };
        } OBP0;
        union {
            uint8_t raw;
            union {
                uint8_t : 2;
                uint8_t shade_color_number1: 2;
                uint8_t shade_color_number2: 2;
                uint8_t shade_color_number3: 2;
            };
        } OBP1;
    } video;
    struct {
        struct {
            uint64_t last_update;
            union {
                struct {
                    uint8_t _;
                    uint8_t value;
                };
                uint16_t internal;
            };
        } DIV;
        uint8_t TIMA;
        union {
            uint8_t raw;
            struct {
                uint8_t clock_select: 2;
                uint8_t enable: 1;
            };
        } TAC;
        uint8_t TMA;
    } timer;
    struct {
        uint8_t SB;
    } serial;
    struct {
        struct {
            union {
                uint8_t raw;
                struct {
                    uint8_t buttons: 4;
                    uint8_t directions: 4;
                };
                struct {
                    uint8_t buttonA: 1;
                    uint8_t buttonB: 1;
                    uint8_t select: 1;
                    uint8_t start: 1;
                    uint8_t right: 1;
                    uint8_t left: 1;
                    uint8_t up: 1;
                    uint8_t down: 1;
                };
            };
        } keys;
        uint8_t JOYP;
    } joypad;
    uint8_t vram[0x2000]; // 8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
    uint8_t eram[0x2000]; // A000-BFFF    8KB External RAM
    uint8_t wram_bank_0[0x1000]; // C000-CFFF   4KB Work RAM Bank 0 (WRAM)
    uint8_t wram_bank_1[0x1000]; // D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
    union {
        uint8_t oam[0xa0]; // FE00-FE9F   Sprite Attribute Table (OAM)
        struct Sprite sprites[0x40];
    };
    uint8_t hram[0xFFFF - 0xFF80]; // FF80-FFFE   High RAM (HRAM)
};

#endif /* definitions_h */
