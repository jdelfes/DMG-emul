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

int main(int argc, const char * argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s rom.gb bios.rom\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr,  "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    video_init();
    return load_and_run(argv[1], argv[2]);
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

    if (this.rom_size > 0x4000) {
        this.rom_extra_bank = this.rom_data + 0x4000;
    }

    printf("License code: 0x%02x 0x%02x\n",
           this.rom_header->license_code[0],
           this.rom_header->license_code[1]);
    printf("ROM Name: %s\n", this.rom_header->title);
    printf("Catridge Type: 0x%02x\n", this.rom_header->catridge_type);
    printf("ROM Size: 0x%02x\n", this.rom_header->rom_size);
    printf("RAM Size: 0x%02x\n", this.rom_header->ram_size);
    printf("Destination Code: 0x%02x\n", this.rom_header->destination_code);
    printf("License Code: 0x%02x\n", this.rom_header->old_license_code);

    this.bios_rom = map_file(bios_filename, NULL);

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
