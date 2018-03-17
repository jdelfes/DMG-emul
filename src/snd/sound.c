#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "definitions.h"
#include "sound.h"
#include "debug.h"

int sound_init(struct Context *this) {
    snd_card_init();
    return 0;
}

void sound_tick(struct Context *this) {
    if (this->sound.sound_enabled) {
        snd_frame_seq_tick(this);

        snd_channel01_tick(this);
        snd_channel02_tick(this);
        snd_channel03_tick(this);
        snd_channel04_tick(this);

        snd_mixer_tick(this);

        snd_card_tick(this);
    }
}

bool sound_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value) {
    switch (address) {
        case 0xff10: // NR10 - Channel 1 Sweep register (R/W)
            *ret_value = this->sound.regs.NR10.raw | 0x80;
            return true;
        case 0xff11: // NR11 - Channel 1 Sound length/Wave pattern duty (R/W)
            *ret_value = this->sound.regs.NR11.raw | 0x3f;
            return true;
        case 0xff12: // NR12 - Channel 1 Volume Envelope (R/W)
            *ret_value = this->sound.regs.NR12.raw;
            return true;
        case 0xff13: // NR13 - Channel 1 Frequency lo (Write Only)
            *ret_value = 0xff;
            return true;
        case 0xff14: // NR14 - Channel 1 Frequency hi (R/W)
            *ret_value = this->sound.regs.NR13_14.NRx4_raw | 0xbf;
            return true;
        case 0xff16: // NR21 - Channel 2 Sound Length/Wave Pattern Duty (R/W)
            *ret_value = this->sound.regs.NR21.raw | 0x3f;
            return true;
        case 0xff17: // NR22 - Channel 2 Volume Envelope (R/W)
            *ret_value = this->sound.regs.NR22.raw;
            return true;
        case 0xff19: // NR24 - Channel 2 Frequency hi data (R/W)
            *ret_value = this->sound.regs.NR23_24.NRx4_raw | 0xbf;
            return true;
        case 0xff1a: // NR30 - Channel 3 Sound on/off (R/W)
            *ret_value = this->sound.regs.NR30 | 0x7f;
            return true;
        case 0xff1c: // NR32 - Channel 3 Select output level (R/W)
            *ret_value = this->sound.regs.NR32.raw | 0x9f;
            return true;
        case 0xff1e: // NR34 - Channel 3 Frequency's higher data (R/W)
            *ret_value = this->sound.regs.NR33_34.NRx4_raw | 0xbf;
            return true;
        case 0xff21: // NR42 - Channel 4 Volume Envelope (R/W)
            *ret_value = this->sound.regs.NR42.raw;
            return true;
        case 0xff22: // NR43 - Channel 4 Polynomial Counter (R/W)
            *ret_value = this->sound.regs.NR43.raw;
            return true;
        case 0xff23: // NR44 - Channel 4 Counter/consecutive; Inital (R/W)
            *ret_value = this->sound.regs.NR44.raw | 0xbf;
            return true;
        case 0xff24: // NR50 - Channel control / ON-OFF / Volume (R/W)
            *ret_value = this->sound.regs.NR50.raw;
            return true;
        case 0xff25: // NR51 - Selection of Sound output terminal (R/W)
            *ret_value = this->sound.regs.NR51.raw;
            return true;
        case 0xff26: // NR52 - Sound on/off
            *ret_value = (this->sound.sound_enabled ? 0x80 : 0x00)
                | (this->sound.channel04.enabled ? 0x08 : 0x00)
                | (this->sound.channel03.enabled ? 0x04 : 0x00)
                | (this->sound.channel02.enabled ? 0x02 : 0x00)
                | (this->sound.channel01.enabled ? 0x01 : 0x00)
                | 0x70;
            return true;
    }

    if (address >= 0xff30 && address <= 0xff3f) {
        // Wave Pattern RAM
        *ret_value = this->sound.regs.wave_pattern_ram[address - 0xff30].raw;
        return true;
    }

    if (address >= 0xff10 && address <= 0xff3f) {
        *ret_value = 0xff;
        return true;
    }

    return false;
}

