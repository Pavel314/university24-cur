#ifndef MONODISPLAY_H
#define MONODISPLAY_H
#include "all.h"
#include "utils.h"
/******************************************
 ____________DISPLAY_STYLE API____________
*******************************************/
typedef struct {
    int rad;
    float power;
} monodisplay_halo_config;

typedef struct {
    SDL_Color bg, fg, border;
    float gap;
    float ghosting;
    float vignette;
    SDL_FPoint noise;
    monodisplay_halo_config halo;
    SDL_FRect padcell;
} monodisplay_style;

static inline bool monodisplay_style_has_noise(const monodisplay_style* st) {
    return (st->noise.x != 1.0f || st->noise.y != 1.0f);
}
static inline bool monodisplay_style_has_vignette(const monodisplay_style* st) {return (st->vignette>0.0f);}
static inline bool monodisplay_style_has_ghosting(const monodisplay_style* st) { return (st->ghosting>0.0f); }
static inline bool monodisplay_style_has_padcell(const monodisplay_style* st) {
    return (st->padcell.x != 0.0f || st->padcell.y != 0.0f || st->padcell.w != 0.0f || st->padcell.h != 0.0f);
}
static inline bool monodisplay_style_has_halo(const monodisplay_style* st) {return st->halo.rad > 0;}

typedef enum {
    MONODISPLAY_STYLE_BLACKWHITE,
    MONODISPLAY_STYLE_REDNEON,
    MONODISPLAY_STYLE_SIEMENS_ORRANGE,
    MONODISPLAY_STYLE_NOKIA_GREEN,
    MONODISPLAY_STYLE_OLDMONITOR,
    MONODISPLAY_STYLE_MAX,
} monodisplay_style_kind;

static const monodisplay_style monodisplay_styles[MONODISPLAY_STYLE_MAX] = {
    [MONODISPLAY_STYLE_BLACKWHITE] = {
        .bg = { 0,0,0,255 },
        .fg = { 255,255,255,255 },
        .border = { 180, 100, 30, 255 },
        .gap = 0.0f,
        .ghosting = 0.50f,
        .vignette = 0.00f,
        .noise = {1.00, 1.00},
        .halo = {0},
    },
    [MONODISPLAY_STYLE_REDNEON] = {
        .bg = {0,0,0,255},
        .fg = { 255,0,0,255 },
        .border = { 180, 100, 30, 255 },
        .gap = 0.0f,
        .ghosting = 0.90f,
        .vignette = 0.00f,
        .noise = {1.00, 1.00},
        .halo = {3, 0.7f},
    },
    [MONODISPLAY_STYLE_SIEMENS_ORRANGE] = {
        .bg = { 255, 160, 20, 255 },
        .fg = { 20, 10, 0, 255 },
        .border = { 180, 100, 30, 255 },
        .gap = 0.0f,
        .ghosting = 0.00f,
        .vignette = 0.05f,
        .noise = {1.00, 1.00},
        .halo = {0},
        .padcell = {-2.0f, -2.0f, 0.0f, 0.0f}
    },
    [MONODISPLAY_STYLE_NOKIA_GREEN] = {
        .bg = { 168, 200, 108, 255 },
        .fg = {  28,  48,  18, 255 },
        .border = {  70,  85,  55, 255 },
        .gap = 0.0f,
        .ghosting = 0.3f,
        .vignette = 0.05f,
        .noise = { 1.0f, 1.00f },
        .halo = {0},
        .padcell = { -2.0f, -2.0f, 0.0f, 0.0f },
    },
    [MONODISPLAY_STYLE_OLDMONITOR] = {
        .bg = { 255,255,255, 255 },
        .fg = { 0,0,0, 255 },
        .border = { 180, 100, 30, 255 },
        .gap = 0.0f,
        .ghosting = 0.50f,
        .vignette = 0.50f,
        .noise = {0.96f, 1.00f},
        .halo = {0}
    }
};


typedef struct {
    float halo;
    float ghosting;
} monodisplay__buffer;

