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

uint8_t CBOpcodeTiming[] = {
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

struct Byte rlc_func_v(struct Context *this, struct Byte value);
void rlc_func(struct Context *this, struct Byte *reg);
uint8_t rrc_func_v(struct Context *this, uint8_t value);
void rrc_func(struct Context *this, struct Byte *reg);
uint8_t rl_func_v(struct Context *this, uint8_t value);
void rl_func(struct Context *this, struct Byte *reg);
uint8_t rr_func_v(struct Context *this, uint8_t value);
void rr_func(struct Context *this, struct Byte *reg);
uint8_t sla_func_v(struct Context *this, uint8_t value);
void sla_func(struct Context *this, struct Byte *reg);
uint8_t sra_func_v(struct Context *this, uint8_t value);
void sra_func(struct Context *this, struct Byte *reg);
uint8_t swap_func_v(struct Context *this, uint8_t value);
void swap_func(struct Context *this, struct Byte *reg);
uint8_t srl_func_v(struct Context *this, uint8_t value);
void srl_func(struct Context *this, struct Byte *reg);
void bit0_func(struct Context *this, struct Byte value);
void bit1_func(struct Context *this, struct Byte value);
void bit2_func(struct Context *this, struct Byte value);
void bit3_func(struct Context *this, struct Byte value);
void bit4_func(struct Context *this, struct Byte value);
void bit5_func(struct Context *this, struct Byte value);
void bit6_func(struct Context *this, struct Byte value);
void bit7_func(struct Context *this, struct Byte value);

uint8_t parse_cb(struct Context *this) {
    uint8_t cb_opcode = get_mem_u8(this, this->cpu.registers.PC++);
    struct Byte temp_u8;

    switch (cb_opcode) {
        case 0x00: // RLC B
            rlc_func(this, &this->cpu.registers.B);
            break;
        case 0x01: // RLC C
            rlc_func(this, &this->cpu.registers.C);
            break;
        case 0x02: // RLC D
            rlc_func(this, &this->cpu.registers.D);
            break;
        case 0x03: // RLC E
            rlc_func(this, &this->cpu.registers.E);
            break;
        case 0x04: // RLC H
            rlc_func(this, &this->cpu.registers.H);
            break;
        case 0x05: // RLC L
            rlc_func(this, &this->cpu.registers.L);
            break;
        case 0x06: // RLC (HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            set_mem_u8(this, this->cpu.registers.HL, rlc_func_v(this, temp_u8).byte);
            break;
        case 0x07: // RLC A
            rlc_func(this, &this->cpu.registers.A);
            break;
        case 0x08: // RRC B
            rrc_func(this, &this->cpu.registers.B);
            break;
        case 0x09: // RRC C
            rrc_func(this, &this->cpu.registers.C);
            break;
        case 0x0a: // RRC D
            rrc_func(this, &this->cpu.registers.D);
            break;
        case 0x0b: // RRC E
            rrc_func(this, &this->cpu.registers.E);
            break;
        case 0x0C: // RRC H
            rrc_func(this, &this->cpu.registers.H);
            break;
        case 0x0d: // RRC L
            rrc_func(this, &this->cpu.registers.L);
            break;
        case 0x0e: // RRC (HL)
            set_mem_u8(this, this->cpu.registers.HL,
                       rrc_func_v(this, get_mem_u8(this, this->cpu.registers.HL)));
            break;
        case 0x0f: // RRC A
            rrc_func(this, &this->cpu.registers.A);
            break;
        case 0x10: // RL B
            rl_func(this, &this->cpu.registers.B);
            break;
        case 0x11: // RL C
            rl_func(this, &this->cpu.registers.C);
            break;
        case 0x12: // RL D
            rl_func(this, &this->cpu.registers.D);
            break;
        case 0x13: // RL E
            rl_func(this, &this->cpu.registers.E);
            break;
        case 0x14: // RL H
            rl_func(this, &this->cpu.registers.H);
            break;
        case 0x15: // RL L
            rl_func(this, &this->cpu.registers.L);
            break;
        case 0x16: // RL (HL)
            set_mem_u8(this, this->cpu.registers.HL,
                       rl_func_v(this, get_mem_u8(this, this->cpu.registers.HL)));
            break;
        case 0x17: // RL A
            rl_func(this, &this->cpu.registers.A);
            break;
        case 0x18: // RR B
            rr_func(this, &this->cpu.registers.B);
            break;
        case 0x19: // RR C
            rr_func(this, &this->cpu.registers.C);
            break;
        case 0x1a: // RR D
            rr_func(this, &this->cpu.registers.D);
            break;
        case 0x1b: // RR E
            rr_func(this, &this->cpu.registers.E);
            break;
        case 0x1c: // RR H
            rr_func(this, &this->cpu.registers.H);
            break;
        case 0x1d: // RR L
            rr_func(this, &this->cpu.registers.L);
            break;
        case 0x1e: // RR (HL)
            set_mem_u8(this, this->cpu.registers.HL,
                       rr_func_v(this, get_mem_u8(this, this->cpu.registers.HL)));
            break;
        case 0x1f: // RR A
            rr_func(this, &this->cpu.registers.A);
            break;
        case 0x20: // SLA B
            sla_func(this, &this->cpu.registers.B);
            break;
        case 0x21: // SLA C
            sla_func(this, &this->cpu.registers.C);
            break;
        case 0x22: // SLA D
            sla_func(this, &this->cpu.registers.D);
            break;
        case 0x23: // SLA E
            sla_func(this, &this->cpu.registers.E);
            break;
        case 0x24: // SLA H
            sla_func(this, &this->cpu.registers.H);
            break;
        case 0x25: // SLA L
            sla_func(this, &this->cpu.registers.L);
            break;
        case 0x26: // SLA (HL)
            set_mem_u8(this, this->cpu.registers.HL,
                       sla_func_v(this, get_mem_u8(this, this->cpu.registers.HL)));
            break;
        case 0x27: // SLA A
            sla_func(this, &this->cpu.registers.A);
            break;
        case 0x28: // SRA B
            sra_func(this, &this->cpu.registers.B);
            break;
        case 0x29: // SRA C
            sra_func(this, &this->cpu.registers.C);
            break;
        case 0x2a: // SRA D
            sra_func(this, &this->cpu.registers.D);
            break;
        case 0x2b: // SRA E
            sra_func(this, &this->cpu.registers.E);
            break;
        case 0x2c: // SRA H
            sra_func(this, &this->cpu.registers.H);
            break;
        case 0x2d: // SRA L
            sra_func(this, &this->cpu.registers.L);
            break;
        case 0x2e: // SRA (HL)
            set_mem_u8(this, this->cpu.registers.HL,
                       sra_func_v(this, get_mem_u8(this, this->cpu.registers.HL)));
            break;
        case 0x2f: // SRA A
            sra_func(this, &this->cpu.registers.A);
            break;
        case 0x30: // SWAP B
            swap_func(this, &this->cpu.registers.B);
            break;
        case 0x31: // SWAP C
            swap_func(this, &this->cpu.registers.C);
            break;
        case 0x32: // SWAP D
            swap_func(this, &this->cpu.registers.D);
            break;
        case 0x33: // SWAP E
            swap_func(this, &this->cpu.registers.E);
            break;
        case 0x34: // SWAP H
            swap_func(this, &this->cpu.registers.H);
            break;
        case 0x35: // SWAP L
            swap_func(this, &this->cpu.registers.L);
            break;
        case 0x36: // SWAP (HL)
            set_mem_u8(this, this->cpu.registers.HL,
                       swap_func_v(this, get_mem_u8(this, this->cpu.registers.HL)));
            break;
        case 0x37: // SWAP A
            swap_func(this, &this->cpu.registers.A);
            break;
        case 0x38: // SRL B
            srl_func(this, &this->cpu.registers.B);
            break;
        case 0x39: // SRL C
            srl_func(this, &this->cpu.registers.C);
            break;
        case 0x3a: // SRL D
            srl_func(this, &this->cpu.registers.D);
            break;
        case 0x3b: // SRL E
            srl_func(this, &this->cpu.registers.E);
            break;
        case 0x3c: // SRL H
            srl_func(this, &this->cpu.registers.H);
            break;
        case 0x3d: // SRL L
            srl_func(this, &this->cpu.registers.L);
            break;
        case 0x3e: // SRL (HL)
            set_mem_u8(this, this->cpu.registers.HL,
                       srl_func_v(this, get_mem_u8(this, this->cpu.registers.HL)));
            break;
        case 0x3f: // SRL A
            srl_func(this, &this->cpu.registers.A);
            break;
        case 0x40: // BIT 0,B
            bit0_func(this, this->cpu.registers.B);
            break;
        case 0x41: // BIT 0,C
            bit0_func(this, this->cpu.registers.C);
            break;
        case 0x42: // BIT 0,D
            bit0_func(this, this->cpu.registers.D);
            break;
        case 0x43: // BIT 0,E
            bit0_func(this, this->cpu.registers.E);
            break;
        case 0x44: // BIT 0,H
            bit0_func(this, this->cpu.registers.H);
            break;
        case 0x45: // BIT 0,L
            bit0_func(this, this->cpu.registers.L);
            break;
        case 0x46: // BIT 0,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            bit0_func(this, temp_u8);
            break;
        case 0x47: // BIT 0,A
            bit0_func(this, this->cpu.registers.A);
            break;
        case 0x48: // BIT 1,B
            bit1_func(this, this->cpu.registers.B);
            break;
        case 0x49: // BIT 1,C
            bit1_func(this, this->cpu.registers.C);
            break;
        case 0x4a: // BIT 1,D
            bit1_func(this, this->cpu.registers.D);
            break;
        case 0x4b: // BIT 1,E
            bit1_func(this, this->cpu.registers.E);
            break;
        case 0x4c: // BIT 1,H
            bit1_func(this, this->cpu.registers.H);
            break;
        case 0x4d: // BIT 1,L
            bit1_func(this, this->cpu.registers.L);
            break;
        case 0x4e: // BIT 1,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            bit1_func(this, temp_u8);
            break;
        case 0x4f: // BIT 1,A
            bit1_func(this, this->cpu.registers.A);
            break;
        case 0x50: // BIT 2,B
            bit2_func(this, this->cpu.registers.B);
            break;
        case 0x51: // BIT 2,C
            bit2_func(this, this->cpu.registers.C);
            break;
        case 0x52: // BIT 2,D
            bit2_func(this, this->cpu.registers.D);
            break;
        case 0x53: // BIT 2,E
            bit2_func(this, this->cpu.registers.E);
            break;
        case 0x54: // BIT 2,H
            bit2_func(this, this->cpu.registers.H);
            break;
        case 0x55: // BIT 2,L
            bit2_func(this, this->cpu.registers.L);
            break;
        case 0x56: // BIT 2,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            bit2_func(this, temp_u8);
            break;
        case 0x57: // BIT 2,A
            bit2_func(this, this->cpu.registers.A);
            break;
        case 0x58: // BIT 3,B
            bit3_func(this, this->cpu.registers.B);
            break;
        case 0x59: // BIT 3,C
            bit3_func(this, this->cpu.registers.C);
            break;
        case 0x5a: // BIT 3,D
            bit3_func(this, this->cpu.registers.D);
            break;
        case 0x5b: // BIT 3,E
            bit3_func(this, this->cpu.registers.E);
            break;
        case 0x5c: // BIT 3,H
            bit3_func(this, this->cpu.registers.H);
            break;
        case 0x5d: // BIT 3,L
            bit3_func(this, this->cpu.registers.L);
            break;
        case 0x5e: // BIT 3,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            bit3_func(this, temp_u8);
            break;
        case 0x5f: // BIT 3,A
            bit3_func(this, this->cpu.registers.A);
            break;
        case 0x60: // BIT 4,B
            bit4_func(this, this->cpu.registers.B);
            break;
        case 0x61: // BIT 4,C
            bit4_func(this, this->cpu.registers.C);
            break;
        case 0x62: // BIT 4,D
            bit4_func(this, this->cpu.registers.D);
            break;
        case 0x63: // BIT 4,E
            bit4_func(this, this->cpu.registers.E);
            break;
        case 0x64: // BIT 4,H
            bit4_func(this, this->cpu.registers.H);
            break;
        case 0x65: // BIT 4,L
            bit4_func(this, this->cpu.registers.L);
            break;
        case 0x66: // BIT 4,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            bit4_func(this, temp_u8);
            break;
        case 0x67: // BIT 4,A
            bit4_func(this, this->cpu.registers.A);
            break;
        case 0x68: // BIT 5,B
            bit5_func(this, this->cpu.registers.B);
            break;
        case 0x69: // BIT 5,C
            bit5_func(this, this->cpu.registers.C);
            break;
        case 0x6a: // BIT 5,D
            bit5_func(this, this->cpu.registers.D);
            break;
        case 0x6b: // BIT 5,E
            bit5_func(this, this->cpu.registers.E);
            break;
        case 0x6c: // BIT 5,H
            bit5_func(this, this->cpu.registers.H);
            break;
        case 0x6d: // BIT 5,L
            bit5_func(this, this->cpu.registers.L);
            break;
        case 0x6e: // BIT 5,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            bit5_func(this, temp_u8);
            break;
        case 0x6f: // BIT 5,A
            bit5_func(this, this->cpu.registers.A);
            break;
        case 0x70: // BIT 6,B
            bit6_func(this, this->cpu.registers.B);
            break;
        case 0x71: // BIT 6,C
            bit6_func(this, this->cpu.registers.C);
            break;
        case 0x72: // BIT 6,D
            bit6_func(this, this->cpu.registers.D);
            break;
        case 0x73: // BIT 6,E
            bit6_func(this, this->cpu.registers.E);
            break;
        case 0x74: // BIT 6,H
            bit6_func(this, this->cpu.registers.H);
            break;
        case 0x75: // BIT 6,L
            bit6_func(this, this->cpu.registers.L);
            break;
        case 0x76: // BIT 6,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            bit6_func(this, temp_u8);
            break;
        case 0x77: // BIT 6,A
            bit6_func(this, this->cpu.registers.A);
            break;
        case 0x78: // BIT 7,B
            bit7_func(this, this->cpu.registers.B);
            break;
        case 0x79: // BIT 7,C
            bit7_func(this, this->cpu.registers.C);
            break;
        case 0x7a: // BIT 7,D
            bit7_func(this, this->cpu.registers.D);
            break;
        case 0x7b: // BIT 7,E
            bit7_func(this, this->cpu.registers.E);
            break;
        case 0x7c: // BIT 7,H
            bit7_func(this, this->cpu.registers.H);
            break;
        case 0x7d: // BIT 7,L
            bit7_func(this, this->cpu.registers.L);
            break;
        case 0x7e: // BIT 7,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            bit7_func(this, temp_u8);
            break;
        case 0x7f: // BIT 7,A
            bit7_func(this, this->cpu.registers.A);
            break;
        case 0x80: // RES 0,B
            this->cpu.registers.B.b0 = 0;
            break;
        case 0x81: // RES 0,C
            this->cpu.registers.C.b0 = 0;
            break;
        case 0x82: // RES 0,D
            this->cpu.registers.D.b0 = 0;
            break;
        case 0x83: // RES 0,E
            this->cpu.registers.E.b0 = 0;
            break;
        case 0x84: // RES 0,H
            this->cpu.registers.H.b0 = 0;
            break;
        case 0x85: // RES 0,L
            this->cpu.registers.L.b0 = 0;
            break;
        case 0x86: // RES 0,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b0 = 0;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0x87: // RES 0,A
            this->cpu.registers.A.b0 = 0;
            break;
        case 0x88: // RES 1,B
            this->cpu.registers.B.b1 = 0;
            break;
        case 0x89: // RES 1,C
            this->cpu.registers.C.b1 = 0;
            break;
        case 0x8a: // RES 1,D
            this->cpu.registers.D.b1 = 0;
            break;
        case 0x8b: // RES 1,E
            this->cpu.registers.E.b1 = 0;
            break;
        case 0x8c: // RES 1,H
            this->cpu.registers.H.b1 = 0;
            break;
        case 0x8d: // RES 1,L
            this->cpu.registers.L.b1 = 0;
            break;
        case 0x8e: // RES 1,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b1 = 0;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0x8f: // RES 1,A
            this->cpu.registers.A.b1 = 0;
            break;
        case 0x90: // RES 2,B
            this->cpu.registers.B.b2 = 0;
            break;
        case 0x91: // RES 2,C
            this->cpu.registers.C.b2 = 0;
            break;
        case 0x92: // RES 2,D
            this->cpu.registers.D.b2 = 0;
            break;
        case 0x93: // RES 2,E
            this->cpu.registers.E.b2 = 0;
            break;
        case 0x94: // RES 2,H
            this->cpu.registers.H.b2 = 0;
            break;
        case 0x95: // RES 2,L
            this->cpu.registers.L.b2 = 0;
            break;
        case 0x96: // RES 2,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b2 = 0;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0x97: // RES 2,A
            this->cpu.registers.A.b2 = 0;
            break;
        case 0x98: // RES 3,B
            this->cpu.registers.B.b3 = 0;
            break;
        case 0x99: // RES 3,C
            this->cpu.registers.C.b3 = 0;
            break;
        case 0x9a: // RES 3,D
            this->cpu.registers.D.b3 = 0;
            break;
        case 0x9b: // RES 3,E
            this->cpu.registers.E.b3 = 0;
            break;
        case 0x9c: // RES 3,H
            this->cpu.registers.H.b3 = 0;
            break;
        case 0x9d: // RES 3,L
            this->cpu.registers.L.b3 = 0;
            break;
        case 0x9e: // RES 3,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b3 = 0;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0x9f: // RES 3,A
            this->cpu.registers.A.b3 = 0;
            break;
        case 0xa0: // RES 4,B
            this->cpu.registers.B.b4 = 0;
            break;
        case 0xa1: // RES 4,C
            this->cpu.registers.C.b4 = 0;
            break;
        case 0xa2: // RES 4,D
            this->cpu.registers.D.b4 = 0;
            break;
        case 0xa3: // RES 4,E
            this->cpu.registers.E.b4 = 0;
            break;
        case 0xa4: // RES 4,H
            this->cpu.registers.H.b4 = 0;
            break;
        case 0xa5: // RES 4,L
            this->cpu.registers.L.b4 = 0;
            break;
        case 0xa6: // RES 4,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b4 = 0;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xa7: // RES 4,A
            this->cpu.registers.A.b4 = 0;
            break;
        case 0xa8: // RES 5,B
            this->cpu.registers.B.b5 = 0;
            break;
        case 0xa9: // RES 5,C
            this->cpu.registers.C.b5 = 0;
            break;
        case 0xaa: // RES 5,D
            this->cpu.registers.D.b5 = 0;
            break;
        case 0xab: // RES 5,E
            this->cpu.registers.E.b5 = 0;
            break;
        case 0xac: // RES 5,H
            this->cpu.registers.H.b5 = 0;
            break;
        case 0xad: // RES 5,L
            this->cpu.registers.L.b5 = 0;
            break;
        case 0xae: // RES 5,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b5 = 0;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xaf: // RES 5,A
            this->cpu.registers.A.b5 = 0;
            break;
        case 0xb0: // RES 6,B
            this->cpu.registers.B.b6 = 0;
            break;
        case 0xb1: // RES 6,C
            this->cpu.registers.C.b6 = 0;
            break;
        case 0xb2: // RES 6,D
            this->cpu.registers.D.b6 = 0;
            break;
        case 0xb3: // RES 6,E
            this->cpu.registers.E.b6 = 0;
            break;
        case 0xb4: // RES 6,H
            this->cpu.registers.H.b6 = 0;
            break;
        case 0xb5: // RES 6,L
            this->cpu.registers.L.b6 = 0;
            break;
        case 0xb6: // RES 6,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b6 = 0;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xb7: // RES 6,A
            this->cpu.registers.A.b6 = 0;
            break;
        case 0xb8: // RES 7,B
            this->cpu.registers.B.b7 = 0;
            break;
        case 0xb9: // RES 7,C
            this->cpu.registers.C.b7 = 0;
            break;
        case 0xba: // RES 7,D
            this->cpu.registers.D.b7 = 0;
            break;
        case 0xbb: // RES 7,E
            this->cpu.registers.E.b7 = 0;
            break;
        case 0xbc: // RES 7,H
            this->cpu.registers.H.b7 = 0;
            break;
        case 0xbd: // RES 7,L
            this->cpu.registers.L.b7 = 0;
            break;
        case 0xbe: // RES 7,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b7 = 0;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xbf: // RES 7,A
            this->cpu.registers.A.b7 = 0;
            break;
        case 0xc0: // SET 0,B
            this->cpu.registers.B.b0 = 1;
            break;
        case 0xc1: // SET 0,C
            this->cpu.registers.C.b0 = 1;
            break;
        case 0xc2: // SET 0,D
            this->cpu.registers.D.b0 = 1;
            break;
        case 0xc3: // SET 0,E
            this->cpu.registers.E.b0 = 1;
            break;
        case 0xc4: // SET 0,H
            this->cpu.registers.H.b0 = 1;
            break;
        case 0xc5: // SET 0,L
            this->cpu.registers.L.b0 = 1;
            break;
        case 0xc6: // SET 0,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b0 = 1;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xc7: // SET 0,A
            this->cpu.registers.A.b0 = 1;
            break;
        case 0xc8: // SET 1,B
            this->cpu.registers.B.b1 = 1;
            break;
        case 0xc9: // SET 1,C
            this->cpu.registers.C.b1 = 1;
            break;
        case 0xca: // SET 1,D
            this->cpu.registers.D.b1 = 1;
            break;
        case 0xcb: // SET 1,E
            this->cpu.registers.E.b1 = 1;
            break;
        case 0xcc: // SET 1,H
            this->cpu.registers.H.b1 = 1;
            break;
        case 0xcd: // SET 1,L
            this->cpu.registers.L.b1 = 1;
            break;
        case 0xce: // SET 1,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b1 = 1;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xcf: // SET 1,A
            this->cpu.registers.A.b1 = 1;
            break;
        case 0xd0: // SET 2,B
            this->cpu.registers.B.b2 = 1;
            break;
        case 0xd1: // SET 2,C
            this->cpu.registers.C.b2 = 1;
            break;
        case 0xd2: // SET 2,D
            this->cpu.registers.D.b2 = 1;
            break;
        case 0xd3: // SET 2,E
            this->cpu.registers.E.b2 = 1;
            break;
        case 0xd4: // SET 2,H
            this->cpu.registers.H.b2 = 1;
            break;
        case 0xd5: // SET 2,L
            this->cpu.registers.L.b2 = 1;
            break;
        case 0xd6: // SET 2,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b2 = 1;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xd7: // SET 2,A
            this->cpu.registers.A.b2 = 1;
            break;
        case 0xd8: // SET 3,B
            this->cpu.registers.B.b3 = 1;
            break;
        case 0xd9: // SET 3,C
            this->cpu.registers.C.b3 = 1;
            break;
        case 0xda: // SET 3,D
            this->cpu.registers.D.b3 = 1;
            break;
        case 0xdb: // SET 3,E
            this->cpu.registers.E.b3 = 1;
            break;
        case 0xdc: // SET 3,H
            this->cpu.registers.H.b3 = 1;
            break;
        case 0xdd: // SET 3,L
            this->cpu.registers.L.b3 = 1;
            break;
        case 0xde: // SET 3,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b3 = 1;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xdf: // SET 3,A
            this->cpu.registers.A.b3 = 1;
            break;
        case 0xe0: // SET 4,B
            this->cpu.registers.B.b4 = 1;
            break;
        case 0xe1: // SET 4,C
            this->cpu.registers.C.b4 = 1;
            break;
        case 0xe2: // SET 4,D
            this->cpu.registers.D.b4 = 1;
            break;
        case 0xe3: // SET 4,E
            this->cpu.registers.E.b4 = 1;
            break;
        case 0xe4: // SET 4,H
            this->cpu.registers.H.b4 = 1;
            break;
        case 0xe5: // SET 4,L
            this->cpu.registers.L.b4 = 1;
            break;
        case 0xe6: // SET 4,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b4 = 1;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xe7: // SET 4,A
            this->cpu.registers.A.b4 = 1;
            break;
        case 0xe8: // SET 5,B
            this->cpu.registers.B.b5 = 1;
            break;
        case 0xe9: // SET 5,C
            this->cpu.registers.C.b5 = 1;
            break;
        case 0xea: // SET 5,D
            this->cpu.registers.D.b5 = 1;
            break;
        case 0xeb: // SET 5,E
            this->cpu.registers.E.b5 = 1;
            break;
        case 0xec: // SET 5,H
            this->cpu.registers.H.b5 = 1;
            break;
        case 0xed: // SET 5,L
            this->cpu.registers.L.b5 = 1;
            break;
        case 0xee: // SET 5,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b5 = 1;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xef: // SET 5,A
            this->cpu.registers.A.b5 = 1;
            break;
        case 0xf0: // SET 6,B
            this->cpu.registers.B.b6 = 1;
            break;
        case 0xf1: // SET 6,C
            this->cpu.registers.C.b6 = 1;
            break;
        case 0xf2: // SET 6,D
            this->cpu.registers.D.b6 = 1;
            break;
        case 0xf3: // SET 6,E
            this->cpu.registers.E.b6 = 1;
            break;
        case 0xf4: // SET 6,H
            this->cpu.registers.H.b6 = 1;
            break;
        case 0xf5: // SET 6,L
            this->cpu.registers.L.b6 = 1;
            break;
        case 0xf6: // SET 6,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b6 = 1;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xf7: // SET 6,A
            this->cpu.registers.A.b6 = 1;
            break;
        case 0xf8: // SET 7,B
            this->cpu.registers.B.b7 = 1;
            break;
        case 0xf9: // SET 7,C
            this->cpu.registers.C.b7 = 1;
            break;
        case 0xfa: // SET 7,D
            this->cpu.registers.D.b7 = 1;
            break;
        case 0xfb: // SET 7,E
            this->cpu.registers.E.b7 = 1;
            break;
        case 0xfc: // SET 7,H
            this->cpu.registers.H.b7 = 1;
            break;
        case 0xfd: // SET 7,L
            this->cpu.registers.L.b7 = 1;
            break;
        case 0xfe: // SET 7,(HL)
            temp_u8.byte = get_mem_u8(this, this->cpu.registers.HL);
            temp_u8.b7 = 1;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8.byte);
            break;
        case 0xff: // SET 7,A
            this->cpu.registers.A.b7 = 1;
            break;
        default:
            fprintf(stderr, "Unknown CB opcode: 0x%02x at 0x%04x\n",
                    cb_opcode, this->cpu.registers.PC - 1);
            print_debug(this, EXIT_FAILURE);
    }
    return CBOpcodeTiming[cb_opcode];
}

