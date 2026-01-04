#ifndef CLOCKGEN_H
#define CLOCKGEN_H
#include "all.h"
#include "utils.h"
/******************************************
 _______________CLOCKGEN API_______________
*******************************************/
enum {
    CLOCKGEN_MAX_DEVICES = 16
};

typedef struct {
    double freq[CLOCKGEN_MAX_DEVICES];
    double accum[CLOCKGEN_MAX_DEVICES];
    int count[CLOCKGEN_MAX_DEVICES];
} clockgen;

static void clockgen_init(clockgen* st, const double* freqes, int count) {
    halt_assert(count <= CLOCKGEN_MAX_DEVICES, "Freqes array too long, length is %d", count);
    clockgen ret = { 0 };
    memcpy(ret.freq, freqes, count * sizeof(freqes[0]));
    *st = ret;
}

static const int* clockgen_update(clockgen* st, double delta) {
    for (int i = 0; i < CLOCKGEN_MAX_DEVICES; ++i) {
        st->count[i] = 0;
        if (st->freq[i] <= 0.0) continue;
        st->accum[i] += st->freq[i] * delta;
        st->count[i] = (int)st->accum[i];
        st->accum[i] -= st->count[i];
    }
    return st->count;
}

static void clockgen_free(clockgen* st) {
    if (!st) return;
    memset(st, 0, sizeof * st);
}
/*END OF CLOCKGEN API*/
#endif