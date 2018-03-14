#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "definitions.h"
#include "opcodes.h"
#include "memory.h"
#include "cb_prefix.h"
#include "debug.h"
#include "cpu.h"

static uint8_t OpcodesTiming[] = {
    4, 12, 8, 8, 4, 4, 8, 4, 20, 8, 8, 8, 4, 4, 8, 4, // 0x
    4, 12, 8, 8, 4, 4, 8, 4, 12, 8, 8, 8, 4, 4, 8, 4, // 1x
    12/*8*/, 12, 8, 8, 4, 4, 8, 4, 12/*8*/, 8, 8, 8, 4, 4, 8, 4,  // 2x
    12/*8*/, 12, 8, 8, 12, 12, 12, 4, 12/*8*/, 8, 8, 8, 4, 4, 8, 4,  // 3x
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4, // 4x
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4, // 5x
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4, // 6x
    8, 8, 8, 8, 8, 8, 0, 8, 4, 4, 4, 4, 4, 4, 8, 4, // 7x
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4, // 8x
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4, // 9x
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4, // Ax
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4, // Bx
    20/*8*/, 12, 16/*12*/, 16, 24/*12*/, 16, 8, 16, 20/*8*/, 16, 16/*12*/, 0, 24/*12*/, 24, 8, 16, // Cx
    20/*8*/, 12, 16/*12*/, 0, 24/*12*/, 16, 8, 16, 20/*8*/, 16, 16/*12*/, 0, 24/*12*/, 0, 8, 16, // Dx
    12, 12, 8, 0, 0, 16, 8, 16, 16, 4, 16, 0, 0, 0, 8, 16, // Ex
    12, 12, 8, 4, 0, 16, 8, 16, 12, 8, 16, 4, 0, 0, 8, 16 // Fx
};

void inc_func(struct Context *this, struct Byte *reg);
void dec_func(struct Context *this, struct Byte *reg);
void add_a_func(struct Context *this, struct Byte *reg_src);
void adc_a_func(struct Context *this, struct Byte *reg_src);
void sub_func(struct Context *this, struct Byte *reg_src);
void sbc_a_func(struct Context *this, struct Byte *reg_src);
void and_func(struct Context *this, uint8_t value);
void xor_func(struct Context *this, uint8_t value);
void or_func(struct Context *this, uint8_t value);
void cp_func(struct Context *this, uint8_t value);

