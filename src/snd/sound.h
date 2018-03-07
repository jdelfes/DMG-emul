#ifndef sound_h
#define sound_h

#include "snd_channel01.h"
#include "snd_channel02.h"
#include "snd_mixer.h"
#include "snd_card.h"

struct Sound {
    union {
        uint8_t raw;
        struct {
            uint8_t sweep_shift: 3;
            uint8_t sweep_direction: 1;
            uint8_t sweep_time: 3;
        };
    } NR10;
    union {
        uint8_t raw;
        struct {
            uint8_t sound_length_data: 6;
            uint8_t wave_pattern_duty: 2;
        };
    } NR11;
    union {
        uint8_t raw;
        struct {
            uint8_t number_envelope_sweep: 3;
            uint8_t envelope_direction: 1;
            uint8_t initial_envelope_volume: 4;
        };
    } NR12;
    union {
        uint16_t _;
        struct {
            uint8_t NRx3_raw;
            uint8_t NRx4_raw;
        };
        struct {
            uint16_t channel_freq: 11;
            uint16_t : 3;
            uint16_t counter_consecutive_selection: 1;
        };
    } NR13_14;
    union {
        uint8_t raw;
        struct {
            uint8_t sound_length_data: 6;
            uint8_t wave_pattern_duty: 2;
        };
    } NR21;
    union {
        uint8_t raw;
        struct {
            uint8_t number_envelope_sweep: 3;
            uint8_t envelope_direction: 1;
            uint8_t initial_envelope_volume: 4;
        };
    } NR22;
    union {
        uint16_t _;
        struct {
            uint8_t NRx3_raw;
            uint8_t NRx4_raw;
        };
        struct {
            uint16_t channel_freq: 11;
            uint16_t : 3;
            uint16_t counter_consecutive_selection: 1;
        };
    } NR23_24;
    union {
        uint8_t raw;
        struct {
            uint8_t sO1_output_level: 3;
            uint8_t s01_output_vin: 1;
            uint8_t sO2_output_level: 3;
            uint8_t s02_output_vin: 1;
        };
    } NR50;
    union {
        uint8_t raw;
        struct {
            uint8_t sound_1_s01: 1;
            uint8_t sound_2_s01: 1;
            uint8_t sound_3_s01: 1;
            uint8_t sound_4_s01: 1;
            uint8_t sound_1_s02: 1;
            uint8_t sound_2_s02: 1;
            uint8_t sound_3_s02: 1;
            uint8_t sound_4_s02: 1;
        };
    } NR51;
    Channel01 channel01;
    Channel02 channel02;
    Mixer mixer;
    SoundCard sound_card;
};

int sound_init(struct Context *this);
void sound_tick(struct Context *this);
void sound_push(struct Context *this);
bool sound_handle_get_u8(struct Context *this, uint16_t address, uint8_t *ret_value);
bool sound_handle_set_u8(struct Context *this, uint16_t address, uint8_t value);

#endif /* sound_h */
