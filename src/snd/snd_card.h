#ifndef snd_card_h
#define snd_card_h

typedef struct {
    uint64_t last_update;
    float sample_timer;
    int buffer_pos;
    struct {
        float left;
        float right;
    } buffer[1024];
} SoundCard;

int snd_card_init(void);
void snd_card_tick(struct Context *this);
void snd_card_flush(struct Context *this);

#endif /* snd_card_h */
