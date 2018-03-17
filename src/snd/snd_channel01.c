#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "definitions.h"
#include "snd_duty.h"
#include "snd_channel01.h"

void snd_channel01_tick_frame_seq(struct Context *this, int step) {
    Channel01 *channel = &this->sound.channel01;

    switch (step) {
        case 2:
        case 6:
            // sweep
            if (channel->freq_sweep.enabled && this->sound.regs.NR10.sweep_time) {
                if (channel->freq_sweep.period > 0) {
                    channel->freq_sweep.period--;
                }
                if (channel->freq_sweep.period == 0) {
                    uint16_t new_freq = snd_channel01_freq_sweep_new_value(this);
                    if (new_freq > 2047) {
                        channel->enabled = false;
                        new_freq = 2047;
                    }

                    if (this->sound.regs.NR10.sweep_shift) {
                        this->sound.regs.NR13_14.channel_freq = new_freq;
                        uint16_t freq = snd_channel01_freq_sweep_new_value(this);
                        if (freq > 2047) {
                            channel->enabled = false;
                        }
                    }
                    channel->freq_sweep.period = this->sound.regs.NR10.sweep_time;
                }
            }
        case 0:
        case 4:
            if (this->sound.regs.NR13_14.counter_consecutive_selection) {
                if (channel->length_counter > 0) {
                    channel->length_counter--;
                }

                if (channel->length_counter == 0) {
                    channel->enabled = false;
                }
            }
            break;
        case 7:
            if (channel->envelope.enabled) {
                if (--channel->envelope.period == 0) {
                    if (this->sound.regs.NR12.envelope_direction) {
                        if (channel->envelope.volume < 15) {
                            channel->envelope.volume++;
                        } else {
                            channel->envelope.volume = 15;
                            channel->envelope.enabled = false;
                        }
                    } else {
                        if (channel->envelope.volume > 0) {
                            channel->envelope.volume--;
                        } else {
                            channel->envelope.volume = 0;
                            channel->envelope.enabled = false;
                        }
                    }
                    channel->envelope.period = this->sound.regs.NR12.number_envelope_sweep;
                }
            }
            break;
    }
}

void snd_channel01_tick(struct Context *this) {
    Channel01 *channel = &this->sound.channel01;

    if (!channel->enabled) {
        channel->last_sample = 0;
        return;
    }

    uint64_t diff = this->cpu_timing - channel->last_update;
    channel->last_update = this->cpu_timing;
    channel->freq_timer += diff;

    const uint64_t freq_period = (2048 - this->sound.regs.NR13_14.channel_freq) * 4;
    if (channel->freq_timer >= freq_period) {
        channel->freq_timer -= freq_period;

        channel->duty_step++;
        if (channel->duty_step >= 8) {
            channel->duty_step = 0;
        }
    }

    float value = snd_duty_value(this->sound.regs.NR11.wave_pattern_duty, channel->duty_step);

    value *= channel->envelope.volume / 15.0;

    channel->last_sample = value;
}

uint16_t snd_channel01_freq_sweep_new_value(struct Context *this) {
    uint16_t freq = this->sound.regs.NR13_14.channel_freq;
    freq >>= this->sound.regs.NR10.sweep_shift;
    if (this->sound.regs.NR10.sweep_direction) {
        if (this->sound.regs.NR13_14.channel_freq > freq) {
            freq = this->sound.regs.NR13_14.channel_freq - freq;
        }
    } else {
        freq = this->sound.regs.NR13_14.channel_freq + freq;
    }
    return freq;
}