bool sound_handle_set_u8(struct Context *this, uint16_t address, uint8_t value) {
    if (this->sound.sound_enabled) {
        switch (address) {
            case 0xff10: // NR10 - Channel 1 Sweep register (R/W)
                this->sound.regs.NR10.raw = value;
                this->sound.channel01.freq_sweep.period = this->sound.regs.NR10.sweep_time;
                return true;
            case 0xff11: // NR11 - Channel 1 Sound length/Wave pattern duty (R/W)
                this->sound.regs.NR11.raw = value;
                this->sound.channel01.length_counter = 64 - this->sound.regs.NR11.sound_length_data;
                return true;
            case 0xff12: // NR12 - Channel 1 Volume Envelope (R/W)
                this->sound.regs.NR12.raw = value;
                if (this->sound.regs.NR12.initial_envelope_volume == 0) {
                    this->sound.channel01.enabled = false;
                }
                this->sound.channel01.envelope.volume = this->sound.regs.NR12.initial_envelope_volume;
                this->sound.channel01.envelope.period = this->sound.regs.NR12.number_envelope_sweep;
                this->sound.channel01.envelope.enabled = this->sound.regs.NR12.number_envelope_sweep > 0;
                return true;
            case 0xff13: // NR13 - Channel 1 Frequency lo (Write Only)
                this->sound.regs.NR13_14.NRx3_raw = value;
                return true;
            case 0xff14: // NR14 - Channel 1 Frequency hi (R/W)
                this->sound.regs.NR13_14.NRx4_raw = value;
                if (value & 0x80) {
                    if (this->sound.channel01.length_counter == 0) {
                        this->sound.channel01.length_counter = 64;
                    }
                    if (this->sound.regs.NR12.raw & 0xf8) {
                        this->sound.channel01.enabled = true;
                    } else {
                        this->sound.channel01.enabled = false;
                    }
                    this->sound.channel01.last_update = this->cpu_timing;
                    this->sound.channel01.envelope.volume = this->sound.regs.NR12.initial_envelope_volume;
                    this->sound.channel01.envelope.period = this->sound.regs.NR12.number_envelope_sweep;
                    this->sound.channel01.envelope.enabled = this->sound.regs.NR12.number_envelope_sweep > 0;
                    this->sound.channel01.freq_sweep.enabled = (this->sound.regs.NR10.sweep_shift > 0)
                        || (this->sound.regs.NR10.sweep_time > 0);
                    this->sound.channel01.freq_sweep.period = this->sound.regs.NR10.sweep_time;
                    if (this->sound.regs.NR10.sweep_shift > 0) {
                        uint16_t freq = snd_channel01_freq_sweep_new_value(this);
                        if (freq > 2047) {
                            this->sound.channel01.enabled = false;
                            freq = 2047;
                        }
                        this->sound.regs.NR13_14.channel_freq = freq;
                    }
                }
                return true;
            case 0xff16: // NR21 - Channel 2 Sound Length/Wave Pattern Duty (R/W)
                this->sound.regs.NR21.raw = value;
                this->sound.channel02.length_counter = 64 - this->sound.regs.NR21.sound_length_data;
                return true;
            case 0xff17: // NR22 - Channel 2 Volume Envelope (R/W)
                this->sound.regs.NR22.raw = value;
                if (this->sound.regs.NR22.initial_envelope_volume == 0) {
                    this->sound.channel02.enabled = false;
                }
                this->sound.channel02.envelope.volume = this->sound.regs.NR22.initial_envelope_volume;
                this->sound.channel02.envelope.period = this->sound.regs.NR22.number_envelope_sweep;
                this->sound.channel02.envelope.enabled = this->sound.regs.NR22.number_envelope_sweep > 0;
                return true;
            case 0xff18: // NR23 - Channel 2 Frequency lo data (W)
                this->sound.regs.NR23_24.NRx3_raw = value;
                return true;
            case 0xff19: // NR24 - Channel 2 Frequency hi data (R/W)
                this->sound.regs.NR23_24.NRx4_raw = value;
                if (value & 0x80) {
                    if (this->sound.channel02.length_counter == 0) {
                        this->sound.channel02.length_counter = 64;
                    }
                    if (this->sound.regs.NR22.raw & 0xf8) {
                        this->sound.channel02.enabled = true;
                    } else {
                        this->sound.channel02.enabled = false;
                    }
                    this->sound.channel02.last_update = this->cpu_timing;
                    this->sound.channel02.envelope.volume = this->sound.regs.NR22.initial_envelope_volume;
                    this->sound.channel02.envelope.period = this->sound.regs.NR22.number_envelope_sweep;
                    this->sound.channel02.envelope.enabled = this->sound.regs.NR22.number_envelope_sweep > 0;
                }
                return true;
            case 0xff1a: // NR30 - Channel 3 Sound on/off (R/W)
                this->sound.regs.NR30 = value;
                if ((value & 0x80) == 0) {
                    this->sound.channel03.enabled = false;
                }
                return true;
            case 0xff1b: // NR31 - Channel 3 Sound Length
                this->sound.regs.NR31 = value;
                this->sound.channel03.length_counter = 256 - this->sound.regs.NR31;
                return true;
            case 0xff1c: // NR32 - Channel 3 Select output level (R/W)
                this->sound.regs.NR32.raw = value;
                return true;
            case 0xff1d: // NR33 - Channel 3 Frequency's lower data (W)
                this->sound.regs.NR33_34.NRx3_raw = value;
                return true;
            case 0xff1e: // NR34 - Channel 3 Frequency's higher data (R/W)
                this->sound.regs.NR33_34.NRx4_raw = value;
                if (value & 0x80) {
                    if (this->sound.channel03.length_counter == 0) {
                        this->sound.channel03.length_counter = 256;
                    }
                    if (this->sound.regs.NR30 & 0x80) {
                        this->sound.channel03.enabled = true;
                    } else {
                        this->sound.channel03.enabled = false;
                    }
                    this->sound.channel03.last_update = this->cpu_timing;
                }
                return true;
            case 0xff20: // NR41 - Channel 4 Sound Length (R/W)
                this->sound.regs.NR41.raw = value;
                this->sound.channel04.length_counter = 64 - this->sound.regs.NR41.sound_length_data;
                return true;
            case 0xff21: // NR42 - Channel 4 Volume Envelope (R/W)
                this->sound.regs.NR42.raw = value;
                if (this->sound.regs.NR42.initial_envelope_volume == 0) {
                    this->sound.channel04.enabled = false;
                }
                this->sound.channel04.envelope.volume = this->sound.regs.NR42.initial_envelope_volume;
                this->sound.channel04.envelope.period = this->sound.regs.NR42.number_envelope_sweep;
                this->sound.channel04.envelope.enabled = this->sound.regs.NR42.number_envelope_sweep > 0;
                return true;
            case 0xff22: // NR43 - Channel 4 Polynomial Counter (R/W)
                this->sound.regs.NR43.raw = value;
                return true;
            case 0xff23: // NR44 - Channel 4 Counter/consecutive; Inital (R/W)
                this->sound.regs.NR44.raw = value;
                if (value & 0x80) {
                    if (this->sound.channel04.length_counter == 0) {
                        this->sound.channel04.length_counter = 64;
                    }
                    if (this->sound.regs.NR42.raw & 0xf8) {
                        this->sound.channel04.enabled = true;
                    } else {
                        this->sound.channel04.enabled = false;
                    }
                    this->sound.channel04.last_update = this->cpu_timing;
                    this->sound.channel04.envelope.volume = this->sound.regs.NR42.initial_envelope_volume;
                    this->sound.channel04.envelope.period = this->sound.regs.NR42.number_envelope_sweep;
                    this->sound.channel04.envelope.enabled = this->sound.regs.NR42.number_envelope_sweep > 0;
                    this->sound.channel04.lfsr.raw = ~0;
                }
                return true;
            case 0xff24: // NR50 - Channel control / ON-OFF / Volume (R/W)
                this->sound.regs.NR50.raw = value;
                return true;
            case 0xff25: // NR51 - Selection of Sound output terminal (R/W)
                this->sound.regs.NR51.raw = value;
                return true;
        }
    }

    if (address == 0xff26) { // NR52 - Sound on/off
        if (value & 0x80) {
            this->sound.sound_enabled = true;
            memset(&this->sound.mixer, 0, sizeof(this->sound.mixer));
            memset(&this->sound.frame_seq, 0, sizeof(this->sound.frame_seq));
            this->sound.frame_seq.last_update = this->timer.DIV.value;
            this->sound.frame_seq.last_step = -1;
        } else {
            this->sound.sound_enabled = false;
            this->sound.channel01.enabled = false;
            this->sound.channel02.enabled = false;
            this->sound.channel03.enabled = false;
            this->sound.channel04.enabled = false;
            WavePattern pat[16];
            memcpy(&pat, &this->sound.regs.wave_pattern_ram, sizeof(pat));
            memset(&this->sound.regs, 0, sizeof(this->sound.regs));
            memcpy(&this->sound.regs.wave_pattern_ram, &pat, sizeof(pat));
        }
        return true;
    }

    if (address >= 0xff30 && address <= 0xff3f) {
        // Wave Pattern RAM
        this->sound.regs.wave_pattern_ram[address - 0xff30].raw = value;
        return true;
    }

    if (address >= 0xff10 && address <= 0xff3f) {
        return true;
    }

    return false;
}
