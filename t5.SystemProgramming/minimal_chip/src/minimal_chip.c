#define NK_IMPLEMENTATION
#define NK_SDL3_RENDERER_IMPLEMENTATION
#include "all.h"
#include "utils.h"
#include "chip8.h"
#include "clockgen.h"
#include "wavegen.h"
#include "monodisplay.h"
#include "ui.h"
#include "version.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

/*
TODO
1)The romdb (python script) skips entries if the given platform is not supported, 
but it would be better to leave it, then the program will be able to output the error that the rom is not supported.
Although, some games may be indexed incorrectly. [For example, Sub-Terr8nia, specified as xochip but workly fine on schip]
2)[Done] Screen rotation(ui button) needed by Sub-Terr8nia
3)screenRotation field may be readed from romdb[But only Sub-Terr8nia use it]
4)When window resizing, check and correct the bounds of the ui_rom_info
5)chip_rom_init(defualt_rom_res->data, defualt_rom_res->size, NULL); -- const correctness
[Ideas for future improvement]
1)Saving and loading the VM's state to a file
2)Combobox for precise platform selection. Since romdb autoconfiguration is not always ideal[5-quirks.ch8 fox example]
*/

enum {
    CHIP_DEVICE_VM = 0,
    CHIP_DEVICE_VMTIMER,
    CHIP_DEVICE_MAX,
};
static const double chip_standard_freques[CHIP_DEVICE_MAX] = { [CHIP_DEVICE_VM] = 1500,[CHIP_DEVICE_VMTIMER] = 60 };

static const SDL_Scancode chip_keymapper[] = {
    [0x0] = SDL_SCANCODE_X, 
    [0x1] = SDL_SCANCODE_1, [0x2] = SDL_SCANCODE_2, [0x3] = SDL_SCANCODE_3,
    [0x4] = SDL_SCANCODE_Q, [0x5] = SDL_SCANCODE_W, [0x6] = SDL_SCANCODE_E, 
    [0x7] = SDL_SCANCODE_A, [0x8] = SDL_SCANCODE_S, [0x9] = SDL_SCANCODE_D,
    [0xA] = SDL_SCANCODE_Z, 
    [0xB] = SDL_SCANCODE_C, 
    [0xC] = SDL_SCANCODE_4, [0xD] = SDL_SCANCODE_R, [0xE] = SDL_SCANCODE_F, [0xF] = SDL_SCANCODE_V,
};


static const float app_toolbox_h = 35.0f;
static inline const chip_rom* app_get_default_rom() {
    //Note. The main problem with using the default rom is that it requires the appropriate features from the chip interpreter
    //for starsky2091 is hires
    static THREAD_LOCAL_192F chip_rom default_rom;
    static bool init = false;
    if (!init) {
        const chip_resource* defualt_rom_res = chip_get_res(CHIP_DEFAULT_ROM);
        default_rom = chip_rom_init(defualt_rom_res->data, defualt_rom_res->size, NULL);
        init = true;
    }
    return &default_rom;
}

typedef enum {RES_ICON_OPEN, RES_ICON_PLAYPAUSE, RES_ICON_RESET, RES_ICON_VOLUME, RES_ICON_ROMINFO, RES_ICON_ABOUT, RES_ICON_ROTATE, RES_ICON_MAX} res_icons;
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_AudioStream* audio;
    bool sound_allow;
    struct nk_context* nk_ctx;
    Uint64 last_time;

    chip8 chip;
    chip_rom chip_rom;
    clockgen chip_time;
    wavegen chip_wave;
    monodisplay_renderer chip_display;
    monodisplay_style_kind chip_display_style;

    struct nk_image icons[RES_ICON_MAX];
    ui_info_window ui_rom_info;
    bool ui_about_shown;

    fps_counter_state fps_counter;
} app_state;

static void app_switch_sound(app_state* st, bool needed) {
    if (!st->audio) return;//audio system was not inited
    bool paused = SDL_AudioStreamDevicePaused(st->audio);
    if (!st->sound_allow) { if (!paused) SDL_PauseAudioStreamDevice(st->audio); return; }
    if (needed == paused) {
        bool ok = needed ? SDL_ResumeAudioStreamDevice(st->audio) : SDL_PauseAudioStreamDevice(st->audio);
        if (!ok) UNUSED_192F(report_sdl(0));
    }
}
static void app_set_font(app_state* st, float font_scale) {
    struct nk_font_config config = nk_font_config(0);
    ;
    /* set up the font atlas and add desired font; note that font sizes are
     * multiplied by font_scale to produce better results at higher DPIs */
    /* FIXME: This looks odd, but all existing renderer no calls nk_font_atlas_clear
     * inside nk_sdl_font_stash_begin. This might cause a leak if called multiple times */
    struct nk_font_atlas* atlas = nk_sdl_font_stash_begin(st->nk_ctx);
    struct nk_font* font = nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
    nk_sdl_font_stash_end(st->nk_ctx);
    /* this hack makes the font appear to be scaled down to the desired
     * size and is only necessary when font_scale > 1 */
    font->handle.height /= font_scale;
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    nk_style_set_font(st->nk_ctx, &font->handle);
}

