#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "definitions.h"
#include "opcodes.h"
#include "memory.h"
#include "debug.h"
#include "cb_prefix.h"

static uint8_t CBOpcodeTiming[] = {
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0x
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 1x
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 2x
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 3x
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 4x
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 5x
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 6x
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 7x
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 8x
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 9x
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // ax
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // bx
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // cx
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // dx
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // ex
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8  // fx
};

static Byte rlc_func(struct Context *this, Byte value);
static Byte rrc_func(struct Context *this, Byte value);
static Byte rl_func(struct Context *this, Byte value);
static Byte rr_func(struct Context *this, Byte value);
static Byte sla_func(struct Context *this, Byte value);
static Byte sra_func(struct Context *this, Byte value);
static Byte swap_func(struct Context *this, Byte value);
static Byte srl_func(struct Context *this, Byte value);
static Byte bit_func(struct Context *this, Byte value, uint8_t bit);
static Byte res_func(struct Context *this, Byte value, uint8_t bit);
static Byte set_func(struct Context *this, Byte value, uint8_t bit);

uint8_t parse_cb(struct Context *this) {
    Instruction cb_opcode;
    cb_opcode.byte = get_mem_u8(this, this->cpu.registers.PC++);
    Byte temp_u8;

    switch (cb_opcode.z) {
        case 0: temp_u8 = this->cpu.registers.B; break;
        case 1: temp_u8 = this->cpu.registers.C; break;
        case 2: temp_u8 = this->cpu.registers.D; break;
        case 3: temp_u8 = this->cpu.registers.E; break;
        case 4: temp_u8 = this->cpu.registers.H; break;
        case 5: temp_u8 = this->cpu.registers.L; break;
        case 6: temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL); break;
        case 7: temp_u8 = this->cpu.registers.A; break;
        default: exit(EXIT_FAILURE);
    }

    switch (cb_opcode.x) {
        case 0:
            switch (cb_opcode.y) {
                case 0: temp_u8 = rlc_func(this, temp_u8); break;
                case 1: temp_u8 = rrc_func(this, temp_u8); break;
                case 2: temp_u8 = rl_func(this, temp_u8); break;
                case 3: temp_u8 = rr_func(this, temp_u8); break;
                case 4: temp_u8 = sla_func(this, temp_u8); break;
                case 5: temp_u8 = sra_func(this, temp_u8); break;
                case 6: temp_u8 = swap_func(this, temp_u8); break;
                case 7: temp_u8 = srl_func(this, temp_u8); break;
            }
            break;
        case 1: bit_func(this, temp_u8, cb_opcode.y); break;
        case 2: temp_u8 = res_func(this, temp_u8, cb_opcode.y); break;
        case 3: temp_u8 = set_func(this, temp_u8, cb_opcode.y); break;
    }

    switch (cb_opcode.z) {
        case 0: this->cpu.registers.B = temp_u8; break;
        case 1: this->cpu.registers.C = temp_u8; break;
        case 2: this->cpu.registers.D = temp_u8; break;
        case 3: this->cpu.registers.E = temp_u8; break;
        case 4: this->cpu.registers.H = temp_u8; break;
        case 5: this->cpu.registers.L = temp_u8; break;
        case 6: set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte); break;
        case 7: this->cpu.registers.A = temp_u8; break;
    }

    return CBOpcodeTiming[cb_opcode.byte];
}

static Byte rlc_func(struct Context *this, Byte value) {
    Byte temp_u8;
    temp_u8.byte = (value.byte << 1) | value.b7;
    this->cpu.registers.flags.zf = (temp_u8.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    this->cpu.registers.flags.cy = temp_u8.b0;
    return temp_u8;
}

static Byte rrc_func(struct Context *this, Byte value) {
    Byte temp_u8;
    temp_u8.byte = (value.byte >> 1) | (value.byte << 7);
    this->cpu.registers.flags.zf = (temp_u8.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    this->cpu.registers.flags.cy = temp_u8.b7;
    return temp_u8;
}

static Byte rl_func(struct Context *this, Byte value) {
    Byte temp_u8;
    temp_u8.byte = (value.byte << 1) | this->cpu.registers.flags.cy;
    this->cpu.registers.flags.zf = (temp_u8.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    this->cpu.registers.flags.cy = value.b7;
    return temp_u8;
}

static Byte rr_func(struct Context *this, Byte value) {
    Byte temp_u8;
    temp_u8.byte = (this->cpu.registers.flags.cy << 7) | (value.byte >> 1);
    this->cpu.registers.flags.cy = value.b0;
    this->cpu.registers.flags.zf = (temp_u8.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    return temp_u8;
}

static Byte sla_func(struct Context *this, Byte value) {
    Byte temp_u8;
    temp_u8.byte = value.byte << 1;
    this->cpu.registers.flags.cy = value.b7;
    this->cpu.registers.flags.zf = (temp_u8.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    return temp_u8;
}

static Byte sra_func(struct Context *this, Byte value) {
    Byte temp_u8;
    temp_u8.byte = (value.byte >> 1) | (value.byte & 0x80);
    this->cpu.registers.flags.cy = value.b0;
    this->cpu.registers.flags.zf = (temp_u8.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    return temp_u8;
}

static Byte swap_func(struct Context *this, Byte value) {
    Byte temp_u8;
    temp_u8.n0 = value.n1;
    temp_u8.n1 = value.n0;
    this->cpu.registers.F = 0;
    this->cpu.registers.flags.zf = (temp_u8.byte == 0) ? 1 : 0;
    return temp_u8;
}

static Byte srl_func(struct Context *this, Byte value) {
    Byte temp_u8;
    temp_u8.byte = value.byte >> 1;
    this->cpu.registers.flags.cy = value.b0;
    this->cpu.registers.flags.zf = (temp_u8.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    return temp_u8;
}

static Byte bit_func(struct Context *this, Byte value, uint8_t bit) {
    this->cpu.registers.flags.zf = (value.byte & (1 << bit)) ? 0 : 1;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 1;
    return value;
}

static Byte res_func(struct Context *this, Byte value, uint8_t bit) {
    value.byte &= ~(1 << bit);
    return value;
}

static Byte set_func(struct Context *this, Byte value, uint8_t bit) {
    value.byte |= (1 << bit);
    return value;
}
