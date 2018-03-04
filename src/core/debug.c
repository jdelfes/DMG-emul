#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#include "definitions.h"
#include "debug.h"

void print_debug(struct Context *this, int failure) {
    fprintf(stderr, "---------------\n");
    fprintf(stderr, "PC: %04x  SP: %04x\n", this->cpu.registers.PC, this->cpu.registers.SP);
    fprintf(stderr, "AF: %04x  BC: %04x\n", this->cpu.registers.AF, this->cpu.registers.BC);
    fprintf(stderr, "DE: %04x  HL: %04x\n", this->cpu.registers.DE, this->cpu.registers.HL);
    exit(failure);
}