static void app_on_resize(app_state* st) {
    SDL_Rect rect;
    SDL_GetRenderViewport(st->renderer, &rect);
    monodisplay_render_set_bounds(&st->chip_display, 0.0f, app_toolbox_h, rect.w, rect.h - app_toolbox_h);
}

static void app_on_scale_size_changed(app_state* st) {
    float scale = SDL_GetWindowDisplayScale(st->window);
    assert(scale > 0);
    SDL_SetRenderScale(st->renderer, scale, scale);
    app_set_font(st, scale);
    app_on_resize(st);
}

static void app_set_monodisplay_rotation(app_state* st, monodisplay_rotation_kind rotation) {
    monodisplay_render_set_rotation(&st->chip_display, rotation);
}

static void app_set_monodisplay_style(app_state* st, monodisplay_style_kind style) {
    st->chip_display_style = style;
    monodisplay_renderer_set_style(&st->chip_display, &monodisplay_styles[style]);
}

static void app_set_rom(app_state* st, const chip_rom* rom, const chip_config* cfg, const char* desc, double hz, const char* rom_name) {
    if (!rom) rom = app_get_default_rom();
    if (!cfg) cfg = &chip_presets[CHIP_PRESET_OCTO_CHIP];
    if (!desc) desc = "";
    if (hz < 0) hz = chip_standard_freques[CHIP_DEVICE_VM];
    if (!rom_name) rom_name = "";
    chip_rom_free(&st->chip_rom);
    st->chip_rom = *rom;
    st->chip_time.freq[CHIP_DEVICE_VM] = hz;
    ui_info_window_set_text(&st->ui_rom_info, desc);
    if (desc && *desc) st->ui_rom_info.shown = true;
    chip8_reinit(&st->chip, cfg, &st->chip_rom, true);
    app_set_monodisplay_rotation(st, MONODISPLAY_ROTATION_0);

    char title[128];
    utils_strcat_many(title, sizeof(title), " - ", APP_NAME, rom_name, NULL);
    SDL_SetWindowTitle(st->window, title);
}

static void app_on_open_rom(app_state* st, const char* path) {
    chip_rom newrom;
    if (chip_rom_from_file(&newrom, path, CHIP8_MAX_ROM_SIZE)) {
        const char* filename = utils_filepath_name(path);
        const char* sha1hex = utils_sha1_hex(newrom.mem, newrom.size);
        const romdb_entry* dbentry = romdb_get(sha1hex);
        if (dbentry) {
            static const int preset_mapper[] = {
                [ROMDB_PLATFORM_ORIGINALCHIP8] = CHIP_PRESET_ORIGINAL_CHIP8,
                [ROMDB_PLATFORM_HYBRIDVIP] = CHIP_PRESET_HYBRID_CHIP8,
                [ROMDB_PLATFORM_MODERNCHIP8] = CHIP_PRESET_MODERN_CHIP8,
                [ROMDB_PLATFORM_CHIP_8X] = CHIP_PRESET_CHIP8X,
                [ROMDB_PLATFORM_CHIP48] = CHIP_PRESET_CHIP48,
                [ROMDB_PLATFORM_SUPERCHIP1] = CHIP_PRESET_SCHIP_1_0,
                [ROMDB_PLATFORM_SUPERCHIP] = CHIP_PRESET_SCHIP,
            };
            const char spacefight2091sha1[] = "a05844df3305738e4030512f0063db2fe4f3bd11";
            const bool isspacefight2091 = strcmp(spacefight2091sha1, sha1hex) == 0;
            chip_preset_kind preset = isspacefight2091? CHIP_PRESET_SPACEFIGHT2091_CHIP: preset_mapper[dbentry->platform];
            app_set_rom(st, &newrom, &chip_presets[preset], dbentry->desc, dbentry->tickrate * 60.0, filename);
        }
        else {
            app_set_rom(st, &newrom, NULL, "", -1.0, filename);
        }
    }
    else {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Unable to load selected rom. The file may be too large", st->window);
        app_set_rom(st, NULL, NULL, "", -1.0, NULL);
    }
}

