#ifndef snd_channel01_h
#define snd_channel01_h

typedef struct {
    bool enabled;
    uint64_t last_update;
    struct {
        uint8_t period;
        bool enabled;
    } freq_sweep;
    uint64_t freq_timer;
    uint8_t duty_step;
    uint8_t length_counter;
    struct {
        bool enabled;
        uint8_t volume;
        uint8_t period;
    } envelope;
    float last_sample;
} Channel01;

void snd_channel01_tick_frame_seq(struct Context *this, int step);
void snd_channel01_tick(struct Context *this);
uint16_t snd_channel01_freq_sweep_new_value(struct Context *this);

#endif /* snd_channel01_h */
