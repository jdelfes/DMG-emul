#include <stdio.h>
#include <stdbool.h>

#include <SDL_audio.h>

#include "definitions.h"
#include "snd_card.h"

#ifdef ENABLE_SOUND
SDL_AudioDeviceID audio_dev;
SDL_AudioSpec spec = {0};
#endif

int snd_card_init() {
#ifdef ENABLE_SOUND
    spec.freq = 44100;
    spec.format = AUDIO_F32SYS;
    spec.channels = 2;
    audio_dev = SDL_OpenAudioDevice(NULL, 0, &spec, &spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    if (audio_dev == 0) {
        fprintf(stderr,  "SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    printf("freq: %i\n", spec.freq);
    printf("format: %i\n", spec.format);
    printf("channels: %i\n", spec.channels);
    printf("silence: %i\n", spec.silence);
    printf("samples: %i\n", spec.samples);
    printf("size: %i\n", spec.size);

    SDL_PauseAudioDevice(audio_dev, 0);
#endif
    return 0;
}

void snd_card_tick(struct Context *this) {
#ifdef ENABLE_SOUND
    const int buffer_positions = sizeof(this->sound.sound_card.buffer) / sizeof(this->sound.sound_card.buffer[0]);
    SoundCard *snd_card = &this->sound.sound_card;

    uint64_t diff = this->cpu_timing - snd_card->last_update;
    snd_card->last_update = this->cpu_timing;
    snd_card->sample_timer += diff;

    const float sound_card_period = ((float) this->cpu.clock) / spec.freq;
    if (snd_card->sample_timer >= sound_card_period) {
        snd_card->sample_timer -= sound_card_period;
        snd_card->buffer[snd_card->buffer_pos].left = this->sound.mixer.last_sample_ch02;
        snd_card->buffer[snd_card->buffer_pos].right = this->sound.mixer.last_sample_ch01;
        snd_card->buffer_pos++;
    }

    if (snd_card->buffer_pos >= buffer_positions) {
        snd_card_flush(this);
    }
#endif
}

void snd_card_flush(struct Context *this) {
#ifdef ENABLE_SOUND
    const int item_size = sizeof(this->sound.sound_card.buffer[0]);
    SoundCard *snd_card = &this->sound.sound_card;

    if (snd_card->buffer_pos > 0) {
        while (SDL_GetQueuedAudioSize(audio_dev) > spec.size) ;
//        printf("sending: %i %i\n", snd_card->buffer_pos, SDL_GetQueuedAudioSize(audio_dev));
        SDL_QueueAudio(audio_dev, snd_card->buffer, item_size * snd_card->buffer_pos);

        memset(snd_card->buffer, 0, item_size);
        snd_card->buffer_pos = 0;
    }
#endif
}