uint8_t cpu_execute(struct Context *this, const struct Opcode opcode) {
    uint8_t temp_u8;
    uint16_t temp_u16;
    uint32_t temp_u32;
    uint8_t timing = OpcodesTiming[opcode.code];

    switch (opcode.code) {
        case 0x00: // NOP
            break;
        case 0x01: // LD BC,d16
            this->cpu.registers.BC = opcode.u16;
            break;
        case 0x02: // LD (BC),A
            set_mem_u8(this, this->cpu.registers.BC, this->cpu.registers.A.byte);
            break;
        case 0x03: // INC BC
            this->cpu.registers.BC++;
            break;
        case 0x04: // INC B
            inc_func(this, &this->cpu.registers.B);
            break;
        case 0x05: // DEC B
            dec_func(this, &this->cpu.registers.B);
            break;
        case 0x06: // LD B,d8
            this->cpu.registers.B.byte = opcode.u8;
            break;
        case 0x07: // RLCA
            temp_u16 = (this->cpu.registers.A.byte << 1) | (this->cpu.registers.A.byte >> 7);
            this->cpu.registers.flags.cy = temp_u16 >> 8;
            this->cpu.registers.A.byte = temp_u16;
            this->cpu.registers.flags.zf = 0;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.h = 0;
            break;
        case 0x08: // LD (a16),SP
            set_mem_u16(this, opcode.u16, this->cpu.registers.SP);
            break;
        case 0x09: // ADD HL,BC
            this->cpu.registers.flags.h = ((this->cpu.registers.HL & 0xfff) + (this->cpu.registers.BC & 0xfff)) >> 12;
            temp_u32 = this->cpu.registers.HL + this->cpu.registers.BC;
            this->cpu.registers.HL = temp_u32;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.cy = temp_u32 >> 16;
            break;
        case 0x0a: // LD A,(BC)
            this->cpu.registers.A.byte = get_mem_u8(this, this->cpu.registers.BC);
            break;
        case 0x0b: // DEC BC
            this->cpu.registers.BC--;
            break;
        case 0x0c: // INC C
            inc_func(this, &this->cpu.registers.C);
            break;
        case 0x0d: // DEC C
            dec_func(this, &this->cpu.registers.C);
            break;
        case 0x0e: // LD C,d8
            this->cpu.registers.C.byte = opcode.u8;
            break;
        case 0x0f: // RRCA
            temp_u16 = (this->cpu.registers.A.byte >> 1) | ((this->cpu.registers.A.byte & 1) << 7);
            this->cpu.registers.flags.cy = temp_u16 >> 7;
            this->cpu.registers.A.byte = temp_u16;
            this->cpu.registers.flags.zf = 0;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.h = 0;
            break;
        case 0x10: // STOP
            printf("STOP\n");
            break;
        case 0x11: // LD DE,d16
            this->cpu.registers.DE = opcode.u16;
            break;
        case 0x12: // LD (DE),A
            set_mem_u8(this, this->cpu.registers.DE, this->cpu.registers.A.byte);
            break;
        case 0x13: // INC DE
            this->cpu.registers.DE++;
            break;
        case 0x14: // INC D
            inc_func(this, &this->cpu.registers.D);
            break;
        case 0x15: // DEC D
            dec_func(this, &this->cpu.registers.D);
            break;
        case 0x16: // LD D,d8
            this->cpu.registers.D.byte = opcode.u8;
            break;
        case 0x17: // RLA
            temp_u16 = (this->cpu.registers.A.byte << 1) | this->cpu.registers.flags.cy;
            this->cpu.registers.flags.cy = temp_u16 >> 8;
            this->cpu.registers.A.byte = temp_u16;
            this->cpu.registers.flags.zf = 0;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.h = 0;
            break;
        case 0x18: // JR r8
            this->cpu.registers.PC += opcode.i8;
            break;
        case 0x19: // ADD HL,DE
            this->cpu.registers.flags.h = ((this->cpu.registers.HL & 0xfff) + (this->cpu.registers.DE & 0xfff)) >> 12;
            temp_u32 = this->cpu.registers.HL + this->cpu.registers.DE;
            this->cpu.registers.HL = temp_u32;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.cy = temp_u32 >> 16;
            break;
        case 0x1a: // LD A,(DE)
            this->cpu.registers.A.byte = get_mem_u8(this, this->cpu.registers.DE);
            break;
        case 0x1b: // DEC DE
            this->cpu.registers.DE--;
            break;
        case 0x1c: // INC E
            inc_func(this, &this->cpu.registers.E);
            break;
        case 0x1d: // DEC E
            dec_func(this, &this->cpu.registers.E);
            break;
        case 0x1e: // LD E,d8
            this->cpu.registers.E.byte = opcode.u8;
            break;
        case 0x1f: // RRA
            temp_u16 = (this->cpu.registers.flags.cy << 8) | this->cpu.registers.A.byte;
            this->cpu.registers.flags.cy = temp_u16 & 1;
            temp_u16 >>= 1;
            this->cpu.registers.A.byte = temp_u16;
            this->cpu.registers.flags.zf = 0;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.h = 0;
            break;
        case 0x20: // JR NZ,n
            if (!this->cpu.registers.flags.zf) {
                this->cpu.registers.PC += opcode.i8;
            } else timing -= 4;
            break;
        case 0x21: // LD HL,d16
            this->cpu.registers.HL = opcode.u16;
            break;
        case 0x22: // LD (HL+),A
            set_mem_u8(this, this->cpu.registers.HL, this->cpu.registers.A.byte);
            this->cpu.registers.HL++;
            break;
        case 0x23: // INC HL
            this->cpu.registers.HL++;
            break;
        case 0x24: // INC H
            inc_func(this, &this->cpu.registers.H);
            break;
        case 0x25: // DEC H
            dec_func(this, &this->cpu.registers.H);
            break;
        case 0x26: // LD H,d8
            this->cpu.registers.H.byte = opcode.u8;
            break;
        case 0x27: // DAA
            temp_u16 = this->cpu.registers.A.byte;
            if (this->cpu.registers.flags.n) {
                if (this->cpu.registers.flags.h) {
                    temp_u16 -= 6;
                    if (!this->cpu.registers.flags.cy) {
                        temp_u16 &= 0xff;
                    }
                }
                if (this->cpu.registers.flags.cy) {
                    temp_u16 -= 0x60;
                }
            } else {
                if (this->cpu.registers.flags.h || (temp_u16 & 0x0f) > 9) {
                    temp_u16 += 0x06;
                }
                if (this->cpu.registers.flags.cy || (temp_u16 >> 4) > 9) {
                    temp_u16 += 0x60;
                }
            }
            this->cpu.registers.A.byte = temp_u16;
            if (temp_u16 >> 8) {
                this->cpu.registers.flags.cy = 1;
            }
            this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
            this->cpu.registers.flags.h = 0;
            break;
        case 0x28: // JR Z,r8
            if (this->cpu.registers.flags.zf) {
                this->cpu.registers.PC += opcode.i8;
            } else timing -= 4;
            break;
        case 0x29: // ADD HL,HL
            this->cpu.registers.flags.h = ((this->cpu.registers.HL & 0xfff) + (this->cpu.registers.HL & 0xfff)) >> 12;
            temp_u32 = this->cpu.registers.HL + this->cpu.registers.HL;
            this->cpu.registers.HL = temp_u32;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.cy = temp_u32 >> 16;
            break;
        case 0x2a: // LD A,(HL+)
            this->cpu.registers.A.byte = get_mem_u8(this, this->cpu.registers.HL);
            this->cpu.registers.HL++;
            break;
        case 0x2b: // DEC HL
            this->cpu.registers.HL--;
            break;
        case 0x2c: // INC L
            inc_func(this, &this->cpu.registers.L);
            break;
        case 0x2d: // DEC L
            dec_func(this, &this->cpu.registers.L);
            break;
        case 0x2e: // LD L,d8
            this->cpu.registers.L.byte = opcode.u8;
            break;
        case 0x2f: // CPL
            this->cpu.registers.flags.n = 1;
            this->cpu.registers.flags.h = 1;
            this->cpu.registers.A.byte = ~this->cpu.registers.A.byte;
            break;
        case 0x30: // JR NC,r8
            if (!this->cpu.registers.flags.cy) {
                this->cpu.registers.PC += opcode.i8;
            } else timing -= 4;
            break;
        case 0x31: // LD SP,d16
            this->cpu.registers.SP = opcode.u16;
            break;
        case 0x32: // LD (HL-),A
            set_mem_u8(this, this->cpu.registers.HL, this->cpu.registers.A.byte);
            this->cpu.registers.HL--;
            break;
        case 0x33: // INC SP
            this->cpu.registers.SP++;
            break;
        case 0x34: // INC (HL)
            temp_u8 = get_mem_u8(this, this->cpu.registers.HL);
            this->cpu.registers.flags.h = ((temp_u8 & 0x0f) + 1) >> 4;
            temp_u8++;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8);
            this->cpu.registers.flags.zf = (temp_u8 == 0) ? 1 : 0;
            this->cpu.registers.flags.n = 0;
            break;
        case 0x35: // DEC (HL)
            temp_u8 = get_mem_u8(this, this->cpu.registers.HL);
            this->cpu.registers.flags.h = ((temp_u8 & 0x0f) == 0) ? 1 : 0;
            temp_u8--;
            set_mem_u8(this, this->cpu.registers.HL, temp_u8);
            this->cpu.registers.flags.zf = (temp_u8 == 0) ? 1 : 0;
            this->cpu.registers.flags.n = 1;
            break;
        case 0x36: // LD (HL),d8
            set_mem_u8(this, this->cpu.registers.HL, opcode.u8);
            break;
        case 0x37: // SCF
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.h = 0;
            this->cpu.registers.flags.cy = 1;
            break;
        case 0x38: // JR C,r8
            if (this->cpu.registers.flags.cy) {
                this->cpu.registers.PC += opcode.i8;
            } else timing -= 4;
            break;
        case 0x39: // ADD HL,SP
            temp_u32 = this->cpu.registers.HL + this->cpu.registers.SP;
            this->cpu.registers.flags.h = ((this->cpu.registers.HL & 0xfff) + (this->cpu.registers.SP & 0xfff)) >> 12;
            this->cpu.registers.HL = temp_u32;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.cy = temp_u32 >> 16;
            break;
        case 0x3a: // LD A,(HL-)
            this->cpu.registers.A.byte = get_mem_u8(this, this->cpu.registers.HL);
            this->cpu.registers.HL--;
            break;
        case 0x3b: // DEC SP
            this->cpu.registers.SP--;
            break;
        case 0x3c: // INC A
            inc_func(this, &this->cpu.registers.A);
            break;
        case 0x3d: // DEC A
            dec_func(this, &this->cpu.registers.A);
            break;
        case 0x3e: // LD A,d8
            this->cpu.registers.A.byte = opcode.u8;
            break;
        case 0x3f: // CCF
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.h = 0;
            this->cpu.registers.flags.cy ^= 1; // CHECK
            break;
        case 0x40: // LD B,B
            //this->cpu.registers.B = this->cpu.registers.B;
            break;
        case 0x41: // LD B,C
            this->cpu.registers.B = this->cpu.registers.C;
            break;
        case 0x42: // LD B,D
            this->cpu.registers.B = this->cpu.registers.D;
            break;
        case 0x43: // LD B,E
            this->cpu.registers.B = this->cpu.registers.E;
            break;
        case 0x44: // LD B,H
            this->cpu.registers.B = this->cpu.registers.H;
            break;
        case 0x45: // LD B,L
            this->cpu.registers.B = this->cpu.registers.L;
            break;
        case 0x46: // LD B,(HL)
            this->cpu.registers.B.byte = get_mem_u8(this, this->cpu.registers.HL);
            break;
        case 0x47: // LD B,A
            this->cpu.registers.B = this->cpu.registers.A;
            break;
        case 0x48: // LD C,B
            this->cpu.registers.C = this->cpu.registers.B;
            break;
        case 0x49: // LD C,C
            //this->cpu.registers.C = this->cpu.registers.C;
            break;
        case 0x4a: // LD C,D
            this->cpu.registers.C = this->cpu.registers.D;
            break;
        case 0x4b: // LD C,E
            this->cpu.registers.C = this->cpu.registers.E;
            break;
        case 0x4c: // LD C,H
            this->cpu.registers.C = this->cpu.registers.H;
            break;
        case 0x4d: // LD C,L
            this->cpu.registers.C = this->cpu.registers.L;
            break;
        case 0x4e: // LD C,(HL)
            this->cpu.registers.C.byte = get_mem_u8(this, this->cpu.registers.HL);
            break;
        case 0x4f: // LD C,A
            this->cpu.registers.C = this->cpu.registers.A;
            break;
        case 0x50: // LD D,B
            this->cpu.registers.D = this->cpu.registers.B;
            break;
        case 0x51: // LD D,C
            this->cpu.registers.D = this->cpu.registers.C;
            break;
        case 0x52: // LD D,D
            //this->cpu.registers.D = this->cpu.registers.D;
            break;
        case 0x53: // LD D,E
            this->cpu.registers.D = this->cpu.registers.E;
            break;
        case 0x54: // LD D,H
            this->cpu.registers.D = this->cpu.registers.H;
            break;
        case 0x55: // LD D,L
            this->cpu.registers.D = this->cpu.registers.L;
            break;
        case 0x56: // LD D,(HL)
            this->cpu.registers.D.byte = get_mem_u8(this, this->cpu.registers.HL);
            break;
        case 0x57: // LD D,A
            this->cpu.registers.D = this->cpu.registers.A;
            break;
        case 0x58: // LD E,B
            this->cpu.registers.E = this->cpu.registers.B;
            break;
        case 0x59: // LD E,C
            this->cpu.registers.E = this->cpu.registers.C;
            break;
        case 0x5a: // LD E,D
            this->cpu.registers.E = this->cpu.registers.D;
            break;
        case 0x5b: // LD E,E
            //this->cpu.registers.E = this->cpu.registers.E;
            break;
        case 0x5c: // LD E,H
            this->cpu.registers.E = this->cpu.registers.H;
            break;
        case 0x5d: // LD E,L
            this->cpu.registers.E = this->cpu.registers.L;
            break;
        case 0x5e: // LD E,(HL)
            this->cpu.registers.E.byte = get_mem_u8(this, this->cpu.registers.HL);
            break;
        case 0x5f: // LD E,A
            this->cpu.registers.E = this->cpu.registers.A;
            break;
        case 0x60: // LD H,B
            this->cpu.registers.H = this->cpu.registers.B;
            break;
        case 0x61: // LD H,C
            this->cpu.registers.H = this->cpu.registers.C;
            break;
        case 0x62: // LD H,D
            this->cpu.registers.H = this->cpu.registers.D;
            break;
        case 0x63: // LD H,E
            this->cpu.registers.H = this->cpu.registers.E;
            break;
        case 0x64: // LD H,H
            this->cpu.registers.H = this->cpu.registers.H;
            break;
        case 0x65: // LD H,L
            this->cpu.registers.H = this->cpu.registers.L;
            break;
        case 0x66: // LD H,(HL)
            this->cpu.registers.H.byte = get_mem_u8(this, this->cpu.registers.HL);
            break;
        case 0x67: // LD H,A
            this->cpu.registers.H = this->cpu.registers.A;
            break;
        case 0x68: // LD L,B
            this->cpu.registers.L = this->cpu.registers.B;
            break;
        case 0x69: // LD L,C
            this->cpu.registers.L = this->cpu.registers.C;
            break;
        case 0x6a: // LD L,D
            this->cpu.registers.L = this->cpu.registers.D;
            break;
        case 0x6b: // LD L,E
            this->cpu.registers.L = this->cpu.registers.E;
            break;
        case 0x6c: // LD L,H
            this->cpu.registers.L = this->cpu.registers.H;
            break;
        case 0x6d: // LD L,L
            //this->cpu.registers.L = this->cpu.registers.L;
            break;
        case 0x6e: // LD L,(HL)
            this->cpu.registers.L.byte = get_mem_u8(this, this->cpu.registers.HL);
            break;
        case 0x6f: // LD L,A
            this->cpu.registers.L = this->cpu.registers.A;
            break;
        case 0x70: // LD (HL),B
            set_mem_u8(this, this->cpu.registers.HL, this->cpu.registers.B.byte);
            break;
        case 0x71: // LD (HL),C
            set_mem_u8(this, this->cpu.registers.HL, this->cpu.registers.C.byte);
            break;
        case 0x72: // LD (HL),D
            set_mem_u8(this, this->cpu.registers.HL, this->cpu.registers.D.byte);
            break;
        case 0x73: // LD (HL),E
            set_mem_u8(this, this->cpu.registers.HL, this->cpu.registers.E.byte);
            break;
        case 0x74: // LD (HL),H
            set_mem_u8(this, this->cpu.registers.HL, this->cpu.registers.H.byte);
            break;
        case 0x75: // LD (HL),L
            set_mem_u8(this, this->cpu.registers.HL, this->cpu.registers.L.byte);
            break;
        case 0x76: // HALT
            if (!this->interrupts.IME &&
                (this->interrupts.IE.raw & this->interrupts.IF.raw & 0x1f) != 0) {
                this->cpu.halt_bug = true;
            } else {
                this->cpu_halted = true;
                if ((this->interrupts.IE.raw & 0x1f) == 0) {
                    fprintf(stderr, "CPU halted but no interrupts are enabled!\n");
                    print_debug(this, EXIT_FAILURE);
                }
            }
            break;
        case 0x77: // LD (HL),A
            set_mem_u8(this, this->cpu.registers.HL, this->cpu.registers.A.byte);
            break;
        case 0x78: // LD A,B
            this->cpu.registers.A = this->cpu.registers.B;
            break;
        case 0x79: // LD A,C
            this->cpu.registers.A = this->cpu.registers.C;
            break;
        case 0x7a: // LD A,D
            this->cpu.registers.A = this->cpu.registers.D;
            break;
        case 0x7b: // LD A,E
            this->cpu.registers.A = this->cpu.registers.E;
            break;
        case 0x7c: // LD A,H
            this->cpu.registers.A = this->cpu.registers.H;
            break;
        case 0x7d: // LD A,L
            this->cpu.registers.A = this->cpu.registers.L;
            break;
        case 0x7e: // LD A,(HL)
            this->cpu.registers.A.byte = get_mem_u8(this, this->cpu.registers.HL);
            break;
        case 0x7f: // LD A,A
            //this->cpu.registers.A = this->cpu.registers.A;
            break;
        case 0x80: // ADD A,B
            add_a_func(this, &this->cpu.registers.B);
            break;
        case 0x81: // ADD A,C
            add_a_func(this, &this->cpu.registers.C);
            break;
        case 0x82: // ADD A,D
            add_a_func(this, &this->cpu.registers.D);
            break;
        case 0x83: // ADD A,E
            add_a_func(this, &this->cpu.registers.E);
            break;
        case 0x84: // ADD A,H
            add_a_func(this, &this->cpu.registers.H);
            break;
        case 0x85: // ADD A,L
            add_a_func(this, &this->cpu.registers.L);
            break;
        case 0x86: // ADD A,(HL)
            temp_u8 = get_mem_u8(this, this->cpu.registers.HL);
            this->cpu.registers.flags.h = (this->cpu.registers.A.n0 + (temp_u8 & 0x0f)) >> 4;
            temp_u16 = this->cpu.registers.A.byte + temp_u8;
            this->cpu.registers.A.byte = temp_u16;
            this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.cy = temp_u16 >> 8;
            break;
        case 0x87: // ADD A,A
            add_a_func(this, &this->cpu.registers.A);
            break;
        case 0x88: // ADC A,B
            adc_a_func(this, &this->cpu.registers.B);
            break;
        case 0x89: // ADC A,C
            adc_a_func(this, &this->cpu.registers.C);
            break;
        case 0x8a: // ADC A,D
            adc_a_func(this, &this->cpu.registers.D);
            break;
        case 0x8b: // ADC A,E
            adc_a_func(this, &this->cpu.registers.E);
            break;
        case 0x8c: // ADC A,H
            adc_a_func(this, &this->cpu.registers.H);
            break;
        case 0x8d: // ADC A,L
            adc_a_func(this, &this->cpu.registers.L);
            break;
        case 0x8e: // ADC A,(HL)
            temp_u8 = get_mem_u8(this, this->cpu.registers.HL);
            this->cpu.registers.flags.h = (this->cpu.registers.A.n0 + this->cpu.registers.flags.cy + (temp_u8 & 0x0f)) >> 4;
            temp_u16 = this->cpu.registers.A.byte + this->cpu.registers.flags.cy + temp_u8;
            this->cpu.registers.A.byte = temp_u16;
            this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.cy = temp_u16 >> 8;
            break;
        case 0x8f: // ADC A,A
            adc_a_func(this, &this->cpu.registers.A);
            break;
        case 0x90: // SUB B
            sub_func(this, &this->cpu.registers.B);
            break;
        case 0x91: // SUB C
            sub_func(this, &this->cpu.registers.C);
            break;
        case 0x92: // SUB D
            sub_func(this, &this->cpu.registers.D);
            break;
        case 0x93: // SUB E
            sub_func(this, &this->cpu.registers.E);
            break;
        case 0x94: // SUB H
            sub_func(this, &this->cpu.registers.H);
            break;
        case 0x95: // SUB L
            sub_func(this, &this->cpu.registers.L);
            break;
        case 0x96: // SUB (HL)
            temp_u8 = get_mem_u8(this, this->cpu.registers.HL);
            this->cpu.registers.flags.h = this->cpu.registers.A.n0 < (temp_u8 & 0x0f);
            this->cpu.registers.flags.cy = this->cpu.registers.A.byte < temp_u8;
            this->cpu.registers.A.byte -= temp_u8;
            this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
            this->cpu.registers.flags.n = 1;
            break;
        case 0x97: // SUB A
            sub_func(this, &this->cpu.registers.A);
            break;
        case 0x98: // SBC A,B
            sbc_a_func(this, &this->cpu.registers.B);
            break;
        case 0x99: // SBC A,C
            sbc_a_func(this, &this->cpu.registers.C);
            break;
        case 0x9a: // SBC A,D
            sbc_a_func(this, &this->cpu.registers.D);
            break;
        case 0x9b: // SBC A,E
            sbc_a_func(this, &this->cpu.registers.E);
            break;
        case 0x9c: // SBC A,H
            sbc_a_func(this, &this->cpu.registers.H);
            break;
        case 0x9d: // SBC A,L
            sbc_a_func(this, &this->cpu.registers.L);
            break;
        case 0x9e: // SBC A,(HL)
            temp_u8 = get_mem_u8(this, this->cpu.registers.HL);
            this->cpu.registers.flags.h = (this->cpu.registers.A.n0 < ((temp_u8 & 0x0f) + this->cpu.registers.flags.cy)) ? 1 : 0;
            temp_u16 = temp_u8 + this->cpu.registers.flags.cy;
            this->cpu.registers.flags.cy = (this->cpu.registers.A.byte < temp_u16) ? 1 : 0;
            this->cpu.registers.A.byte -= temp_u16;
            this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
            this->cpu.registers.flags.n = 1;
            break;
        case 0x9f: // SBC A,A
            sbc_a_func(this, &this->cpu.registers.A);
            break;
        case 0xa0: // AND B
            and_func(this, this->cpu.registers.B.byte);
            break;
        case 0xa1: // AND C
            and_func(this, this->cpu.registers.C.byte);
            break;
        case 0xa2: // AND D
            and_func(this, this->cpu.registers.D.byte);
            break;
        case 0xa3: // AND E
            and_func(this, this->cpu.registers.E.byte);
            break;
        case 0xa4: // AND H
            and_func(this, this->cpu.registers.H.byte);
            break;
        case 0xa5: // AND L
            and_func(this, this->cpu.registers.L.byte);
            break;
        case 0xa6: // AND (HL)
            and_func(this, get_mem_u8(this, this->cpu.registers.HL));
            break;
        case 0xa7: // AND A
            and_func(this, this->cpu.registers.A.byte);
            break;
        case 0xa8: // XOR B
            xor_func(this, this->cpu.registers.B.byte);
            break;
        case 0xa9: // XOR C
            xor_func(this, this->cpu.registers.C.byte);
            break;
        case 0xaa: // XOR D
            xor_func(this, this->cpu.registers.D.byte);
            break;
        case 0xab: // XOR E
            xor_func(this, this->cpu.registers.E.byte);
            break;
        case 0xac: // XOR H
            xor_func(this, this->cpu.registers.H.byte);
            break;
        case 0xad: // XOR L
            xor_func(this, this->cpu.registers.L.byte);
            break;
        case 0xae: // XOR (HL)
            xor_func(this, get_mem_u8(this, this->cpu.registers.HL));
            break;
        case 0xaf: // XOR A
            xor_func(this, this->cpu.registers.A.byte);
            break;
        case 0xb0: // OR B
            or_func(this, this->cpu.registers.B.byte);
            break;
        case 0xb1: // OR C
            or_func(this, this->cpu.registers.C.byte);
            break;
        case 0xb2: // OR D
            or_func(this, this->cpu.registers.D.byte);
            break;
        case 0xb3: // OR E
            or_func(this, this->cpu.registers.E.byte);
            break;
        case 0xb4: // OR H
            or_func(this, this->cpu.registers.H.byte);
            break;
        case 0xb5: // OR L
            or_func(this, this->cpu.registers.L.byte);
            break;
        case 0xb6: // OR (HL)
            or_func(this, get_mem_u8(this, this->cpu.registers.HL));
            break;
        case 0xb7: // OR A
            or_func(this, this->cpu.registers.A.byte);
            break;
        case 0xb8: // CP B
            cp_func(this, this->cpu.registers.B.byte);
            break;
        case 0xb9: // CP C
            cp_func(this, this->cpu.registers.C.byte);
            break;
        case 0xba: // CP D
            cp_func(this, this->cpu.registers.D.byte);
            break;
        case 0xbb: // CP E
            cp_func(this, this->cpu.registers.E.byte);
            break;
        case 0xbc: // CP H
            cp_func(this, this->cpu.registers.H.byte);
            break;
        case 0xbd: // CP L
            cp_func(this, this->cpu.registers.L.byte);
            break;
        case 0xbe: // CP (HL)
            cp_func(this, get_mem_u8(this, this->cpu.registers.HL));
            break;
        case 0xbf: // CP A
            cp_func(this, this->cpu.registers.A.byte);
            break;
        case 0xc0: // RET NZ
            if (!this->cpu.registers.flags.zf) {
                this->cpu.registers.PC = pop16(this);
            } else timing -= 12;
            break;
        case 0xc1: // POP BC
            this->cpu.registers.BC = pop16(this);
            break;
        case 0xc2: // JP NZ,a16
            if (!this->cpu.registers.flags.zf) {
                this->cpu.registers.PC = opcode.u16;
            } else timing -= 4;
            break;
        case 0xc3: // JP a16
            this->cpu.registers.PC = opcode.u16;
            break;
        case 0xc4: // CALL NZ,a16
            if (!this->cpu.registers.flags.zf) {
                push16(this, this->cpu.registers.PC);
                this->cpu.registers.PC = opcode.u16;
            } else timing -= 12;
            break;
        case 0xc5: // PUSH BC
            push16(this, this->cpu.registers.BC);
            break;
        case 0xc6: // ADD A,d8
            this->cpu.registers.flags.h = (this->cpu.registers.A.n0 + (opcode.u8 & 0x0f)) >> 4; // CHECK
            temp_u16 = this->cpu.registers.A.byte + opcode.u8;
            this->cpu.registers.A.byte = temp_u16;
            this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.cy = temp_u16 >> 8;
            break;
        case 0xc7: // RST 00H
            push16(this, this->cpu.registers.PC);
            this->cpu.registers.PC = 0x0000;
            break;
        case 0xc8: // RET Z
            if (this->cpu.registers.flags.zf) {
                this->cpu.registers.PC = pop16(this);
            } else timing -= 12;
            break;
        case 0xc9: // RET
            this->cpu.registers.PC = pop16(this);
            break;
        case 0xca: // JP Z,a16
            if (this->cpu.registers.flags.zf) {
                this->cpu.registers.PC = opcode.u16;
            } else timing -= 4;
            break;
        case 0xcb: // CB
            timing = parse_cb(this);
            break;
        case 0xcc: // CALL Z,a16
            if (this->cpu.registers.flags.zf) {
                push16(this, this->cpu.registers.PC);
                this->cpu.registers.PC = opcode.u16;
            } else timing -= 12;
            break;
        case 0xcd: // CALL a16
            push16(this, this->cpu.registers.PC);
            this->cpu.registers.PC = opcode.u16;
            break;
        case 0xce: // ADC A,d8
            this->cpu.registers.flags.h = (this->cpu.registers.A.n0 + this->cpu.registers.flags.cy + (opcode.u8 & 0x0f)) >> 4;
            temp_u16 = this->cpu.registers.A.byte + this->cpu.registers.flags.cy + opcode.u8;
            this->cpu.registers.A.byte = temp_u16;
            this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.cy = temp_u16 >> 8;
            break;
        case 0xcf: // RST 08H
            push16(this, this->cpu.registers.PC);
            this->cpu.registers.PC = 0x0008;
            break;
        case 0xd0: // RET NC
            if (!this->cpu.registers.flags.cy) {
                this->cpu.registers.PC = pop16(this);
            } else timing -= 12;
            break;
        case 0xd1: // POP DE
            this->cpu.registers.DE = pop16(this);
            break;
        case 0xd2: // JP NC,a16
            if (!this->cpu.registers.flags.cy) {
                this->cpu.registers.PC = opcode.u16;
            } else timing -= 4;
            break;
        case 0xd4: // CALL NC,a16
            if (!this->cpu.registers.flags.cy) {
                push16(this, this->cpu.registers.PC);
                this->cpu.registers.PC = opcode.u16;
            } else timing -= 12;
            break;
        case 0xd5: // PUSH DE
            push16(this, this->cpu.registers.DE);
            break;
        case 0xd6: // SUB d8
            this->cpu.registers.flags.h = (this->cpu.registers.A.n0 < (opcode.u8 & 0x0f)) ? 1 : 0;
            this->cpu.registers.flags.cy = this->cpu.registers.A.byte < opcode.u8;
            this->cpu.registers.A.byte -= opcode.u8;
            this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
            this->cpu.registers.flags.n = 1;
            break;
        case 0xd7: // RST 10H
            push16(this, this->cpu.registers.PC);
            this->cpu.registers.PC = 0x0010;
            break;
        case 0xd8: // RET C
            if (this->cpu.registers.flags.cy) {
                this->cpu.registers.PC = pop16(this);
            } else timing -= 12;
            break;
        case 0xd9: // RETI
            this->interrupts.IME = true;
            this->cpu.registers.PC = pop16(this);
            break;
        case 0xda: // JP C,a16
            if (this->cpu.registers.flags.cy) {
                this->cpu.registers.PC = opcode.u16;
            } else timing -= 4;
            break;
        case 0xdc: // CALL C,a16
            if (this->cpu.registers.flags.cy) {
                push16(this, this->cpu.registers.PC);
                this->cpu.registers.PC = opcode.u16;
            } else timing -= 12;
            break;
        case 0xde: // SBC A,d8
            this->cpu.registers.flags.h = (this->cpu.registers.A.n0 < ((opcode.u8 & 0x0f) + this->cpu.registers.flags.cy)) ? 1 : 0;
            temp_u16 = opcode.u8 + this->cpu.registers.flags.cy;
            this->cpu.registers.flags.cy = (this->cpu.registers.A.byte < temp_u16) ? 1 : 0;
            this->cpu.registers.A.byte -= temp_u16;
            this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
            this->cpu.registers.flags.n = 1;
            break;
        case 0xdf: // RST 18H
            push16(this, this->cpu.registers.PC);
            this->cpu.registers.PC = 0x0018;
            break;
        case 0xe0: // LDH (a8),A
            set_mem_u8(this, 0xff00 | opcode.u8, this->cpu.registers.A.byte);
            break;
        case 0xe1: // POP HL
            this->cpu.registers.HL = pop16(this);
            break;
        case 0xe2: // LD (C),A
            set_mem_u8(this, 0xff00 | this->cpu.registers.C.byte, this->cpu.registers.A.byte);
            break;
        case 0xe5: // PUSH HL
            push16(this, this->cpu.registers.HL);
            break;
        case 0xe6: // AND d8
            this->cpu.registers.A.byte &= opcode.u8;
            this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.h = 1;
            this->cpu.registers.flags.cy = 0;
            break;
        case 0xe7: // RST 20H
            push16(this, this->cpu.registers.PC);
            this->cpu.registers.PC = 0x0020;
            break;
        case 0xe8: // ADD SP,r8
            this->cpu.registers.flags.h = ((this->cpu.registers.SP & 0x0f) + (opcode.u8 & 0x0f)) >> 4;
            this->cpu.registers.flags.cy = ((this->cpu.registers.SP & 0xff) + opcode.u8) >> 8;
            this->cpu.registers.SP += opcode.i8;
            this->cpu.registers.flags.zf = 0;
            this->cpu.registers.flags.n = 0;
            break;
        case 0xe9: // JP (HL)
            this->cpu.registers.PC = this->cpu.registers.HL;
            break;
        case 0xea: // LD (a16),A
            set_mem_u8(this, opcode.u16, this->cpu.registers.A.byte);
            break;
        case 0xee: // XOR d8
            xor_func(this, opcode.u8);
            break;
        case 0xef: // RST 28H
            push16(this, this->cpu.registers.PC);
            this->cpu.registers.PC = 0x0028;
            break;
        case 0xf0: // LDH A,(a8)
            this->cpu.registers.A.byte = get_mem_u8(this, 0xff00 | opcode.u8);
            break;
        case 0xf1: // POP AF
            this->cpu.registers.AF = pop16(this) & 0xfff0;
            break;
        case 0xf2: // LD A,(C)
            this->cpu.registers.A.byte = get_mem_u8(this, 0xff00 | this->cpu.registers.C.byte);
            break;
        case 0xf3: // DI
            this->interrupts.IME = false;
            break;
        case 0xf5: // PUSH AF
            push16(this, this->cpu.registers.AF & 0xfff0);
            break;
        case 0xf6: // OR d8
            or_func(this, opcode.u8);
            break;
        case 0xf7: // RST 30H
            push16(this, this->cpu.registers.PC);
            this->cpu.registers.PC = 0x0030;
            break;
        case 0xf8: // LD HL,SP+r8
            this->cpu.registers.HL = this->cpu.registers.SP + opcode.i8;
            this->cpu.registers.flags.zf = 0;
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.h = ((this->cpu.registers.SP & 0x0f) + (opcode.u8 & 0x0f)) >> 4;
            this->cpu.registers.flags.cy = ((this->cpu.registers.SP & 0xff) + opcode.u8) >> 8;
            break;
        case 0xf9: // LD SP,HL
            this->cpu.registers.SP = this->cpu.registers.HL;
            break;
        case 0xfa: // LD A,(a16)
            this->cpu.registers.A.byte = get_mem_u8(this, opcode.u16);
            break;
        case 0xfb: // EI
            this->interrupts.enable_interrupts_step = 2;
            break;
        case 0xfe: // CP d8
            cp_func(this, opcode.u8);
            break;
        case 0xff: // RST 38H
            push16(this, this->cpu.registers.PC);
            this->cpu.registers.PC = 0x0038;
            break;
        default:
            fprintf(stderr, "Unknown opcode: 0x%02x at 0x%04x\n", opcode.code,
                    this->cpu.registers.PC - opcode.size);
            print_debug(this, EXIT_FAILURE);
    }

    return timing;
}

