#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "definitions.h"
#include "snd_duty.h"
#include "snd_envelope.h"
#include "snd_channel04.h"

static uint8_t div_ratio_freq_value(uint8_t div_ratio_freq) {
//    return (div_ratio_freq + 1) * (div_ratio_freq + 1);
    switch (div_ratio_freq) {
        case 0: return 1;
        case 1: return 4;
        case 2: return 9;
        case 3: return 16;
        case 4: return 25;
        case 5: return 36;
        case 6: return 49;
        case 7: return 64;
        default: exit(EXIT_FAILURE);
    }
}

static void lfsr_roll(struct Context *this) {
    Channel04 *channel = &this->sound.channel04;
    uint8_t xor = channel->lfsr.bit0 ^ channel->lfsr.bit1;

    channel->lfsr.raw >>= 1;
    channel->lfsr.bit14 = xor;

    if (this->sound.NR43.counter_step_width) {
        channel->lfsr.bit6 = xor;
    }
}

static void mult_lfsr_roll(struct Context *this) {
//    float shift = (this->sound.NR43.shift_clock_freq > 0) ? this->sound.NR43.shift_clock_freq : 0.5;
//    const uint64_t freq_period = shift * 8 * div_ratio_freq_value(this->sound.NR43.shift_clock_freq);
    uint64_t freq_period;
    uint8_t shift = this->sound.NR43.shift_clock_freq;
    if (shift > 0) {
        freq_period = shift * 8 * div_ratio_freq_value(this->sound.NR43.shift_clock_freq);
    } else {
        freq_period = 4 * div_ratio_freq_value(this->sound.NR43.shift_clock_freq);
    }
    while (this->sound.channel04.freq_timer >= freq_period) {
        this->sound.channel04.freq_timer -= freq_period;

        lfsr_roll(this);
    }
}

static void frame(struct Context *this, uint64_t previous_frame_step) {
    Channel04 *channel = &this->sound.channel04;
    if (previous_frame_step != channel->frame.step) {
        switch (channel->frame.step) {
            case 2:
            case 6:
            case 0:
            case 4:
                if (this->sound.NR44.counter_consecutive_selection) {
                    if (channel->length_counter > 0) {
                        channel->length_counter--;
                    } else {
                        channel->length_counter = 0;
                        channel->enabled = false;
                    }
                }
                break;
            case 7:
                if (this->sound.NR42.number_envelope_sweep > 0) {
                    channel->envelope_step++;
                    if (channel->envelope_step == this->sound.NR42.number_envelope_sweep) {
                        channel->envelope_step = 0;

                        if (this->sound.NR42.envelope_direction) {
                            if (channel->envelope_volume < 15) {
                                channel->envelope_volume++;
                            }
                        } else {
                            if (channel->envelope_volume > 0) {
                                channel->envelope_volume--;
                            }
                        }
                    }
                }
                break;
            default:
                break;
        }
    }

}

void snd_channel04_tick(struct Context *this) {
    Channel04 *channel = &this->sound.channel04;
    if (!channel->enabled) {
        channel->last_sample = 0;
        return;
    }
    uint64_t diff = this->cpu_timing - channel->last_update;
    channel->last_update = this->cpu_timing;
    uint64_t previous_frame_step = channel->frame.step;
    channel->frame.timer += diff;
    channel->freq_timer += diff;

    mult_lfsr_roll(this);

    float value = (channel->lfsr.raw & 1) ? -1 : 1;

    frame(this, previous_frame_step);

    value *= snd_envelope_output_level_mult(channel->envelope_volume);

    channel->last_sample = value;
}
