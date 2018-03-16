#include <stdio.h>

#include "definitions.h"
#include "snd_channel01.h"
#include "snd_channel02.h"
#include "snd_channel03.h"
#include "snd_channel04.h"
#include "snd_frame_seq.h"

void snd_frame_seq_tick(struct Context *this) {
    const uint8_t bit = 1 << 4;
    if (((this->sound.frame_seq.last_update ^ this->timer.DIV.value) & bit)
        && (this->sound.frame_seq.last_update & bit)) {
        this->sound.frame_seq.last_step++;
        if (this->sound.frame_seq.last_step >= 8) {
            this->sound.frame_seq.last_step = 0;
        }
        int step = this->sound.frame_seq.last_step;
        snd_channel01_tick_frame_seq(this, step);
        snd_channel02_tick_frame_seq(this, step);
        snd_channel03_tick_frame_seq(this, step);
        snd_channel04_tick_frame_seq(this, step);
    }

    this->sound.frame_seq.last_update = this->timer.DIV.value;
}