typedef struct {
    app_state* st;
    char* path;
} app_open_rom_info;

static void app_on_open_rom_cb(void* vinfo) {
    app_open_rom_info* info = (app_open_rom_info*)vinfo;
    app_on_open_rom(info->st, info->path);
    SDL_free(info->path);
    free(info);
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
void app_on_open_rom_ems(uintptr_t ust, const char* path) {
    app_state* st = (app_state*)ust;
    app_on_open_rom(st, path);
}
#endif

static void app_showdialog_cb(void* vst, const char* const* filelist, int filter) {
    app_state* st = (app_state*)vst;
    if (filelist && filelist[0]) {
        app_open_rom_info* info = halt_malloc(sizeof * info, NULL);
        info->st = (app_state*)vst;
        info->path = SDL_strdup(filelist[0]);
        SDL_RunOnMainThread(app_on_open_rom_cb, info, false);
    }
}

static void app_showdialog(app_state* st) {
#ifdef __EMSCRIPTEN__
    EM_ASM({
      if (!Module._mc_input) {
          var i = document.createElement('input');
          i.type = 'file';
          i.style.display = 'none';
          document.body.appendChild(i);
          i.addEventListener('change', async function() {
              var f = i.files && i.files[0];
              if (!f) return;
              var buf = new Uint8Array(await f.arrayBuffer());
              Module.FS.writeFile('/upload.ch8', buf);
              Module.ccall('app_on_open_rom_ems', null, ['number', 'string'], [$0, '/upload.ch8']);
              i.value = '';
          });
          Module._mc_input = i;
      }
      Module._mc_input.click();
    }, (uintptr_t)st);

#else
    SDL_ShowOpenFileDialog(app_showdialog_cb, st, st->window, NULL, 0, NULL, false);
#endif
}



static void app_reset(app_state* st, bool soft_reset) {
    //TODO call app_set_rom insead
    chip8_reinit(&st->chip, NULL, &st->chip_rom, soft_reset);
}

static void app_toggle_pause(app_state* st) {
    chip8_toggle_pause(&st->chip);
}

static void app_on_keydown(app_state* st, SDL_Scancode key, bool isdown) {
    if (isdown) {
        if (key == SDL_SCANCODE_O) {
            app_showdialog(st);
            return;
        }
        if (key == SDL_SCANCODE_TAB) {
            app_set_monodisplay_style(st, (monodisplay_style_kind)((st->chip_display_style + 1) % MONODISPLAY_STYLE_MAX));
            return;
        }
        if (key == SDL_SCANCODE_SPACE) {
            app_reset(st, true);
            return;
        }
        if (key == SDL_SCANCODE_P) {
            app_toggle_pause(st);
            return;
        }
    }
    const int mapper_sz = (int)COUNTOF_192F(chip_keymapper);
    for (int i = 0;i < mapper_sz;++i) {
        if (chip_keymapper[i] == key) {
            chip8_input_put_key(&st->chip, i, isdown);
        }
    }
}

static inline SDL_AppResult app_init_sdl3(app_state* st) {
    //TODO init joustick for android?
    if (!SDL_CreateWindowAndRenderer(APP_NAME, 800, 600, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY, &st->window, &st->renderer))
        return report(SDL_APP_FAILURE, "%s", SDL_GetError());

    UNUSED_192F(SDL_SetWindowMinimumSize(st->window, 200, 200));

    if (!SDL_SetRenderVSync(st->renderer, 1))
        return report(SDL_APP_FAILURE, "SDL_SetRenderVSync failed: %s", SDL_GetError());
    
    if (!SDL_SetRenderDrawBlendMode(st->renderer, SDL_BLENDMODE_BLEND))
        UNUSED_192F(report_sdl(0));

    return SDL_APP_CONTINUE;
}

static inline void app_uninit_sdl3(app_state* st) {
    if (!st) return;
    if (st->renderer) {
        SDL_DestroyRenderer(st->renderer);
        st->renderer = NULL;
    }
    if (st->window) {
        SDL_DestroyWindow(st->window);
        st->window = NULL;
    }
}

static inline SDL_AppResult app_init_sdl3_audio(app_state* st) {
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) return report_sdl(SDL_APP_FAILURE);
    wavegen_init(&st->chip_wave, WAVE_SQUARE, 44100, 440.0f, 0.02f);
    SDL_AudioSpec spec = wavegen_sdl3spec(&st->chip_wave);
    st->audio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, wavegen_sdl3callback, &st->chip_wave);
    if (!st->audio) { wavegen_free(&st->chip_wave); return report_sdl(SDL_APP_FAILURE); }
    SDL_PauseAudioStreamDevice(st->audio);
    return SDL_APP_CONTINUE;
}

