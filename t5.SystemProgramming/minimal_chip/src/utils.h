#ifndef UTILS_H
#define UTILS_H
#include "all.h"
#include <stdnoreturn.h>
#include "sha1.h"

/******************************************
 _______________UTILITY API________________
*******************************************/
static inline noreturn void vhalt(char const* fmt, va_list args) {
    //fflush(stdout);
    //vfprintf(stderr, fmt, args);
    //fputc('\n', stderr);

    SDL_LogMessageV(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_CRITICAL, fmt, args);
    //SDL_Quit();//TODO think about atexit(cleanup);void cleanup(void) {SDL_Quit();}
    exit(EXIT_FAILURE);
}

static inline void halt(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vhalt(fmt, args);
    va_end(args);
}

static inline void halt_assert(int cond, const char* fmt, ...) {
    if (cond) return;
    va_list args;
    va_start(args, fmt);
    vhalt(fmt, args);
    va_end(args);
}

static inline void* halt_malloc(size_t size, const char* fmt, ...) {
    void* ret = malloc(size);
    if (!ret) {
        if (fmt) {
            va_list args;
            va_start(args, fmt);
            vhalt(fmt, args);
            va_end(args);
        }
        else
            halt("Unable to malloc, requested size: %zu", size);
    }
    return ret;
}



static inline int vreport(int ret, char const* fmt, va_list args) {
    SDL_LogMessageV(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_CRITICAL, fmt, args);
    return ret;
}

static inline int report(int ret, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ret=vreport(ret, fmt, args);
    va_end(args);
    return ret;
}

static inline void* report_malloc(size_t size, const char* fmt, ...) {
    void* ret = malloc(size);
    if (!ret) {
        if (fmt) {
            va_list args;
            va_start(args, fmt);
            UNUSED_192F(vreport(0, fmt, args));
            va_end(args);
        }
        else
            UNUSED_192F(report(0, "Unable to malloc, requested size: %zu", size));
    }
    return ret;
}

static inline int report_sdl(int ret) {
    return report(ret, "%s", SDL_GetError());
}

static inline float utils_lerpf(float a, float b, float t) { return a + t * (b - a); }//{ return (1.0 - t) * a + t * b; }
static inline float utils_inverse_lerpf(float a, float b, float v){return (v - a) / (b - a);}
static inline float utils_range_mapf(float v, float vmin, float vmax, float omin, float omax) { 
    return utils_lerpf(omin, omax, utils_inverse_lerpf(vmin, vmax, v));
}
static inline float utils_rand_rangef(float min, float max) {return utils_range_mapf((float)rand(), 0.0f, (float)RAND_MAX, min, max);}
static inline float utils_clampf(float v, float min, float max) {return v < min ? min : (v > max ? max : v);}
static inline float utils_many_prodf(const float* arr, size_t n, float start) {
    double acc = (double)start; // double is preferable since it reduces rounding error
    for (size_t i = 0; i < n; ++i)
        acc *= (double)arr[i];
    return (float)acc;
}
static int utils_xy2ind(int x, int y, int w) { return y * w + x; }
static unsigned utils_xy2indu(unsigned x, unsigned y, unsigned w) { return y * w + x; }
static float utils_maxf(float x, float y) { return x > y ? x : y; }
static float utils_minf(float x, float y) { return x < y ? x : y; }


static inline int utils_clampi(int v, int min, int max) { return v < min ? min : (v > max ? max : v); }


static struct SDL_Texture* utils_res_icon(SDL_Renderer* rend, const chip_resource* res) {
    halt_assert(res->loader_kinds == CHIP_LOADER_RGBA, "rgba loader expected");
    const int w = res->loader->rgba.width;
    const int h = res->loader->rgba.height;

    SDL_Texture* tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, w, h);
    if (!tex) {
        UNUSED_192F(report_sdl(0));
        return NULL;
    }
    if (!SDL_UpdateTexture(tex, NULL, res->data, w * 4) || !SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND)) {
        UNUSED_192F(report_sdl(0));
        SDL_DestroyTexture(tex);
        return NULL;
    }
    return tex;
}
static const char* utils_sha1hash_to_hex(const SHA1_BYTE hash[SHA1_BLOCK_SIZE]) {
    static const char hex[] = "0123456789abcdef";
    static THREAD_LOCAL_192F char outhex[SHA1_BLOCK_SIZE * 2 + 1];
    memset(outhex, '\0', sizeof(outhex));
    for (int i = 0; i < SHA1_BLOCK_SIZE; ++i) {
        outhex[i * 2 + 0] = hex[(hash[i] >> 4) & 0xF];
        outhex[i * 2 + 1] = hex[(hash[i] >> 0) & 0xF];
    }
    outhex[SHA1_BLOCK_SIZE * 2] = '\0';
    return outhex;
}
static const char* utils_sha1_hex(const void* data, size_t size) {
    SHA1_CTX ctx;
    SHA1_BYTE hash[SHA1_BLOCK_SIZE];
    sha1_init(&ctx);
    sha1_update(&ctx, (const SHA1_BYTE*)data, size);
    sha1_final(&ctx, hash);
    return utils_sha1hash_to_hex(hash);
}

static inline void utils_nk_clip_copy(const struct nk_clipboard* clip, const char* text, int len) {
    if (clip->copy) clip->copy(clip->userdata, text, len);
}

static inline const void utils_nk_clip_copy_all(const struct nk_clipboard* clip, struct nk_text_edit* edit) {
    struct nk_str* str = &edit->string;
    utils_nk_clip_copy(clip, nk_str_get_const(str), nk_str_len_char(str));
}
/*END OF UTILITY API*/

/******************************************
 ____________FPS EMA COUNTER API___________
*******************************************/
//Note: EMA (exponential moving average)
typedef struct {
    double fps;
} fps_counter_state;

static inline fps_counter_state fps_counter_init() {
    return (fps_counter_state) { .fps = 0 };
}

static inline void fps_counter_update(fps_counter_state* st, double delta_secs) {
    const double shaky_fps = 1.0 / delta_secs;
    const double weight = 0.85;
    if (!isfinite(shaky_fps)) {
        st->fps = shaky_fps;
    }
    else {
        const double fifps = isfinite(st->fps) ? st->fps : 0;
        st->fps = weight * fifps + (1 - weight) * shaky_fps;
    }
}

static inline int fps_counter_format(const fps_counter_state* st, char* buf, size_t count) {
    return snprintf(buf, count, "FPS: %.0f", st->fps);
}
/*END OF FPS EMA COUNTER API*/
#endif

