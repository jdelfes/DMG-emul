#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "definitions.h"
#include "snd_channel03.h"

void snd_channel03_tick(struct Context *this) {
    Channel03 *channel = &this->sound.channel03;
    if (!channel->enabled) {
        channel->last_sample = 0;
        return;
    }
    uint64_t diff = this->cpu_timing - channel->last_update;
    channel->last_update = this->cpu_timing;
    uint64_t previous_frame_step = channel->frame.step;
    channel->frame.timer += diff;
    channel->freq_timer += diff;

    const uint64_t freq_period = (2048 - this->sound.NR33_34.channel_freq) * 2;
    while (channel->freq_timer >= freq_period) {
        channel->freq_timer -= freq_period;

        channel->wave_pat_step++;
        if (channel->wave_pat_step >= 32) {
            channel->wave_pat_step = 0;
        }
    }

    WavePattern pat = this->sound.wave_pattern_ram[channel->wave_pat_step >> 1];

    float value;
    if (channel->wave_pat_step & 1) {
        value = pat.nibble0;
    } else {
        value = pat.nibble1;
    }
    value = (value - 7.0) / 8.0;

    if (previous_frame_step != channel->frame.step) {
        switch (channel->frame.step) {
            case 2:
            case 6:
            case 0:
            case 4:
                if (this->sound.NR33_34.counter_consecutive_selection) {
                    if (channel->length_counter > 0) {
                        channel->length_counter--;
                    } else {
                        channel->length_counter = 0;
                        channel->enabled = false;
                    }
                }
                break;
            case 7:
            default:
                break;
        }
    }

    float vol_mult;
    switch (this->sound.NR32.output_level) {
        case 0: vol_mult = 0.00; break; // mute
        case 1: vol_mult = 1.00; break; // 100%
        case 2: vol_mult = 0.50; break; //  50%
        case 3: vol_mult = 0.25; break; //  25%
        default: exit(EXIT_FAILURE);
    }

    value *= vol_mult;

    channel->last_sample = value;
}

