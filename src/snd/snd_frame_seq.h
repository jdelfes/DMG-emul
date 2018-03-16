#ifndef snd_frame_seq_h
#define snd_frame_seq_h

typedef struct {
    uint8_t last_update;
    int last_step;
} FrameSeq;

void snd_frame_seq_tick(struct Context *this);

#endif /* snd_frame_seq_h */
