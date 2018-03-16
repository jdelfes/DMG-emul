#include <stdio.h>
#include <stdbool.h>

#include "definitions.h"
#include "snd_duty.h"
#include "snd_channel02.h"

void snd_channel02_tick_frame_seq(struct Context *this, int step) {
    Channel02 *channel = &this->sound.channel02;

    switch (step) {
        case 0:
        case 2:
        case 4:
        case 6:
            if (this->sound.NR23_24.counter_consecutive_selection) {
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
                    if (this->sound.NR22.envelope_direction) {
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
                    channel->envelope.period = this->sound.NR22.number_envelope_sweep;
                }
            }
            break;
    }
}

void snd_channel02_tick(struct Context *this) {
    Channel02 *channel = &this->sound.channel02;

    if (!channel->enabled) {
        channel->last_sample = 0;
        return;
    }
    uint64_t diff = this->cpu_timing - channel->last_update;
    channel->last_update = this->cpu_timing;
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

    value *= channel->envelope.volume / 15.0;

    channel->last_sample = value;
}
