#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "definitions.h"
#include "debug.h"
#include "serial.h"

void serial_tick(struct Context *this) {
    if (!this->serial.SC.transfer_start) {
        return;
    }

    if (!this->serial.SC.shift_clock) {
        // external clock
        return;
    }

    uint64_t diff = this->cpu_timing - this->serial.last_transfer_started;
    uint16_t period = this->cpu.clock >>
        (this->serial.SC.clock_speed ?
         15 : // 262144Hz * 8 bits
         10); // 8192Hz * 8 bits
    if (diff >= period) {
        this->serial.SC.transfer_start = 0;
        this->interrupts.IF.serial = 1;
    }
}

bool serial_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value) {
    switch (address) {
        case 0xff01: // SB - Serial transfer data (R/W)
            *ret_value = 0xff;
            return true;
        case 0xff02: // SC - Serial Transfer Control (R/W)
            *ret_value = this->serial.SC.raw;
            return true;
    }
    return false;
}

bool serial_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    switch (address) {
        case 0xff01: // SB - Serial transfer data (R/W)
            this->serial.SB = value;
            d_printf("SB %02x\n", value);
            return true;
        case 0xff02: // SC - Serial Transfer Control (R/W)
            this->serial.SC.raw = value;
            d_printf("SC %02x\n", value);
            if (this->serial.SC.transfer_start) {
                this->serial.last_transfer_started = this->cpu_timing;
#ifdef ENABLE_SERIAL
                putchar(this->serial.SB);
                fflush(stdout);
#endif
            }
            return true;
    }

    return false;
}
