#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "definitions.h"
#include "debug.h"
#include "timer.h"
#include "memory.h"
#include "opcodes.h"

uint8_t OpcodeSize[] = {
    1, 3, 1, 1, 1, 1, 2, 1, 3, 1, 1, 1, 1, 1, 2, 1, // 0x
    1, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, // 1x
    2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, // 2x
    2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, // 3x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9x
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // Ax
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // Bx
    1, 1, 3, 3, 3, 1, 2, 1, 1, 1, 3, 1, 3, 3, 2, 1, // Cx
    1, 1, 3, 0, 3, 1, 2, 1, 1, 1, 3, 0, 3, 0, 2, 1, // Dx
    2, 1, 1, 0, 0, 1, 2, 1, 2, 1, 3, 0, 0, 0, 2, 1, // Ex
    2, 1, 1, 1, 0, 1, 2, 1, 2, 1, 3, 1, 0, 0, 2, 1, // Fx
};

uint8_t OpcodesTiming[] = {
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

struct Opcode next_opcode(struct Context *this) {
    uint8_t temp_u8;
    struct Opcode opcode;

    opcode.code = get_mem_u8(this, this->cpu.registers.PC);
    opcode.size = OpcodeSize[opcode.code];
    switch (opcode.size) {
        case 0:
            fprintf(stderr, "Invalid opcode %02x\n", opcode.code);
            print_debug(this, EXIT_FAILURE);
        case 1:
            break;
        case 2:
            opcode.u8 = get_mem_u8(this, this->cpu.registers.PC + 1);
            break;
        case 3:
            temp_u8 = get_mem_u8(this, this->cpu.registers.PC + 1);
            opcode.u16 = (get_mem_u8(this, this->cpu.registers.PC + 2) << 8) | temp_u8;
            break;
        default:
            fprintf(stderr, "Weird opcode size %i\n", opcode.size);
            print_debug(this, EXIT_FAILURE);
    }

    this->cpu.registers.PC += opcode.size;
    return opcode;
}

