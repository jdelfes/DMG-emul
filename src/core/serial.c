#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "definitions.h"
#include "debug.h"
#include "serial.h"

bool serial_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value) {
    switch (address) {
        case 0xff02:
            *ret_value = 0xff;
            return true;
    }
    return false;
}

bool serial_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    switch (address) {
        case 0xff01: // SB - Serial transfer data (R/W)
            d_printf("SB %02x\n", value);
            this->serial.SB = value;
            return true;
        case 0xff02: // SC - Serial Transfer Control (R/W)
            d_printf("SC %02x\n", value);
            if (value >> 7) {
                putchar(this->serial.SB);
                fflush(stdout);
            }
            return true;
    }

    return false;
}