static inline void app_uninit_sdl3_audio(app_state* st) {
    if (st->audio) {
        SDL_PauseAudioStreamDevice(st->audio);
        SDL_DestroyAudioStream(st->audio);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        st->audio = NULL;
        wavegen_free(&st->chip_wave);
    }
}

static inline SDL_AppResult app_init_res(app_state* st) {
    static const int mapper[] = {
        [RES_ICON_OPEN] = CHIP_ICONS_OPEN,
        [RES_ICON_PLAYPAUSE] = CHIP_ICONS_PLAYPAUSE,
        [RES_ICON_RESET] = CHIP_ICONS_RESET,
        [RES_ICON_VOLUME] = CHIP_ICONS_VOLUME,
        [RES_ICON_ROMINFO] = CHIP_ICONS_ROMINFO,
        [RES_ICON_ABOUT] = CHIP_ICONS_ABOUT,
        [RES_ICON_ROTATE] = CHIP_ICONS_ROTATE
    };
    for (int i = 0; i < COUNTOF_192F(mapper); ++i) {
        SDL_Texture* icon = utils_res_icon(st->renderer, chip_get_res(mapper[i]));
        if (!icon) return report(SDL_APP_FAILURE, "Unable to init resources");
        st->icons[i] = nk_image_ptr(icon);
    }
    return SDL_APP_CONTINUE;
}

static inline void app_uninit_res(app_state* st) {
    for (int i = 0;i < RES_ICON_MAX;++i) {
        SDL_DestroyTexture((SDL_Texture*)st->icons[i].handle.ptr);
        st->icons[i].handle.ptr = NULL;
    }
}
static inline SDL_AppResult app_init_nuklear(app_state* st) {
    struct nk_context* ctx = nk_sdl_init(st->window, st->renderer, nk_sdl_allocator());
    st->nk_ctx = ctx;
    if (!ctx) return report(SDL_APP_FAILURE, "nk_sdl_init failed");
    ui_info_window_init(&st->ui_rom_info, nk_rect(0, app_toolbox_h, 250, 300), false);

    ui_set_color_theme(ctx, UI_THEME_GREEN);
    ctx->style.button.image_padding = nk_vec2(0, 0);
    ctx->style.window.padding = nk_vec2(0, 0);
    ctx->style.combo.button_padding = nk_vec2(6.0f, 6.0f);
    return SDL_APP_CONTINUE;
}

static inline void app_uninit_nuklear(app_state* st) {
    ui_info_window_free(&st->ui_rom_info);
    nk_sdl_shutdown(st->nk_ctx);
}

static inline SDL_AppResult app_init_chip(app_state* st) {
    st->chip_rom = *app_get_default_rom();
    chip8_init(&st->chip, &chip_presets[CHIP_PRESET_OCTO_CHIP], &st->chip_rom);

    clockgen_init(&st->chip_time, chip_standard_freques, CHIP_DEVICE_MAX);
    st->chip_display_style = MONODISPLAY_STYLE_BLACKWHITE;
    monodisplay_renderer_init(&st->chip_display, monodisplay_styles[st->chip_display_style], (SDL_FRect) { 0, 0, 512, 512 }, CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT);
    
    app_set_rom(st, NULL, NULL, "", -1.0, NULL);
    app_set_monodisplay_style(st, MONODISPLAY_STYLE_BLACKWHITE);
    return SDL_APP_CONTINUE;
}

static inline void app_uninit_chip(app_state* st) {
    monodisplay_renderer_free(&st->chip_display);
    clockgen_free(&st->chip_time);
    chip_rom_free(&st->chip_rom);
    chip8_free(&st->chip);
}

