#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "definitions.h"
#include "debug.h"
#include "timer.h"

bool timer_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value) {
    switch (address) {
        case 0xff04: // DIV - Divider Register (R/W)
            *ret_value = this->timer.DIV.value;
            return true;
        case 0xff05: // TIMA - Timer counter (R/W)
            *ret_value = this->timer.TIMA;
            return true;
        case 0xff07: // TAC - Timer Control (R/W)
            *ret_value = this->timer.TAC.raw | (~0x03);
            return true;
    }
    return false;
}

bool timer_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    bool previous;
    switch (address) {
        case 0xff04: // DIV - Divider Register (R/W)
            d_printf("DIV %02x\n", value);
            this->timer.DIV.internal = 0;
            this->timer.DIV.last_update = this->cpu_timing;
            return true;
        case 0xff05: // TIMA - Timer counter (R/W)
            d_printf("TIMA %02x\n", value);
            this->timer.TIMA = value;
            return true;
        case 0xff06: // TMA - Timer Modulo (R/W)
            printf("TMA %02x\n", value);
            this->timer.TMA = value;
            return true;
        case 0xff07: // TAC - Timer Control (R/W)
            d_printf("TAC %02x\n", value);
            previous = this->timer.TAC.enable;
            this->timer.TAC.raw = value & 0x07;
            if ((previous ^ 1) & this->timer.TAC.enable) { // starting timer
                this->timer.TIMA = this->timer.TMA;
            }
            return true;
    }

    return false;
}

void timer_check(struct Context *this) {
    uint16_t previous = this->timer.DIV.internal;
    uint64_t diff = this->cpu_timing - this->timer.DIV.last_update;
    if (diff > 4) {
        fprintf(stderr, "timer_check too late, step %llu\n", diff);
        exit(EXIT_FAILURE);
    }
    this->timer.DIV.internal += diff;
    this->timer.DIV.last_update = this->cpu_timing;

    if (this->timer.TAC.enable) {
        uint8_t bit;
        switch (this->timer.TAC.clock_select) {
            case 0: bit = 10; break;
            case 1: bit = 3; break;
            case 2: bit = 5; break;
            case 3: bit = 7; break;
            default: exit(EXIT_FAILURE);
        }

        if (((~this->timer.DIV.internal) & previous) & (1 << bit)) {
            this->timer.TIMA++;
            if (this->timer.TIMA == 0) {
                this->timer.TIMA = this->timer.TMA;
                this->interrupts.IF.timer = 1;
            }
        }
    }
}
