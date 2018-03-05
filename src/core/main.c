#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <SDL.h>

#include "definitions.h"
#include "utils.h"
#include "opcodes.h"
#include "memory.h"
#include "cb_prefix.h"
#include "interrupts.h"
#include "video.h"
#include "joypad.h"
#include "timer.h"
#include "debug.h"

int run(struct Context *this);
int load_and_run(const char *filename, const char *bios_filename);
void handle_halt(struct Context *this);
void dmg_init(struct Context *this);

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s rom.gb [bios.rom]\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr,  "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    return load_and_run(argv[1], argc > 2 ? argv[2] : NULL);
}

int load_and_run(const char *filename, const char *bios_filename) {
    struct Context this;
    memset(&this, 0, sizeof(this));

    printf("Parsing %s\n", filename);

    this.rom_data = map_file(filename, &this.rom_size);
    if (!this.rom_data) {
        fprintf(stderr, "Can't read  %s -> %d\n", filename, errno);
        exit(EXIT_FAILURE);
    }

    printf("Flags CGB: 0x%02x  SGB: 0x%02x\n", this.rom_header->cgb_flag, this.rom_header->sgb_flag);
    printf("License code: 0x%02x 0x%02x\n",
           this.rom_header->license_code[0],
           this.rom_header->license_code[1]);
    printf("ROM Name: %s\n", this.rom_header->title);
    printf("Catridge Type: 0x%02x\n", this.rom_header->catridge_type);
    printf("ROM Size: 0x%02x\n", this.rom_header->rom_size);
    printf("RAM Size: 0x%02x\n", this.rom_header->ram_size);
    printf("Destination Code: 0x%02x\n", this.rom_header->destination_code);
    if (this.rom_header->old_license_code == 0x33) {
        printf("License Code: 0x%c%c (new)\n", this.rom_header->license_code[0],
               this.rom_header->license_code[1]);
    } else {
        printf("License Code: 0x%02x\n", this.rom_header->old_license_code);
    }

    video_init();
    mbc_init(&this);

    if (bios_filename) {
        this.bios_rom = map_file(bios_filename, NULL);
    } else {
        dmg_init(&this);
    }

    return run(&this);
}

int run(struct Context *this) {
    while (1) {
        video_check(this);
        joypad_check(this);
        interrupts_check(this);

        struct Opcode opcode;
        if (this->cpu_halted) {
            // execute nop
            opcode.code = 0;
            opcode.size = 1;
        } else {
            opcode = next_opcode(this);
        }
        uint8_t timing = cpu_execute(this, opcode);

        if (timing > 24) {
            fprintf(stderr, "weird timing: %i for opcode: %02x\n", timing, opcode.code);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < timing; i += 4) {
            this->cpu_timing += 4;
            timer_check(this);
        }
    }

    return 0;
}

void dmg_init(struct Context *this) {
    this->cpu.registers.AF = 0x01b0;
    this->cpu.registers.BC = 0x0013;
    this->cpu.registers.DE = 0x00d8;
    this->cpu.registers.HL = 0x014d;
    this->cpu.registers.SP = 0xfffe;
    this->cpu.registers.PC = 0x0100;

    //set_mem_u8(this, 0xFF05, 0x00); // TIMA
    //set_mem_u8(this, 0xFF06, 0x00); // TMA
    //set_mem_u8(this, 0xFF07, 0x00); // TAC
#ifdef SOUND // TODO enable this when sound is implemented
    set_mem_u8(this, 0xFF10, 0x80); // NR10
    set_mem_u8(this, 0xFF11, 0xBF); // NR11
    set_mem_u8(this, 0xFF12, 0xF3); // NR12
    set_mem_u8(this, 0xFF14, 0xBF); // NR14
    set_mem_u8(this, 0xFF16, 0x3F); // NR21
    set_mem_u8(this, 0xFF17, 0x00); // NR22
    set_mem_u8(this, 0xFF19, 0xBF); // NR24
    set_mem_u8(this, 0xFF1A, 0x7F); // NR30
    set_mem_u8(this, 0xFF1B, 0xFF); // NR31
    set_mem_u8(this, 0xFF1C, 0x9F); // NR32
    set_mem_u8(this, 0xFF1E, 0xBF); // NR33
    set_mem_u8(this, 0xFF20, 0xFF); // NR41
    set_mem_u8(this, 0xFF21, 0x00); // NR42
    set_mem_u8(this, 0xFF22, 0x00); // NR43
    set_mem_u8(this, 0xFF23, 0xBF); // NR30
    set_mem_u8(this, 0xFF24, 0x77); // NR50
    set_mem_u8(this, 0xFF25, 0xF3); // NR51
#endif
    set_mem_u8(this, 0xFF26, 0xF1); // $F1-GB, $F0-SGB ; NR52
    set_mem_u8(this, 0xFF40, 0x91); // LCDC
    //set_mem_u8(this, 0xFF42, 0x00); // SCY
    //set_mem_u8(this, 0xFF43, 0x00); // SCX
    //set_mem_u8(this, 0xFF45, 0x00); // LYC
    set_mem_u8(this, 0xFF47, 0xFC); // BGP
    set_mem_u8(this, 0xFF48, 0xFF); // OBP0
    set_mem_u8(this, 0xFF49, 0xFF); // OBP1
    //set_mem_u8(this, 0xFF4A, 0x00); // WY
    //set_mem_u8(this, 0xFF4B, 0x00); // WX
    //set_mem_u8(this, 0xFFFF, 0x00); // IE
}
