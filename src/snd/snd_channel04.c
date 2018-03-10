#include <stdio.h>
#include <stdbool.h>

#include "definitions.h"
#include "snd_duty.h"
#include "snd_channel04.h"

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

    uint16_t pow = this->sound.NR43.div_ratio_freq + 1;
    pow *= pow;
    float shift = (this->sound.NR43.shift_clock_freq > 0) ? this->sound.NR43.shift_clock_freq : 0.5;
    const uint64_t freq_period = shift * pow * 8;
    while (channel->freq_timer >= freq_period) {
        channel->freq_timer -= freq_period;

        uint8_t xor = channel->lfsr.bit0 ^ channel->lfsr.bit1;

        channel->lfsr.raw >>= 1;
        channel->lfsr.bit14 = xor;

        if (this->sound.NR43.counter_step_width) {
            channel->lfsr.bit6 = xor;
        }
    }

    float value = (channel->lfsr.raw & 1) ? -1 : 1;

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
                } else {
//                    channel->envelope_volume = 15;
                }
                break;
            default:
                break;
        }
    }

    value *= channel->envelope_volume / 15.0;

    channel->last_sample = value;
}
