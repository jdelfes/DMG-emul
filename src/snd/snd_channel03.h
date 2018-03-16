#ifndef snd_channel03_h
#define snd_channel03_h

typedef struct {
    bool enabled;
    uint64_t last_update;
    uint64_t freq_timer;
    uint8_t wave_pat_step;
    uint16_t length_counter;
    float last_sample;
} Channel03;

void snd_channel03_tick_frame_seq(struct Context *this, int step);
void snd_channel03_tick(struct Context *this);

#endif /* snd_channel03_h */
