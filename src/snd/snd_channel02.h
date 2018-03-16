#ifndef snd_channel02_h
#define snd_channel02_h

typedef struct {
    bool enabled;
    uint64_t last_update;
    uint64_t freq_timer;
    uint8_t duty_step;
    uint8_t length_counter;
    struct {
        bool enabled;
        uint8_t volume;
        uint8_t period;
    } envelope;
    float last_sample;
} Channel02;

void snd_channel02_tick_frame_seq(struct Context *this, int step);
void snd_channel02_tick(struct Context *this);

#endif /* snd_channel01_h */