typedef enum {
    MONODISPLAY_ROTATION_0,
    MONODISPLAY_ROTATION_90,
    MONODISPLAY_ROTATION_180,
    MONODISPLAY_ROTATION_270,
    MONODISPLAY_ROTATION_MAX,
} monodisplay_rotation_kind;

typedef struct {
    monodisplay_style style;
    SDL_FRect bounds;
    monodisplay_rotation_kind rotation;
    int cellsx, cellsy;
    monodisplay__buffer* _cellsbuf;
} monodisplay_renderer;

static void monodisplay_renderer_set_cells(monodisplay_renderer* st, int cellsx, int cellsy) {
    const size_t old_cells = st->cellsx * st->cellsy * sizeof(monodisplay__buffer);
    const size_t new_cells = cellsx * cellsy * sizeof(monodisplay__buffer);
    if (old_cells != new_cells) {
        free(st->_cellsbuf);
        st->_cellsbuf = (monodisplay__buffer*)report_malloc(new_cells, NULL);
    }
    if (st->_cellsbuf) memset(st->_cellsbuf, 0, new_cells);
    st->cellsx = cellsx;
    st->cellsy = cellsy;
}

static void monodisplay_render_set_bounds(monodisplay_renderer* st, float x, float y, float w, float h) {
    st->bounds = (SDL_FRect){x,y,w,h};
}

static void monodisplay_render_set_rotation(monodisplay_renderer* st, monodisplay_rotation_kind rotation) {
    if ((st->rotation & 1) != (rotation & 1)) {
        monodisplay_renderer_set_cells(st, st->cellsy, st->cellsx);
    }
    st->rotation = rotation;
}



static void monodisplay_renderer_set_style(monodisplay_renderer* st, const monodisplay_style* style) {
    st->style = *style;
    monodisplay__buffer* buf = st->_cellsbuf;
    if (buf) {
        for (int i = 0;i < st->cellsx * st->cellsy;++i)
            buf[i].ghosting = buf[i].halo = 0.0f;
    }
}

static void monodisplay_renderer_init(monodisplay_renderer* st, monodisplay_style style, SDL_FRect bounds, int cellsx, int cellsy) {
    monodisplay_renderer ret = { .style = {0}, .bounds = {0.0f, 0.0f, 0.0f, 0.0f}, MONODISPLAY_ROTATION_0, .cellsx = 0, .cellsy = 0};
    monodisplay_renderer_set_cells(&ret, cellsx, cellsy);
    monodisplay_render_set_bounds(&ret, bounds.x, bounds.y, bounds.w, bounds.h);
    monodisplay_render_set_rotation(&ret, MONODISPLAY_ROTATION_0);
    monodisplay_renderer_set_style(&ret, &style);
    *st = ret;
}

static void monodisplay_renderer_free(monodisplay_renderer* st) {
    if (!st) return;
    free(st->_cellsbuf);
    memset(st, 0, sizeof * st);
}

static inline int monodisplay__xy2ind(const monodisplay_renderer* st, int x, int y, int stride) {
    const int w = (st->rotation & 1) ? st->cellsy : st->cellsx;
    const int h = (st->rotation & 1) ? st->cellsx : st->cellsy;
    switch (st->rotation) {
        case MONODISPLAY_ROTATION_0: return utils_xy2ind(x, y, stride);
        case MONODISPLAY_ROTATION_90: return utils_xy2ind(y, h - 1 - x, stride);
        case MONODISPLAY_ROTATION_180: return utils_xy2ind(w - 1 - x, h - 1 - y, stride);
        case MONODISPLAY_ROTATION_270: return utils_xy2ind(w - 1 - y, x, stride);
        default: halt_assert(0, "Unknown branch in monodisplay rotation");
    }
    return 0;
}

static float monodisplay__vignette_effect(SDL_FRect bounds, float intensity, float px, float py) {
    const float dx = utils_range_mapf(px, bounds.x, bounds.x + bounds.w, -1.0f, 1.0f);
    const float dy = utils_range_mapf(py, bounds.y, bounds.y + bounds.h, -1.0f, 1.0f);
    const float dist = sqrtf(dx * dx + dy * dy);
    return utils_clampf(1.0f - intensity * dist,0.0f, 1.0f); //This goes out of the range 0...1 (since sqrt(2)) at the edges. Use clamp
}

