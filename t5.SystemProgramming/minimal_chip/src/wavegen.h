#ifndef WAVEGEN_H
#define WAVEGEN_H
#include "all.h"
#include "utils.h"
/******************************************
 _______________WAVEGEN API_______________
*******************************************/

typedef enum {
    WAVE_SQUARE,
    WAVE_SINE,
    WAVE_TRIANGLE,
    WAVE_SAW
} wave_type;

typedef struct {
    wave_type type;
    int sample_rate;
    float freq;
    float amp;
    float phase;
} wavegen;

static void wavegen_init(wavegen* st, wave_type type, int sample_rate, float freq, float amp) {
    wavegen ret = {
        .sample_rate = sample_rate,
        .type = type,
        .freq = freq,
        .amp = amp,
        .phase = 0
    };
    *st = ret;
}

static void wavegen_free(wavegen* st) {
    if (!st) return;
    memset(st, 0, sizeof * st);
}

static void wavegen_fill_f32(wavegen* st, float* out, int samples) {
    const float phase_inc = st->freq / st->sample_rate;
    for (int i = 0; i < samples; i++) {
        float phase = st->phase;
        float val = 0.0;
        switch (st->type) {
            case WAVE_SQUARE: val = (phase < 0.5f) ? 1.0f : -1.0f; break;
            case WAVE_SINE: val = sinf(2.0f * (float)M_PI * phase); break;
            case WAVE_TRIANGLE: val = 2.0f * fabsf(2.0f * (phase - floorf(phase + 0.5f))) - 1.0f; break;
            case WAVE_SAW: val = 2.0f * (phase - floorf(phase + 0.5f)); break;
            default: halt_assert(0, "Unknown wave type, %d", (int)st->type);
        }
        out[i] = val * st->amp;
        st->phase += phase_inc;
        if (st->phase >= 1.0) st->phase -= 1.0;
    }
}
/*END OF WAVEGEN API*/

/******************************************
 _____________WAVEGEN SDL3 API_____________
*******************************************/
//Note: this is the extensions section for wavegen and sdl3
static SDL_AudioSpec wavegen_sdl3spec(wavegen* st) {
    return (SDL_AudioSpec) { .format = SDL_AUDIO_F32, .channels = 1, .freq = st->sample_rate };
}
static void wavegen_sdl3callback(void* vst, SDL_AudioStream* stream, int additional_amount, int total_amount) {
    UNUSED_192F(total_amount);
    wavegen* st = (wavegen*)(vst);
    enum { tmpbuf_sz = 4096 };
    static THREAD_LOCAL_192F float tmpbuf[tmpbuf_sz];
    for (int needed = additional_amount / sizeof(float); needed > 0;) {
        int chunk = needed < tmpbuf_sz? needed : tmpbuf_sz;
        wavegen_fill_f32(st, tmpbuf, chunk);
        //TODO SDL_PutAudioStreamDataNoCopy
        bool ok = SDL_PutAudioStreamData(stream, tmpbuf, chunk * sizeof(float));
        UNUSED_192F(ok);//TODO what can we do with 'ok'?
        needed -= chunk;
    }
}
/*END OF WAVEGEN SDL3 API*/
#endif