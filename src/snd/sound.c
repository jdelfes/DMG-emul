#include <stdio.h>
#include <stdbool.h>
#include <strings.h>
#include <stdlib.h>

#include "definitions.h"
#include "sound.h"
#include "debug.h"

int sound_init(struct Context *this) {
    snd_card_init();
    return 0;
}

void sound_tick(struct Context *this) {
    snd_channel01_tick(this);
    snd_channel02_tick(this);
    snd_channel03_tick(this);

    snd_mixer_tick(this);

    snd_card_tick(this);
}

bool sound_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value) {
//    switch (address) {
//        case 0xff25: // NR51 - Selection of Sound output terminal (R/W)
//            *ret_value = 0xff;
//            return true;
//        case 0xff26: // NR52 - Sound on/off
//            *ret_value = this->sound.NR52.raw;
//            return true;
//    }
    return false;
}

bool sound_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    switch (address) {
        case 0xff10: // NR10 - Channel 1 Sweep register (R/W)
            this->sound.NR10.raw = value;
            return true;
        case 0xff11: // NR11 - Channel 1 Sound length/Wave pattern duty (R/W)
            this->sound.NR11.raw = value;
            this->sound.channel01.length_counter = 64 - this->sound.NR11.sound_length_data;
            return true;
        case 0xff12: // NR12 - Channel 1 Volume Envelope (R/W)
            this->sound.NR12.raw = value;
            this->sound.channel01.envelope_volume = this->sound.NR12.initial_envelope_volume;
            this->sound.channel01.envelope_step = 0;
            return true;
        case 0xff13: // NR13 - Channel 1 Frequency lo (Write Only)
            this->sound.NR13_14.NRx3_raw = value;
            return true;
        case 0xff14: // NR14 - Channel 1 Frequency hi (R/W)
            this->sound.NR13_14.NRx4_raw = value;
            if (value & 0x80) {
                memset(&this->sound.channel01, 0, sizeof(this->sound.channel01));
                this->sound.channel01.enabled = true;
                this->sound.channel01.last_update = this->cpu_timing;
                this->sound.channel01.length_counter = 64 - this->sound.NR11.sound_length_data;
                this->sound.channel01.envelope_volume = this->sound.NR12.initial_envelope_volume;
            }
            return true;
        case 0xff16: // NR21 - Channel 2 Sound Length/Wave Pattern Duty (R/W)
            this->sound.NR21.raw = value;
            this->sound.channel02.length_counter = 64 - this->sound.NR21.sound_length_data;
            return true;
        case 0xff17: // NR22 - Channel 2 Volume Envelope (R/W)
            this->sound.NR22.raw = value;
            this->sound.channel02.envelope_volume = this->sound.NR22.initial_envelope_volume;
            this->sound.channel02.envelope_step = 0;
            return true;
        case 0xff18: // NR23 - Channel 2 Frequency lo data (W)
            this->sound.NR23_24.NRx3_raw = value;
            return true;
        case 0xff19: // NR24 - Channel 2 Frequency hi data (R/W)
            this->sound.NR23_24.NRx4_raw = value;
            if (value & 0x80) {
                memset(&this->sound.channel02, 0, sizeof(this->sound.channel02));
                this->sound.channel02.enabled = true;
                this->sound.channel02.last_update = this->cpu_timing;
                this->sound.channel02.length_counter = 64 - this->sound.NR21.sound_length_data;
                this->sound.channel02.envelope_volume = this->sound.NR22.initial_envelope_volume;
            }
            return true;
        case 0xff1a: // NR30 - Channel 3 Sound on/off (R/W)
            if (value & 0x80) {
                this->sound.channel03.enabled = true;
            } else {
                this->sound.channel03.enabled = false;
            }
            return true;
        case 0xff1b: // NR31 - Channel 3 Sound Length
            this->sound.NR31 = value;
            return true;
        case 0xff1c: // NR32 - Channel 3 Select output level (R/W)
            this->sound.NR32.raw = value;
            return true;
        case 0xff1d: // NR33 - Channel 3 Frequency's lower data (W)
            this->sound.NR33_34.NRx3_raw = value;
            return true;
        case 0xff1e: // NR34 - Channel 3 Frequency's higher data (R/W)
            this->sound.NR33_34.NRx4_raw = value;
            if (value & 0x80) {
                memset(&this->sound.channel03, 0, sizeof(this->sound.channel03));
                this->sound.channel03.enabled = true;
                this->sound.channel03.last_update = this->cpu_timing;
                this->sound.channel03.length_counter = 256 - this->sound.NR31;
            }
            return true;
//        case 0xff20: // NR41 - Channel 4 Sound Length (R/W)
//            return true;
//        case 0xff21: // NR42 - Channel 4 Volume Envelope (R/W)
//            return true;
//        case 0xff22: // NR43 - Channel 4 Polynomial Counter (R/W)
//            return true;
//        case 0xff23: // NR44 - Channel 4 Counter/consecutive; Inital (R/W)
//            return true;
        case 0xff24: // NR50 - Channel control / ON-OFF / Volume (R/W)
            this->sound.NR50.raw = value;
            return true;
        case 0xff25: // NR51 - Selection of Sound output terminal (R/W)
            this->sound.NR51.raw = value;
            return true;
        case 0xff26: // NR52 - Sound on/off
            if (value & 0x80) {
                memset(&this->sound.mixer, 0, sizeof(this->sound.mixer));
                memset(&this->sound.sound_card, 0, sizeof(this->sound.sound_card));
            } else {
                this->sound.channel01.enabled = false;
                this->sound.channel02.enabled = false;
                this->sound.channel03.enabled = false;
            }
            return true;
    }

    if (address >= 0xff30 && address <= 0xff3f) {
        // Wave Pattern RAM
        this->sound.wave_pattern_ram[address - 0xff30].raw = value;
        return true;
    }

    return false;
}