static SDL_AppResult app_init(app_state* st) {
    srand((unsigned)time(NULL));
    SDL_AppResult ret;
    if ((ret = app_init_sdl3(st)) != SDL_APP_CONTINUE) return ret;
    st->sound_allow = (app_init_sdl3_audio(st) == SDL_APP_CONTINUE);
    if ((ret = app_init_res(st)) != SDL_APP_CONTINUE) return ret;
    if ((ret = app_init_nuklear(st)) != SDL_APP_CONTINUE) return ret;
    if ((ret = app_init_chip(st)) != SDL_APP_CONTINUE) return ret;
    st->last_time = SDL_GetPerformanceCounter();
    st->ui_about_shown = false;
    st->fps_counter = fps_counter_init();
    app_on_scale_size_changed(st);
    return SDL_APP_CONTINUE;
}
static SDL_AppResult app_event(app_state* st, SDL_Event* event) {
    switch (event->type) {
    case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED: app_on_scale_size_changed(st); return SDL_APP_CONTINUE;
    case SDL_EVENT_WINDOW_RESIZED: app_on_resize(st); return SDL_APP_CONTINUE;
    case SDL_EVENT_QUIT: return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_UP: 
    case SDL_EVENT_KEY_DOWN:
        app_on_keydown(st, event->key.scancode, event->type == SDL_EVENT_KEY_DOWN);
        break;
    }
    SDL_ConvertEventToRenderCoordinates(st->renderer, event);
    nk_sdl_handle_event(st->nk_ctx, event);
    return SDL_APP_CONTINUE;
}

static void app_on_chipvm_updated(app_state* st) {
    static bool exited_prev = false;
    const bool is_exited = st->chip.status == CHIP_STATUS_EXITED;
    if (!exited_prev && is_exited) {
        bool ok = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "ROM exited", "This ROM requested exit. You can restart the state or open another ROM", st->window);
        if (!ok) UNUSED_192F(report_sdl(0));
    }
    exited_prev = is_exited;
}


static int app_format_stats(app_state* st, char* buf, int len) {
    char fps[64];
    fps_counter_format(&st->fps_counter, fps, sizeof(fps));
    float dpi = SDL_GetWindowDisplayScale(st->window);
    return snprintf(buf, len,
        "%s %s\n"
        "Build: %s %.5s\n"
        "Created by %s\n"
        "%s\n"
        "DPI: %.4f\n"
        "Controls: 1-4, Q-R, A-F, Z-V\n"
        "DevDate: %s"
        ,
        APP_NAME, APP_VERSION_STRING_192F,
        APP_BUILD_DATE, APP_BUILD_TIME,
        APP_AUTHER,
        fps,
        dpi,
        APP_DEV_DATE);
}

