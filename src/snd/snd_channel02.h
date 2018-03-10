#ifndef snd_channel02_h
#define snd_channel02_h

typedef struct {
    bool enabled;
    uint64_t last_update;
    uint64_t freq_timer;
    uint8_t duty_step;
    uint8_t length_counter;
    uint8_t envelope_volume;
    uint8_t envelope_step;
    float last_sample;
    union {
        uint64_t timer;
        struct {
            uint64_t : 13;
            uint64_t step: 3;
        };
    } frame;
} Channel02;

void snd_channel02_tick(struct Context *this);

#endif /* snd_channel01_h */