static float monodisplay__noise_effect(float vmin, float vmax) {
    return utils_rand_rangef(vmin, vmax);
}


static SDL_FRect monodisplay__padcell_effect(SDL_FRect cell, SDL_FRect pad) {
    return (SDL_FRect) {cell.x + pad.x, cell.y + pad.y, cell.w - pad.x + pad.w, cell.h - pad.y + pad.h};
}


static void monodisplay__halo_effect(monodisplay_renderer* st, int cx, int cy) {
    monodisplay_halo_config halo = st->style.halo;
    const int cellind = utils_xy2ind(cx, cy, st->cellsx);
    st->_cellsbuf[cellind].halo = 1.0f;
    for (int i = -halo.rad;i <= halo.rad;++i) {
        for (int j = -halo.rad; j <= halo.rad; ++j) {
            int x = cx + i, y = cy + j;
            int ind = utils_xy2ind(x, y, st->cellsx);
            if (x < 0 || y < 0 || x >= st->cellsx || y >= st->cellsy || ind == cellind) continue;
            float dist = sqrtf((float)i * i + j * j);
            //float sigma = halo.rad / 2.0f;
            //float strength = 0.9f * expf(-dd * dd / (2.0f * sigma * sigma));
            float halov = halo.power * (1.0f - dist / (halo.rad + 1));
            halov = utils_clampf(halov, 0.0f, 1.0f);
            float* bufhalo = &st->_cellsbuf[ind].halo;
            *bufhalo = utils_maxf(*bufhalo, halov);
        }
    }
}

static void monodisplay__ghostring_effect(monodisplay_renderer* st, int cellind, bool cell_on) {
    float* bufghost = &st->_cellsbuf[cellind].ghosting;
    *bufghost = cell_on ? 1.0f : utils_lerpf(0.0f, *bufghost, st->style.ghosting);
}

static SDL_FRect monodisplay__get_cell_rect(monodisplay_renderer* st, float cell_w, float cell_h, int cx, int cy) {
    const monodisplay_style* style = &st->style;
    if (cell_w <= 0.0f) cell_w = (st->bounds.w - (st->cellsx + 1) * style->gap) / st->cellsx;
    if (cell_h <= 0.0f) cell_h = (st->bounds.h - (st->cellsy + 1) * style->gap) / st->cellsy;
    
    return (SDL_FRect) {
        st->bounds.x + cx * (cell_w + style->gap) + style->gap,
        st->bounds.y + cy * (cell_h + style->gap) + style->gap,
        cell_w,
        cell_h
    };
}

//With fullzscreen, this can lead to unwanted stripes at the top and bottom
//static SDL_FRect monodisplay__get_cell_rect(monodisplay_renderer* st, float cell_w, float cell_h, int cx, int cy) {
//    const monodisplay_style* style = &st->style;
//    if (cell_w <= 0.0f) cell_w = (st->bounds.w - (st->cellsx + 1) * style->gap) / st->cellsx;
//    if (cell_h <= 0.0f) cell_h = (st->bounds.h - (st->cellsy + 1) * style->gap) / st->cellsy;
//
//    const float s = utils_minf(cell_w, cell_h);
//    cell_w = cell_h = s;
//
//    const float grid_w = s * st->cellsx + (st->cellsx + 1) * style->gap;
//    const float grid_h = s * st->cellsy + (st->cellsy + 1) * style->gap;
//
//    const float ox = st->bounds.x + (st->bounds.w - grid_w) * 0.5f;
//    const float oy = st->bounds.y + (st->bounds.h - grid_h) * 0.5f;
//
//    return (SDL_FRect) {
//            ox + style->gap + cx * (s + style->gap),
//            oy + style->gap + cy * (s + style->gap),
//            s,
//            s
//    };
//}


