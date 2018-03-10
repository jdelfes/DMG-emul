#include <stdio.h>
#include <stdbool.h>

#include "definitions.h"
#include "snd_duty.h"
#include "snd_channel02.h"

void snd_channel02_tick(struct Context *this) {
    Channel02 *channel = &this->sound.channel02;
    if (!channel->enabled) {
        channel->last_sample = 0;
        return;
    }
    uint64_t diff = this->cpu_timing - channel->last_update;
    channel->last_update = this->cpu_timing;
    uint64_t previous_frame_step = channel->frame.step;
    channel->frame.timer += diff;
    channel->freq_timer += diff;

    const uint64_t freq_period = (2048 - this->sound.NR23_24.channel_freq) * 4;
    if (channel->freq_timer >= freq_period) {
        channel->freq_timer -= freq_period;

        channel->duty_step++;
        if (channel->duty_step >= 8) {
            channel->duty_step = 0;
        }
    }

    float value = snd_duty_value(this->sound.NR21.wave_pattern_duty, channel->duty_step);

    if (previous_frame_step != channel->frame.step) {
        switch (channel->frame.step) {
            case 2:
            case 6:
            case 0:
            case 4:
                if (this->sound.NR23_24.counter_consecutive_selection) {
                    if (channel->length_counter > 0) {
                        channel->length_counter--;
                    } else {
                        channel->length_counter = 0;
                        channel->enabled = false;
                    }
                }
                break;
            case 7:
                if (this->sound.NR22.number_envelope_sweep > 0) {
                    channel->envelope_step++;
                    if (channel->envelope_step == this->sound.NR22.number_envelope_sweep) {
                        channel->envelope_step = 0;

                        if (this->sound.NR22.envelope_direction) {
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