static void app_frame_ui(app_state* st) {
    struct nk_context* ctx = st->nk_ctx;
    //int w, h;
    //SDL_GetCurrentRenderOutputSize(st->renderer, &w, &h);
    SDL_Rect vrect;
    SDL_GetRenderViewport(st->renderer, &vrect);
    int w = vrect.w;//, h = vrect.h;

    if (st->ui_about_shown) {
        char about_buf[1024];
        app_format_stats(st, about_buf, sizeof(about_buf));
        const float about_width = 200.0f;
        ui_utils_text_place(ctx, "About", nk_rect(w - about_width, app_toolbox_h, about_width, 0.0f), true, about_buf, -1);
    }
    if (nk_begin(ctx, "Toolbox", nk_rect(0, 0, w, app_toolbox_h), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER))
    {

        const float btn_w = app_toolbox_h;
        const float gap = 10.0f;
        const float bigw = 150.0f;

        //const float sizes[] = { app_toolbox_h, 30.0f, 100.f };
        //nk_layout_row(ctx, NK_STATIC, app_toolbox_h - 4, COUNTOF_192F(sizes), sizes);

        nk_layout_row_begin(ctx, NK_STATIC, app_toolbox_h-4, 12);

        nk_layout_row_push(ctx, btn_w);
        if (nk_button_image(ctx, st->icons[RES_ICON_OPEN])) {
            app_showdialog(st);
        }

        nk_layout_row_push(ctx, btn_w);

        bool is_running = chip_status_is_running(st->chip.status);
        if (ui_utils_selected_icon(ctx, st->icons[RES_ICON_PLAYPAUSE], is_running) ^ is_running)
            app_toggle_pause(st);

        nk_layout_row_push(ctx, btn_w);
        if (nk_button_image(ctx, st->icons[RES_ICON_RESET])) {
            app_reset(st, true);
        }

        nk_layout_row_push(ctx, btn_w);

        st->sound_allow = ui_utils_selected_icon(ctx, st->icons[RES_ICON_VOLUME], st->sound_allow) && st->audio;

        nk_layout_row_push(ctx, btn_w);
        if (nk_button_image(ctx, st->icons[RES_ICON_ROTATE])) {
            app_set_monodisplay_rotation(st, (st->chip_display.rotation + 1) % MONODISPLAY_ROTATION_MAX);
        }

        nk_layout_row_push(ctx, gap);
        nk_spacing(ctx, 1);


        nk_layout_row_push(ctx, btn_w);
        st->ui_rom_info.shown = !ui_utils_selected_icon(ctx, st->icons[RES_ICON_ROMINFO], !st->ui_rom_info.shown); 

        nk_layout_row_push(ctx, btn_w);
        st->ui_about_shown = !ui_utils_selected_icon(ctx, st->icons[RES_ICON_ABOUT], !st->ui_about_shown);

        nk_layout_row_push(ctx, gap);
        nk_spacing(ctx, 1);

        nk_layout_row_push(ctx, bigw);
        int freq = (int)st->chip_time.freq[CHIP_DEVICE_VM];
        nk_property_int(ctx, "CPU Hz:", 100, &freq, 3000, 10, 5);
        st->chip_time.freq[CHIP_DEVICE_VM] = freq;

        nk_layout_row_push(ctx, bigw);
        static const char* monodisplay_style_mapper[] = {
            [MONODISPLAY_STYLE_BLACKWHITE] = "Black and White",
            [MONODISPLAY_STYLE_REDNEON] = "Red Neon",
            [MONODISPLAY_STYLE_SIEMENS_ORRANGE] = "Orange Digital",
            [MONODISPLAY_STYLE_NOKIA_GREEN] = "Green Digital",
            [MONODISPLAY_STYLE_OLDMONITOR]="Old Monitor"};

        int monodisplay_style_ind = ui_utils_nk_combo(ctx, monodisplay_style_mapper, COUNTOF_192F(monodisplay_style_mapper), st->chip_display_style, COUNTOF_192F(monodisplay_style_mapper));
        if (monodisplay_style_ind!=st->chip_display_style) //Note. Be careful. This will reset the entire effect.
            app_set_monodisplay_style(st, (monodisplay_style_kind)monodisplay_style_ind);

        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
    
    //nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_color(nk_rgb(255, 0, 0)));
    ui_info_window_draw(&st->ui_rom_info, ctx);
    //nk_style_pop_style_item(ctx);
}

static SDL_AppResult app_frame(app_state* st) {
    Uint64 time_u64 = SDL_GetPerformanceCounter();
    const double delta = (time_u64 - st->last_time) / (double)SDL_GetPerformanceFrequency();
    st->last_time = time_u64;
    fps_counter_update(&st->fps_counter, delta);
    nk_input_end(st->nk_ctx);
    typedef void (*chip_update_device_fn)(chip8*);
    static const chip_update_device_fn update_devices[] = {
        [CHIP_DEVICE_VM] = chip8_step,
        [CHIP_DEVICE_VMTIMER] = chip8_timer_tick,
    };
    const int* update_counts = clockgen_update(&st->chip_time, delta);
    for (int i = 0;i < CHIP_DEVICE_MAX; ++i)
        for (int j = 0;j < update_counts[i];++j) {
            update_devices[i](&st->chip);
            if (i == CHIP_DEVICE_VMTIMER) app_switch_sound(st, chip8_needs_sound(&st->chip));
            if (i == CHIP_DEVICE_VM) app_on_chipvm_updated(st);
        }
    const struct nk_colorf bg = { 0.2f,0.2f,0.2f,1.0f };
    SDL_SetRenderDrawColorFloat(st->renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(st->renderer);
    monodisplay_renderer_draw(&st->chip_display, st->renderer, st->chip.video, CHIP8_DISPLAY_STRIDE);
    app_frame_ui(st);
    nk_sdl_render(st->nk_ctx, NK_ANTI_ALIASING_ON);
    nk_sdl_update_TextInput(st->nk_ctx);
    SDL_RenderPresent(st->renderer);
    nk_input_begin(st->nk_ctx);
    return SDL_APP_CONTINUE;
}

static void app_free(app_state* st) {
    if (!st) return;
    app_uninit_sdl3_audio(st);
    app_uninit_chip(st);
    app_uninit_res(st);
    app_uninit_nuklear(st);
    app_uninit_sdl3(st);
    memset(st, 0, sizeof * st);
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    return app_event((app_state*)appstate, event);
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    static app_state app = { 0 };
    *appstate = &app;
    return app_init(&app);
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    return app_frame((app_state*)appstate);
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    NK_UNUSED(result);
    app_free((app_state*)appstate);
}