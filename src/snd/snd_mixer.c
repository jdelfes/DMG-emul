#include <stdio.h>
#include <stdbool.h>

#include "definitions.h"
#include "snd_mixer.h"

void snd_mixer_tick(struct Context *this) {
    float value = 0;
    if (this->sound.NR51.sound_1_s01) {
        value += this->sound.channel01.last_sample;
    }
    if (this->sound.NR51.sound_2_s01) {
        value += this->sound.channel02.last_sample;
    }
    value *= (this->sound.NR50.sO1_output_level + 1) / 8.0;
    this->sound.mixer.last_sample_ch01 = value;

    value = 0;
    if (this->sound.NR51.sound_1_s02) {
        value += this->sound.channel01.last_sample;
    }
    if (this->sound.NR51.sound_2_s02) {
        value += this->sound.channel02.last_sample;
    }
    value *= (this->sound.NR50.sO2_output_level + 1) / 8.0;
    this->sound.mixer.last_sample_ch02 = value;
}