struct Byte rlc_func_v(struct Context *this, struct Byte value) {
    struct Byte temp_u8;
    temp_u8.byte = (value.byte << 1) | value.b7;
    this->cpu.registers.flags.zf = (temp_u8.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    this->cpu.registers.flags.cy = temp_u8.b0;
    return temp_u8;
}

void rlc_func(struct Context *this, struct Byte *reg) {
    *reg = rlc_func_v(this, *reg);
}

uint8_t rrc_func_v(struct Context *this, uint8_t value) {
    uint8_t temp_u8 = (value >> 1) | (value << 7);
    this->cpu.registers.flags.zf = (temp_u8 == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    this->cpu.registers.flags.cy = temp_u8 >> 7;
    return temp_u8;
}

void rrc_func(struct Context *this, struct Byte *reg) {
    reg->byte = rrc_func_v(this, reg->byte);
}

uint8_t rl_func_v(struct Context *this, uint8_t value) {
    uint16_t temp_u16 = (value << 1) | this->cpu.registers.flags.cy;
    uint8_t temp_u8 = temp_u16;
    this->cpu.registers.flags.zf = (temp_u8 == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    this->cpu.registers.flags.cy = temp_u16 >> 8;
    return temp_u8;
}

void rl_func(struct Context *this, struct Byte *reg) {
    reg->byte = rl_func_v(this, reg->byte);
}

uint8_t rr_func_v(struct Context *this, uint8_t value) {
    uint8_t temp_u8 = (this->cpu.registers.flags.cy << 7) | (value >> 1);
    this->cpu.registers.flags.cy = value;
    this->cpu.registers.flags.zf = (temp_u8 == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    return temp_u8;
}

void rr_func(struct Context *this, struct Byte *reg) {
    reg->byte = rr_func_v(this, reg->byte);
}

uint8_t sla_func_v(struct Context *this, uint8_t value) {
    this->cpu.registers.flags.cy = value >> 7;
    uint8_t temp_u8 = value << 1;
    this->cpu.registers.flags.zf = (temp_u8 == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    return temp_u8;
}

void sla_func(struct Context *this, struct Byte *reg) {
    reg->byte = sla_func_v(this, reg->byte);
}

uint8_t sra_func_v(struct Context *this, uint8_t value) {
    this->cpu.registers.flags.cy = value;
    uint8_t temp_u8 = (value >> 1) | (value & 0x80);
    this->cpu.registers.flags.zf = (temp_u8 == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    return temp_u8;
}

void sra_func(struct Context *this, struct Byte *reg) {
    reg->byte = sra_func_v(this, reg->byte);
}

uint8_t swap_func_v(struct Context *this, uint8_t value) {
    uint8_t temp_u8 = (value << 4) | (value >> 4);
    this->cpu.registers.F = 0;
    this->cpu.registers.flags.zf = (temp_u8 == 0) ? 1 : 0;
    return temp_u8;
}

void swap_func(struct Context *this, struct Byte *reg) {
    reg->byte = swap_func_v(this, reg->byte);
}

uint8_t srl_func_v(struct Context *this, uint8_t value) {
    this->cpu.registers.flags.cy = value;
    uint8_t temp_u8 = value >> 1;
    this->cpu.registers.flags.zf = (temp_u8 == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 0;
    return temp_u8;
}

void srl_func(struct Context *this, struct Byte *reg) {
    reg->byte = srl_func_v(this, reg->byte);
}

void bit0_func(struct Context *this, struct Byte value) {
    this->cpu.registers.flags.zf = !value.b0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 1;
}

void bit1_func(struct Context *this, struct Byte value) {
    this->cpu.registers.flags.zf = !value.b1;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 1;
}

void bit2_func(struct Context *this, struct Byte value) {
    this->cpu.registers.flags.zf = !value.b2;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 1;
}

void bit3_func(struct Context *this, struct Byte value) {
    this->cpu.registers.flags.zf = !value.b3;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 1;
}

void bit4_func(struct Context *this, struct Byte value) {
    this->cpu.registers.flags.zf = !value.b4;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 1;
}

void bit5_func(struct Context *this, struct Byte value) {
    this->cpu.registers.flags.zf = !value.b5;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 1;
}

void bit6_func(struct Context *this, struct Byte value) {
    this->cpu.registers.flags.zf = !value.b6;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 1;
}

void bit7_func(struct Context *this, struct Byte value) {
    this->cpu.registers.flags.zf = !value.b7;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 1;
}
