#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "definitions.h"
#include "sound.h"

bool sound_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value) {
    switch (address) {
        case 0xff25: // NR51 - Selection of Sound output terminal (R/W)
            *ret_value = 0xff;
            return true;
        case 0xff26: // NR52 - Sound on/off
            *ret_value = 0x70;
            return true;
    }
    return false;
}

bool sound_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    switch (address) {
        case 0xff10: // NR10 - Channel 1 Sweep register (R/W)
            return true;
        case 0xff11: // NR11 - Channel 1 Sound length/Wave pattern duty (R/W)
            return true;
        case 0xff12: // NR12 - Channel 1 Volume Envelope (R/W)
            return true;
        case 0xff13: // NR13 - Channel 1 Frequency lo (Write Only)
            return true;
        case 0xff14: // NR14 - Channel 1 Frequency hi (R/W)
            return true;
        case 0xff16: // NR21 - Channel 2 Sound Length/Wave Pattern Duty (R/W)
            return true;
        case 0xff17: // NR22 - Channel 2 Volume Envelope (R/W)
            return true;
        case 0xff18: // NR23 - Channel 2 Frequency lo data (W)
            return true;
        case 0xff19: // NR24 - Channel 2 Frequency hi data (R/W)
            return true;
        case 0xff1a: // NR30 - Channel 3 Sound on/off (R/W)
            return true;
        case 0xff1b: // NR31 - Channel 3 Sound Length
            return true;
        case 0xff1c: // NR32 - Channel 3 Select output level (R/W)
            return true;
        case 0xff1d: // NR33 - Channel 3 Frequency's lower data (W)
            return true;
        case 0xff1e: // NR34 - Channel 3 Frequency's higher data (R/W)
            return true;
        case 0xff20: // NR41 - Channel 4 Sound Length (R/W)
            return true;
        case 0xff21: // NR42 - Channel 4 Volume Envelope (R/W)
            return true;
        case 0xff22: // NR43 - Channel 4 Polynomial Counter (R/W)
            return true;
        case 0xff23: // NR44 - Channel 4 Counter/consecutive; Inital (R/W)
            return true;
        case 0xff24: // NR50 - Channel control / ON-OFF / Volume (R/W)
            return true;
        case 0xff25: // NR51 - Selection of Sound output terminal (R/W)
            return true;
        case 0xff26: // NR52 - Sound on/off
            return true;
    }

    if (address >= 0xff30 && address <= 0xff3f) {
        // Wave Pattern RAM
        return true;
    }

    return false;
}