void push16(struct Context *this, uint16_t value) {
    this->cpu.registers.SP -= 2;
    set_mem_u16(this, this->cpu.registers.SP, value);
}

uint16_t pop16(struct Context *this) {
    uint16_t value = get_mem_u16(this, this->cpu.registers.SP);
    this->cpu.registers.SP += 2;
    return value;
}

void inc_func(struct Context *this, struct Byte *reg) {
    this->cpu.registers.flags.h = (reg->n0 + 1) >> 4;
    reg->byte++;
    this->cpu.registers.flags.zf = (reg->byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
}

void dec_func(struct Context *this, struct Byte *reg) {
    this->cpu.registers.flags.h = (reg->n0 == 0) ? 1 : 0;
    reg->byte--;
    this->cpu.registers.flags.zf = (reg->byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 1;
}

void add_a_func(struct Context *this, struct Byte *reg_src) {
    this->cpu.registers.flags.h = (this->cpu.registers.A.n0 + reg_src->n0) >> 4;
    uint16_t temp_u16 = this->cpu.registers.A.byte + reg_src->byte;
    this->cpu.registers.A.byte = temp_u16;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.cy = temp_u16 >> 8;
}

void adc_a_func(struct Context *this, struct Byte *reg_src) {
    this->cpu.registers.flags.h = (this->cpu.registers.A.n0 + reg_src->n0 + this->cpu.registers.flags.cy) >> 4;
    uint16_t temp_u16 = this->cpu.registers.A.byte + reg_src->byte + this->cpu.registers.flags.cy;
    this->cpu.registers.A.byte = temp_u16;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.cy = temp_u16 >> 8;
}

void sub_func(struct Context *this, struct Byte *reg_src) {
    this->cpu.registers.flags.h = this->cpu.registers.A.n0 < reg_src->n0;
    this->cpu.registers.flags.cy = this->cpu.registers.A.byte < reg_src->byte;
    this->cpu.registers.A.byte -= reg_src->byte;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 1;
}

void sbc_a_func(struct Context *this, struct Byte *reg_src) {
    this->cpu.registers.flags.h = (this->cpu.registers.A.n0 < (reg_src->n0 + this->cpu.registers.flags.cy)) ? 1 : 0;
    uint16_t temp_u16 = reg_src->byte + this->cpu.registers.flags.cy;
    this->cpu.registers.flags.cy = (this->cpu.registers.A.byte < temp_u16) ? 1 : 0;
    this->cpu.registers.A.byte -= temp_u16;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 1;
}

void and_func(struct Context *this, uint8_t value) {
    this->cpu.registers.A.byte &= value;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 1;
    this->cpu.registers.flags.cy = 0;
}

void xor_func(struct Context *this, uint8_t value) {
    this->cpu.registers.A.byte ^= value;
    this->cpu.registers.F = 0;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
}

void or_func(struct Context *this, uint8_t value) {
    this->cpu.registers.A.byte |= value;
    this->cpu.registers.F = 0;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
}

void cp_func(struct Context *this, uint8_t value) {
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == value) ? 1 : 0;
    this->cpu.registers.flags.n = 1;
    this->cpu.registers.flags.h = (this->cpu.registers.A.n0 < (value & 0x0f)) ? 1 : 0;
    this->cpu.registers.flags.cy = (this->cpu.registers.A.byte < value) ? 1 : 0;
}
