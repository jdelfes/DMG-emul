#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "definitions.h"
#include "snd_mixer.h"

static float output_level_mult(uint8_t output_level) {
//    return (output_level + 1) / 8.0;
    switch (output_level) {
        case 0: return 0.125;
        case 1: return 0.250;
        case 2: return 0.375;
        case 3: return 0.500;
        case 4: return 0.625;
        case 5: return 0.750;
        case 6: return 0.875;
        case 7: return 1.000;
        default: exit(EXIT_FAILURE);
    }
}

void snd_mixer_tick(struct Context *this) {
    struct Sound *sound = &this->sound;
    float value = 0;
    if (sound->NR51.sound_1_s01) {
        value += sound->channel01.last_sample;
    }
    if (sound->NR51.sound_2_s01) {
        value += sound->channel02.last_sample;
    }
    if (sound->NR51.sound_3_s01) {
        value += sound->channel03.last_sample;
    }
    if (sound->NR51.sound_4_s01) {
        value += sound->channel04.last_sample;
    }
    value *= output_level_mult(sound->NR50.sO1_output_level);
    sound->mixer.last_sample_ch01 = value;

    value = 0;
    if (sound->NR51.sound_1_s02) {
        value += sound->channel01.last_sample;
    }
    if (sound->NR51.sound_2_s02) {
        value += sound->channel02.last_sample;
    }
    if (sound->NR51.sound_3_s02) {
        value += sound->channel03.last_sample;
    }
    if (sound->NR51.sound_4_s02) {
        value += sound->channel04.last_sample;
    }
    value *= output_level_mult(sound->NR50.sO2_output_level);
    sound->mixer.last_sample_ch02 = value;
}
