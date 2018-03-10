#ifndef snd_channel04_h
#define snd_channel04_h

typedef struct {
    bool enabled;
    uint64_t last_update;
    uint64_t freq_timer;
    union {
        uint16_t raw;
        struct {
            uint16_t bit0: 1;
            uint16_t bit1: 1;
            uint64_t : 4;
            uint16_t bit6: 1;
            uint16_t _: 7;
            uint16_t bit14: 1;
        };
    } lfsr;
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
} Channel04;

void snd_channel04_tick(struct Context *this);

#endif /* snd_channel04_h */
