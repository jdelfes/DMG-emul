#ifndef snd_channel04_h
#define snd_channel04_h

typedef struct {
    bool enabled;
    uint64_t last_update;
    uint64_t freq_timer;
    union __attribute__((packed)) {
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
    struct {
        bool enabled;
        uint8_t volume;
        uint8_t period;
    } envelope;
    float last_sample;
} Channel04;

void snd_channel04_tick_frame_seq(struct Context *this, int step);
void snd_channel04_tick(struct Context *this);

#endif /* snd_channel04_h */