static void monodisplay_renderer_draw(monodisplay_renderer* st, SDL_Renderer* renderer, const bool* cells, unsigned cells_per_row) {
    if (!renderer || !cells) return;
    //if (cells_per_row == 0) cells_per_row = st->cellsx;
    const int cellsx = st->cellsx, cellsy = st->cellsy;
    const int cellscount = cellsx * cellsy;

    const monodisplay_style* style = &st->style;
    SDL_Color bg = style->bg, fg = style->fg;
    SDL_FRect bounds = st->bounds;
    monodisplay__buffer* buf = st->_cellsbuf;

    const bool has_halo = buf && monodisplay_style_has_halo(style);
    const bool has_ghost = buf && monodisplay_style_has_ghosting(style);

    SDL_FRect cellwh = monodisplay__get_cell_rect(st, -1.0f, -1.0f, 0, 0);

    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(renderer, &bounds);


    for (int i = 0;i < cellscount;++i) 
        if (has_halo) buf[i].halo = 0.0f;

    for (int y = 0; y < cellsy; ++y) {
        for (int x = 0; x < cellsx; ++x) {
            if (has_halo && cells[monodisplay__xy2ind(st, x, y, cells_per_row)]) {
                monodisplay__halo_effect(st, x, y);
            }
        }
    }

    for (int y = 0; y < cellsy; ++y) {
        for (int x = 0; x < cellsx; ++x) {
            SDL_FRect cell = monodisplay__get_cell_rect(st, cellwh.w, cellwh.h, x, y);

            int bufidx = utils_xy2ind(x, y, cellsx);
            const bool cell_on = cells[monodisplay__xy2ind(st, x, y, cells_per_row)];
 
            float intensity = 1.0f;
            if (monodisplay_style_has_noise(style))
                intensity *= monodisplay__noise_effect(style->noise.x, style->noise.y);
            if (monodisplay_style_has_vignette(style))
                intensity *= monodisplay__vignette_effect(bounds, style->vignette, cell.x + cell.w * 0.5f, cell.y + cell.h * 0.5f);

            SDL_FColor finbg = { 
                utils_clampf(bg.r * intensity, 0.0f, 255.0f),
                utils_clampf(bg.g * intensity, 0.0f, 255.0f), 
                utils_clampf(bg.b * intensity, 0.0f, 255.0f), 
                fg.a 
            };
            SDL_SetRenderDrawColor(renderer, (Uint8)(finbg.r), (Uint8)(finbg.g), (Uint8)(finbg.b), (Uint8)(finbg.a));
            SDL_RenderFillRect(renderer, &cell);

            intensity = 0.0f;
            bool has_effect = false;

            if (has_halo) {
                intensity = utils_maxf(intensity, buf[bufidx].halo);
                has_effect = true;
            }
            if (has_ghost) {
                monodisplay__ghostring_effect(st, bufidx, cell_on);
                intensity = utils_maxf(intensity, buf[bufidx].ghosting);
                has_effect = true;
            }
            if (!has_effect)
                intensity = cell_on ? 1.0f : 0.0f;

            SDL_SetRenderDrawColor(renderer, fg.r, fg.g, fg.b, (Uint8)(intensity * 255.0f));
            SDL_RenderFillRect(renderer, &cell);
        }
    }

    for (int y = 0; y < cellsy; ++y) {
        for (int x = 0; x < cellsx; ++x) {
            SDL_FRect cell = monodisplay__get_cell_rect(st, cellwh.w, cellwh.h, x, y);
            if (cells[monodisplay__xy2ind(st, x, y, cells_per_row)] && monodisplay_style_has_padcell(style)) {
                SDL_Color padclr = { fg.r, fg.g, fg.b, 128 };
                SDL_SetRenderDrawColor(renderer, padclr.r, padclr.g, padclr.b, padclr.a);
                SDL_FRect padcell_rect = monodisplay__padcell_effect(cell, style->padcell);
                SDL_RenderFillRect(renderer, &padcell_rect);
            }
        }
    }
    

    //if (style->border.a > 0) {
    //    SDL_SetRenderDrawColor(renderer, style->border.r, style->border.g, style->border.b, style->border.a);
    //    SDL_RenderRect(renderer, &bounds);
    //}
}
/*END OF DISPLAY_STYLE API*/
#endif