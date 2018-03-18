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

static Byte inc_func(struct Context *this, Byte value);
static Byte dec_func(struct Context *this, Byte value);
static void add_a_func(struct Context *this, const Byte value);
static void adc_a_func(struct Context *this, const Byte value);
static void sub_func(struct Context *this, const Byte value);
static void sbc_a_func(struct Context *this, Byte value);
static void and_func(struct Context *this, const Byte value);
static void xor_func(struct Context *this, const Byte value);
static void or_func(struct Context *this, const Byte value);
static void cp_func(struct Context *this, const Byte value);

uint8_t cpu_execute(struct Context *this, const struct Opcode opcode) {
    uint16_t temp_u16;
    uint32_t temp_u32;
    Byte b_u8;
    uint8_t timing = OpcodesTiming[opcode.code];
    Instruction instr;
    instr.byte = opcode.code;

    switch (instr.x) {
        case 0:
            switch (instr.z) {
                case 4: // INC r8/(HL)
                case 5: // DEC r8/(HL)
                    switch (instr.y) {
                        case 0: b_u8 = this->cpu.registers.B; break;
                        case 1: b_u8 = this->cpu.registers.C; break;
                        case 2: b_u8 = this->cpu.registers.D; break;
                        case 3: b_u8 = this->cpu.registers.E; break;
                        case 4: b_u8 = this->cpu.registers.H; break;
                        case 5: b_u8 = this->cpu.registers.L; break;
                        case 6: b_u8.byte = get_mem_u8(this, this->cpu.registers.HL); break;
                        case 7: b_u8 = this->cpu.registers.A; break;
                        default: exit(EXIT_FAILURE);
                    }
                case 6: // LD r8/(HL),u8
                    switch (instr.z) {
                        case 4: b_u8 = inc_func(this, b_u8); break;
                        case 5: b_u8 = dec_func(this, b_u8); break;
                        case 6: b_u8.byte = opcode.u8; break;
                    }
                    switch (instr.y) {
                        case 0: this->cpu.registers.B = b_u8; break;
                        case 1: this->cpu.registers.C = b_u8; break;
                        case 2: this->cpu.registers.D = b_u8; break;
                        case 3: this->cpu.registers.E = b_u8; break;
                        case 4: this->cpu.registers.H = b_u8; break;
                        case 5: this->cpu.registers.L = b_u8; break;
                        case 6: set_mem_u8(this, this->cpu.registers.HL, b_u8.byte); break;
                        case 7: this->cpu.registers.A = b_u8; break;
                    }
                    break;
            }
            break;
        case 1: // LD r8/(HL), r8/(HL)
            switch (instr.z) {
                case 0: b_u8 = this->cpu.registers.B; break;
                case 1: b_u8 = this->cpu.registers.C; break;
                case 2: b_u8 = this->cpu.registers.D; break;
                case 3: b_u8 = this->cpu.registers.E; break;
                case 4: b_u8 = this->cpu.registers.H; break;
                case 5: b_u8 = this->cpu.registers.L; break;
                case 6: b_u8.byte = get_mem_u8(this, this->cpu.registers.HL); break;
                case 7: b_u8 = this->cpu.registers.A; break;
                default: exit(EXIT_FAILURE);
            }
            switch (instr.y) {
                case 0: this->cpu.registers.B = b_u8; break;
                case 1: this->cpu.registers.C = b_u8; break;
                case 2: this->cpu.registers.D = b_u8; break;
                case 3: this->cpu.registers.E = b_u8; break;
                case 4: this->cpu.registers.H = b_u8; break;
                case 5: this->cpu.registers.L = b_u8; break;
                case 6: set_mem_u8(this, this->cpu.registers.HL, b_u8.byte); break;
                case 7: this->cpu.registers.A = b_u8; break;
            }
            break;
        case 2: // ADD/ADC/SUB/SBC/AND/XOR/OR/CP A, r8/(HL)
            switch (instr.z) {
                case 0: b_u8 = this->cpu.registers.B; break;
                case 1: b_u8 = this->cpu.registers.C; break;
                case 2: b_u8 = this->cpu.registers.D; break;
                case 3: b_u8 = this->cpu.registers.E; break;
                case 4: b_u8 = this->cpu.registers.H; break;
                case 5: b_u8 = this->cpu.registers.L; break;
                case 6: b_u8.byte = get_mem_u8(this, this->cpu.registers.HL); break;
                case 7: b_u8 = this->cpu.registers.A; break;
                default: exit(EXIT_FAILURE);
            }
            switch (instr.y) {
                case 0: add_a_func(this, b_u8); break;
                case 1: adc_a_func(this, b_u8); break;
                case 2: sub_func(this, b_u8); break;
                case 3: sbc_a_func(this, b_u8); break;
                case 4: and_func(this, b_u8); break;
                case 5: xor_func(this, b_u8); break;
                case 6: or_func(this, b_u8); break;
                case 7: cp_func(this, b_u8); break;
            }
            break;
        case 3:
            switch (instr.z) {
                case 6: // ADD/ADC/SUB/SBC/AND/XOR/OR/CP A, u8
                    b_u8.byte = opcode.u8;
                    switch (instr.y) {
                        case 0: add_a_func(this, b_u8); break;
                        case 1: adc_a_func(this, b_u8); break;
                        case 2: sub_func(this, b_u8); break;
                        case 3: sbc_a_func(this, b_u8); break;
                        case 4: and_func(this, b_u8); break;
                        case 5: xor_func(this, b_u8); break;
                        case 6: or_func(this, b_u8); break;
                        case 7: cp_func(this, b_u8); break;
                    }
                    break;
                case 7: // RST ??H
                    push16(this, this->cpu.registers.PC);
                    switch (instr.y) {
                        case 0: this->cpu.registers.PC = 0x0000; break;
                        case 1: this->cpu.registers.PC = 0x0008; break;
                        case 2: this->cpu.registers.PC = 0x0010; break;
                        case 3: this->cpu.registers.PC = 0x0018; break;
                        case 4: this->cpu.registers.PC = 0x0020; break;
                        case 5: this->cpu.registers.PC = 0x0028; break;
                        case 6: this->cpu.registers.PC = 0x0030; break;
                        case 7: this->cpu.registers.PC = 0x0038; break;
                    }
                    break;
            }
            break;
    }

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
        case 0x3f: // CCF
            this->cpu.registers.flags.n = 0;
            this->cpu.registers.flags.h = 0;
            this->cpu.registers.flags.cy ^= 1; // CHECK
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

static Byte inc_func(struct Context *this, Byte value) {
    this->cpu.registers.flags.h = (value.n0 + 1) >> 4;
    value.byte++;
    this->cpu.registers.flags.zf = (value.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    return value;
}

static Byte dec_func(struct Context *this, Byte value) {
    this->cpu.registers.flags.h = (value.n0 == 0) ? 1 : 0;
    value.byte--;
    this->cpu.registers.flags.zf = (value.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 1;
    return value;
}

static void add_a_func(struct Context *this, const Byte value) {
    this->cpu.registers.flags.h = (this->cpu.registers.A.n0 + value.n0) >> 4;
    uint16_t temp_u16 = this->cpu.registers.A.byte + value.byte;
    this->cpu.registers.A.byte = temp_u16;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.cy = temp_u16 >> 8;
}

static void adc_a_func(struct Context *this, const Byte value) {
    this->cpu.registers.flags.h = (this->cpu.registers.A.n0 + value.n0 + this->cpu.registers.flags.cy) >> 4;
    uint16_t temp_u16 = this->cpu.registers.A.byte + value.byte + this->cpu.registers.flags.cy;
    this->cpu.registers.A.byte = temp_u16;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.cy = temp_u16 >> 8;
}

static void sub_func(struct Context *this, const Byte value) {
    this->cpu.registers.flags.h = this->cpu.registers.A.n0 < value.n0;
    this->cpu.registers.flags.cy = this->cpu.registers.A.byte < value.byte;
    this->cpu.registers.A.byte -= value.byte;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 1;
}

static void sbc_a_func(struct Context *this, const Byte value) {
    this->cpu.registers.flags.h = (this->cpu.registers.A.n0 < (value.n0 + this->cpu.registers.flags.cy)) ? 1 : 0;
    uint16_t temp_u16 = value.byte + this->cpu.registers.flags.cy;
    this->cpu.registers.flags.cy = (this->cpu.registers.A.byte < temp_u16) ? 1 : 0;
    this->cpu.registers.A.byte -= temp_u16;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 1;
}

static void and_func(struct Context *this, const Byte value) {
    this->cpu.registers.A.byte &= value.byte;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
    this->cpu.registers.flags.n = 0;
    this->cpu.registers.flags.h = 1;
    this->cpu.registers.flags.cy = 0;
}

static void xor_func(struct Context *this, const Byte value) {
    this->cpu.registers.A.byte ^= value.byte;
    this->cpu.registers.F = 0;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
}

static void or_func(struct Context *this, const Byte value) {
    this->cpu.registers.A.byte |= value.byte;
    this->cpu.registers.F = 0;
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == 0) ? 1 : 0;
}

static void cp_func(struct Context *this, const Byte value) {
    this->cpu.registers.flags.zf = (this->cpu.registers.A.byte == value.byte) ? 1 : 0;
    this->cpu.registers.flags.n = 1;
    this->cpu.registers.flags.h = (this->cpu.registers.A.n0 < value.n0) ? 1 : 0;
    this->cpu.registers.flags.cy = (this->cpu.registers.A.byte < value.byte) ? 1 : 0;
}
