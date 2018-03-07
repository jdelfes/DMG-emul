#ifndef snd_mixer_h
#define snd_mixer_h

typedef struct {
    float last_sample_ch01;
    float last_sample_ch02;
} Mixer;

void snd_mixer_tick(struct Context *this);

#endif /* snd_mixer_h */
