#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "definitions.h"
#include "snd_duty.h"
#include "snd_channel01.h"

void snd_channel01_tick(struct Context *this) {
    Channel01 *channel = &this->sound.channel01;
    if (!channel->enabled) {
        channel->last_sample = 0;
        return;
    }

    uint64_t diff = this->cpu_timing - channel->last_update;
    channel->last_update = this->cpu_timing;
    uint64_t previous_frame_step = channel->frame.step;
    channel->frame.timer += diff;
    channel->freq_timer += diff;

    const uint64_t freq_period = (2048 - this->sound.NR13_14.channel_freq) * 4;
    if (channel->freq_timer >= freq_period) {
        channel->freq_timer -= freq_period;

        channel->duty_step++;
        if (channel->duty_step >= 8) {
            channel->duty_step = 0;
        }
    }

    float value = snd_duty_value(this->sound.NR11.wave_pattern_duty, channel->duty_step);

    if (previous_frame_step != channel->frame.step) {
        switch (channel->frame.step) {
            case 2:
            case 6:
                // sweep
                if (this->sound.NR10.sweep_time > 0) {
                    channel->sweep_freq_step++;
                    if (channel->sweep_freq_step == this->sound.NR10.sweep_time) {
                        channel->sweep_freq_step = 0;
                    }
                    uint16_t freq = this->sound.NR13_14.channel_freq;
                    freq >>= this->sound.NR10.sweep_shift;
                    if (this->sound.NR10.sweep_direction) {
                        freq = this->sound.NR13_14.channel_freq + freq;
                        if (freq > 2047) {
                            channel->enabled = false;
                            freq = 2047;
                        }
                    } else {
                        if (this->sound.NR13_14.channel_freq > freq) {
                            freq = this->sound.NR13_14.channel_freq - freq;
                        }
                    }
                    this->sound.NR13_14.channel_freq = freq;
                }
            case 0:
            case 4:
                if (this->sound.NR13_14.counter_consecutive_selection) {
                    if (this->sound.NR11.sound_length_data > 0) {
                        this->sound.NR11.sound_length_data--;
                    } else {
                        this->sound.NR11.sound_length_data = 0;
                        channel->enabled = false;
                    }
                }
                break;
            case 7:
                if (this->sound.NR12.number_envelope_sweep > 0) {
                    channel->envelope_step++;
                    if (channel->envelope_step == this->sound.NR12.number_envelope_sweep) {
                        channel->envelope_step = 0;

                        if (this->sound.NR12.envelope_direction) {
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
