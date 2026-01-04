#ifndef UI_H
#define UI_H
#include "all.h"

static struct nk_rect ui_utils_text_place(struct nk_context* ctx, const char* unique_name, struct nk_rect bounds, bool opac, const char* str, int len) {
    assert(ctx && !ctx->current && "should be called not in nk_begin");
    assert(bounds.h == 0.0f && "bounds.h must be 0, it will be calculated");
    struct nk_style* style = &ctx->style;

    if (len == -1) len = nk_strlen(str);

    if (!opac) {
        struct nk_style_item* back_col = &ctx->style.window.fixed_background;
        nk_style_push_style_item(ctx, back_col, nk_style_item_color(nk_rgba(0, 0, 0, 0)));
    }

    nk_style_push_vec2(ctx, &style->window.spacing, nk_vec2(0.0f, 0.0f));
    nk_style_push_vec2(ctx, &style->text.padding, nk_vec2(0.0f, 0.0f));
    nk_style_push_vec2(ctx, &style->window.padding, nk_vec2(0.0f, 0.0f));

    int lines = 1;
    for (int i = 0; i < len; ++i) lines += (str[i] == '\n');

    const float line_h = ctx->style.font->height + 2.0f;
    bounds.h = lines * line_h + 1.0f;

    if (nk_begin(ctx, unique_name, bounds, NK_WINDOW_NOT_INTERACTIVE | NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_dynamic(ctx, line_h, 1);
        for (int i = 0; i < len;) {
            int start = i;
            while (i < len && str[i] != '\n') i++;
            nk_text(ctx, str + start, i - start, NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_TOP);
            if (i < len && str[i] == '\n') i++;
        }
    }
    nk_end(ctx);
    nk_style_pop_vec2(ctx);
    nk_style_pop_vec2(ctx);
    nk_style_pop_vec2(ctx);
    if (!opac) nk_style_pop_style_item(ctx);
    return bounds;
}

static nk_bool ui_utils_selected_icon(struct nk_context* ctx, struct nk_image img, nk_bool value) {
	return nk_select_image_label(ctx, img, " ", NK_TEXT_ALIGN_LEFT, value);
}

static int ui_utils_nk_combo(struct nk_context* ctx, const char* const* items, int count, int selected, int max_lines) {
    const struct nk_style* style = &ctx->style;
    const float font_height = 1.3 * style->font->height;//gb_utils_deafult_height(ctx);//1.3*style->font->height;

    const float vpad = 2 * style->window.spacing.y + 2 * style->text.padding.y;

    const int lines = (max_lines < 0 || max_lines > count) ? count : max_lines;

    const float row_height = font_height + vpad;
    const float combo_height = row_height * lines;

    const float combo_width = nk_widget_width(ctx);
    struct nk_vec2 combo_size = nk_vec2(combo_width, combo_height);
    return nk_combo(ctx, items, count, selected, (int)font_height, combo_size);
}

typedef struct {
    struct nk_rect bounds;/*it's not actual bounds - this bounds will use for next show*/
    bool shown;
    struct nk_text_edit* _text_edit;
} ui_info_window;

static void ui_info_window_init(ui_info_window* st, struct nk_rect bounds, bool shown) {
    ui_info_window ret = {
        .bounds = bounds,
        .shown = shown,
        ._text_edit = halt_malloc(sizeof * ret._text_edit, NULL),
    };
    *st = ret;
    struct nk_allocator alloc = nk_sdl_allocator();
    nk_textedit_init(st->_text_edit, &alloc, 256);
    //nk_textedit_clear_state(st->_text_edit, NK_TEXT_EDIT_MULTI_LINE, 0);
    //st->_text_edit = NK_TEXT_EDIT_MODE_INSERT;
}


static void ui_info_window_set_text(ui_info_window* st, const char* text) {
    struct nk_str* str = &st->_text_edit->string;
    nk_str_clear(str);
    if (!text) return;
    nk_str_append_text_char(str, text, strlen(text));
}

static void ui_info_window_draw(ui_info_window* st, struct nk_context* ctx) {
    if (!st->shown) return;
    if (nk_begin(ctx, "Info", st->bounds, NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_TITLE | NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR)) {
        st->bounds = nk_window_get_bounds(ctx);
        struct nk_rect content = nk_window_get_content_region(ctx);
        const float padding = 5.0f;
        const float btn_h = 28.0f;

        const float text_h = utils_maxf(content.h - (btn_h + padding), 40.0);
        nk_layout_row_dynamic(ctx, text_h, 1);
        nk_edit_buffer(ctx, NK_EDIT_READ_ONLY | NK_EDIT_BOX, st->_text_edit, nk_filter_default);
        //nk_text_wrap(ctx, st->_text_edit->string.buffer.memory.ptr, st->_text_edit->string.buffer.memory.size);
        nk_layout_row_dynamic(ctx, btn_h, 2);
        if (nk_button_label(ctx, "Copy")) {
            utils_nk_clip_copy_all(&ctx->clip, st->_text_edit);
        }
        if (nk_button_label(ctx, "OK")) {
            st->shown = false;
        }
    }
    else {
        st->shown = false;
    }
    nk_end(ctx);
}

static void ui_info_window_free(ui_info_window* st) {
    if (!st) return;
    nk_textedit_free(st->_text_edit);
    free(st->_text_edit);
    memset(st, 0, sizeof * st);
}

typedef enum {
	//UI_THEME_BLACK, Not present even in nuklear repository(common/style.c)
	UI_THEME_WHITE,
	UI_THEME_RED,
	UI_THEME_BLUE,
	UI_THEME_DARK,
	UI_THEME_GREEN,
	UI_THEME_BROWN,
	UI_THEME_PURPLE,
	UI_THEME_DRACULA,
	UI_THEME_DEFAULT,
	UI_THEME_MAX
} ui_color_theme;
/*
This theme set was adapted from the CadZinho repository
https://github.com/zecruel/CadZinho/blob/opengl/src/gui.c
*/
static const struct nk_color ui_color_themes[][NK_COLOR_COUNT] = {
	[UI_THEME_WHITE] = {
		  [NK_COLOR_TEXT] = {35, 35, 35, 255},
		  [NK_COLOR_WINDOW] = {225, 225, 225, 255},
		  [NK_COLOR_HEADER] = {175, 175, 175, 255},
		  [NK_COLOR_BORDER] = {0, 0, 0, 255},
		  [NK_COLOR_BUTTON] = {200, 200, 200, 255},
		  [NK_COLOR_BUTTON_HOVER] = {170, 170, 170, 255},
		  [NK_COLOR_BUTTON_ACTIVE] = {160, 160, 160, 255},
		  [NK_COLOR_TOGGLE] = {150, 150, 150, 255},
		  [NK_COLOR_TOGGLE_HOVER] = {120, 120, 120, 255},
		  [NK_COLOR_TOGGLE_CURSOR] = {225, 225, 225, 255},
		  [NK_COLOR_SELECT] = {200, 200, 200, 255},
		  [NK_COLOR_SELECT_ACTIVE] = {225, 225, 225, 255},
		  [NK_COLOR_SLIDER] = {200, 200, 200, 255},
		  [NK_COLOR_SLIDER_CURSOR] = {80, 80, 80, 255},
		  [NK_COLOR_SLIDER_CURSOR_HOVER] = {70, 70, 70, 255},
		  [NK_COLOR_SLIDER_CURSOR_ACTIVE] = {60, 60, 60, 255},
		  [NK_COLOR_PROPERTY] = {225, 225, 225, 255},
		  [NK_COLOR_EDIT] = {175, 175, 175, 255},
		  [NK_COLOR_EDIT_CURSOR] = {0, 0, 0, 255},
		  [NK_COLOR_COMBO] = {225, 225, 225, 255},
		  [NK_COLOR_CHART] = {160, 160, 160, 255},
		  [NK_COLOR_CHART_COLOR] = {45, 45, 45, 255},
		  [NK_COLOR_CHART_COLOR_HIGHLIGHT] = {255, 0, 0, 255},
		  [NK_COLOR_SCROLLBAR] = {180, 180, 180, 255},
		  [NK_COLOR_SCROLLBAR_CURSOR] = {140, 140, 140, 255},
		  [NK_COLOR_SCROLLBAR_CURSOR_HOVER] = {150, 150, 150, 255},
		  [NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = {160, 160, 160, 255},
		  [NK_COLOR_TAB_HEADER] = {180, 180, 180, 255},
	},
	[UI_THEME_RED] = {
		[NK_COLOR_TEXT] = {190, 190, 190, 255},
		[NK_COLOR_WINDOW] = {30, 33, 40, 215},
		[NK_COLOR_HEADER] = {181, 45, 69, 220},
		[NK_COLOR_BORDER] = {51, 55, 67, 255},
		[NK_COLOR_BUTTON] = {181, 45, 69, 255},
		[NK_COLOR_BUTTON_HOVER] = {190, 50, 70, 255},
		[NK_COLOR_BUTTON_ACTIVE] = {195, 55, 75, 255},
		[NK_COLOR_TOGGLE] = {51, 55, 67, 255},
		[NK_COLOR_TOGGLE_HOVER] = {45, 60, 60, 255},
		[NK_COLOR_TOGGLE_CURSOR] = {181, 45, 69, 255},
		[NK_COLOR_SELECT] = {51, 55, 67, 255},
		[NK_COLOR_SELECT_ACTIVE] = {181, 45, 69, 255},
		[NK_COLOR_SLIDER] = {51, 55, 67, 255},
		[NK_COLOR_SLIDER_CURSOR] = {181, 45, 69, 255},
		[NK_COLOR_SLIDER_CURSOR_HOVER] = {186, 50, 74, 255},
		[NK_COLOR_SLIDER_CURSOR_ACTIVE] = {191, 55, 79, 255},
		[NK_COLOR_PROPERTY] = {51, 55, 67, 255},
		[NK_COLOR_EDIT] = {51, 55, 67, 225},
		[NK_COLOR_EDIT_CURSOR] = {190, 190, 190, 255},
		[NK_COLOR_COMBO] = {51, 55, 67, 255},
		[NK_COLOR_CHART] = {51, 55, 67, 255},
		[NK_COLOR_CHART_COLOR] = {170, 40, 60, 255},
		[NK_COLOR_CHART_COLOR_HIGHLIGHT] = {255, 0, 0, 255},
		[NK_COLOR_SCROLLBAR] = {30, 33, 40, 255},
		[NK_COLOR_SCROLLBAR_CURSOR] = {64, 84, 95, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = {70, 90, 100, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = {75, 95, 105, 255},
		[NK_COLOR_TAB_HEADER] = {181, 45, 69, 220},
	},
	[UI_THEME_BLUE] = {
		[NK_COLOR_TEXT] = {20, 20, 20, 255},
		[NK_COLOR_WINDOW] = {202, 212, 214, 215},
		[NK_COLOR_HEADER] = {137, 182, 224, 220},
		[NK_COLOR_BORDER] = {140, 159, 173, 255},
		[NK_COLOR_BUTTON] = {137, 182, 224, 255},
		[NK_COLOR_BUTTON_HOVER] = {142, 187, 229, 255},
		[NK_COLOR_BUTTON_ACTIVE] = {147, 192, 234, 255},
		[NK_COLOR_TOGGLE] = {177, 210, 210, 255},
		[NK_COLOR_TOGGLE_HOVER] = {182, 215, 215, 255},
		[NK_COLOR_TOGGLE_CURSOR] = {137, 182, 224, 255},
		[NK_COLOR_SELECT] = {177, 210, 210, 255},
		[NK_COLOR_SELECT_ACTIVE] = {137, 182, 224, 255},
		[NK_COLOR_SLIDER] = {177, 210, 210, 255},
		[NK_COLOR_SLIDER_CURSOR] = {137, 182, 224, 245},
		[NK_COLOR_SLIDER_CURSOR_HOVER] = {142, 188, 229, 255},
		[NK_COLOR_SLIDER_CURSOR_ACTIVE] = {147, 193, 234, 255},
		[NK_COLOR_PROPERTY] = {210, 210, 210, 255},
		[NK_COLOR_EDIT] = {210, 210, 210, 225},
		[NK_COLOR_EDIT_CURSOR] = {20, 20, 20, 255},
		[NK_COLOR_COMBO] = {210, 210, 210, 255},
		[NK_COLOR_CHART] = {210, 210, 210, 255},
		[NK_COLOR_CHART_COLOR] = {137, 182, 224, 255},
		[NK_COLOR_CHART_COLOR_HIGHLIGHT] = {255, 0, 0, 255},
		[NK_COLOR_SCROLLBAR] = {190, 200, 200, 255},
		[NK_COLOR_SCROLLBAR_CURSOR] = {64, 84, 95, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = {70, 90, 100, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = {75, 95, 105, 255},
		[NK_COLOR_TAB_HEADER] = {156, 193, 220, 255},
	},
	[UI_THEME_DARK] = {
		[NK_COLOR_TEXT] = {210, 210, 210, 255},
		[NK_COLOR_WINDOW] = {57, 67, 71, 215},
		[NK_COLOR_HEADER] = {51, 51, 56, 220},
		[NK_COLOR_BORDER] = {46, 46, 46, 255},
		[NK_COLOR_BUTTON] = {48, 83, 111, 255},
		[NK_COLOR_BUTTON_HOVER] = {58, 93, 121, 255},
		[NK_COLOR_BUTTON_ACTIVE] = {63, 98, 126, 255},
		[NK_COLOR_TOGGLE] = {50, 58, 61, 255},
		[NK_COLOR_TOGGLE_HOVER] = {45, 53, 56, 255},
		[NK_COLOR_TOGGLE_CURSOR] = {48, 83, 111, 255},
		[NK_COLOR_SELECT] = {57, 67, 61, 255},
		[NK_COLOR_SELECT_ACTIVE] = {48, 83, 111, 255},
		[NK_COLOR_SLIDER] = {50, 58, 61, 255},
		[NK_COLOR_SLIDER_CURSOR] = {48, 83, 111, 245},
		[NK_COLOR_SLIDER_CURSOR_HOVER] = {53, 88, 116, 255},
		[NK_COLOR_SLIDER_CURSOR_ACTIVE] = {58, 93, 121, 255},
		[NK_COLOR_PROPERTY] = {50, 58, 61, 255},
		[NK_COLOR_EDIT] = {50, 58, 61, 225},
		[NK_COLOR_EDIT_CURSOR] = {210, 210, 210, 255},
		[NK_COLOR_COMBO] = {50, 58, 61, 255},
		[NK_COLOR_CHART] = {50, 58, 61, 255},
		[NK_COLOR_CHART_COLOR] = {48, 83, 111, 255},
		[NK_COLOR_CHART_COLOR_HIGHLIGHT] = {255, 0, 0, 255},
		[NK_COLOR_SCROLLBAR] = {50, 58, 61, 255},
		[NK_COLOR_SCROLLBAR_CURSOR] = {48, 83, 111, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = {53, 88, 116, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = {58, 93, 121, 255},
		[NK_COLOR_TAB_HEADER] = {48, 83, 111, 255},
	},
	[UI_THEME_GREEN] = {
		[NK_COLOR_TEXT] = {244, 244, 244, 255},
		[NK_COLOR_WINDOW] = {57, 71, 58, 215},
		[NK_COLOR_HEADER] = {52, 57, 52, 220},
		[NK_COLOR_BORDER] = {46, 46, 46, 255},
		[NK_COLOR_BUTTON] = {48, 112, 54, 255},
		[NK_COLOR_BUTTON_HOVER] = {71, 161, 80, 255},
		[NK_COLOR_BUTTON_ACTIVE] = {89, 201, 100, 255},
		[NK_COLOR_TOGGLE] = {50, 61, 50, 255},
		[NK_COLOR_TOGGLE_HOVER] = {73, 84, 72, 255},
		[NK_COLOR_TOGGLE_CURSOR] = {48, 112, 54, 255},
		[NK_COLOR_SELECT] = {58, 67, 57, 255},
		[NK_COLOR_SELECT_ACTIVE] = {48, 112, 54, 255},
		[NK_COLOR_SLIDER] = {50, 61, 50, 255},
		[NK_COLOR_SLIDER_CURSOR] = {48, 112, 54, 245},
		[NK_COLOR_SLIDER_CURSOR_HOVER] = {59, 115, 53, 255},
		[NK_COLOR_SLIDER_CURSOR_ACTIVE] = {71, 161, 80, 255},
		[NK_COLOR_PROPERTY] = {50, 61, 50, 255},
		[NK_COLOR_EDIT] = {50, 61, 50, 225},
		[NK_COLOR_EDIT_CURSOR] = {210, 210, 210, 255},
		[NK_COLOR_COMBO] = {50, 61, 50, 255},
		[NK_COLOR_CHART] = {50, 61, 50, 255},
		[NK_COLOR_CHART_COLOR] = {48, 112, 54, 255},
		[NK_COLOR_CHART_COLOR_HIGHLIGHT] = {255, 0, 0, 255},
		[NK_COLOR_SCROLLBAR] = {50, 61, 50, 255},
		[NK_COLOR_SCROLLBAR_CURSOR] = {48, 112, 54, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = {59, 115, 53, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = {71, 161, 80, 255},
		[NK_COLOR_TAB_HEADER] = {48, 112, 54, 255},
	},
	[UI_THEME_BROWN] = {
		[NK_COLOR_TEXT] = {210, 210, 210, 255},
		[NK_COLOR_WINDOW] = {71, 67, 57, 215},
		[NK_COLOR_HEADER] = {56, 51, 51, 220},
		[NK_COLOR_BORDER] = {46, 46, 46, 255},
		[NK_COLOR_BUTTON] = {111, 83, 48, 255},
		[NK_COLOR_BUTTON_HOVER] = {121, 93, 58, 255},
		[NK_COLOR_BUTTON_ACTIVE] = {126, 98, 63, 255},
		[NK_COLOR_TOGGLE] = {61, 58, 50, 255},
		[NK_COLOR_TOGGLE_HOVER] = {56, 53, 45, 255},
		[NK_COLOR_TOGGLE_CURSOR] = {111, 83, 48, 255},
		[NK_COLOR_SELECT] = {61, 67, 57, 255},
		[NK_COLOR_SELECT_ACTIVE] = {111, 83, 48, 255},
		[NK_COLOR_SLIDER] = {61, 58, 50, 255},
		[NK_COLOR_SLIDER_CURSOR] = {111, 83, 48, 245},
		[NK_COLOR_SLIDER_CURSOR_HOVER] = {116, 88, 53, 255},
		[NK_COLOR_SLIDER_CURSOR_ACTIVE] = {121, 93, 58, 255},
		[NK_COLOR_PROPERTY] = {61, 58, 50, 255},
		[NK_COLOR_EDIT] = {61, 58, 50, 225},
		[NK_COLOR_EDIT_CURSOR] = {210, 210, 210, 255},
		[NK_COLOR_COMBO] = {61, 58, 50, 255},
		[NK_COLOR_CHART] = {61, 58, 50, 255},
		[NK_COLOR_CHART_COLOR] = {111, 83, 48, 255},
		[NK_COLOR_CHART_COLOR_HIGHLIGHT] = {0, 0, 255, 255},
		[NK_COLOR_SCROLLBAR] = {61, 58, 50, 255},
		[NK_COLOR_SCROLLBAR_CURSOR] = {111, 83, 48, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = {116, 88, 53, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = {121, 93, 58, 255},
		[NK_COLOR_TAB_HEADER] = {111, 83, 48, 255},
	},
	[UI_THEME_PURPLE] = {
		[NK_COLOR_TEXT] = {210, 210, 210, 255},
		[NK_COLOR_WINDOW] = {67, 57, 71, 215},
		[NK_COLOR_HEADER] = {51, 51, 56, 220},
		[NK_COLOR_BORDER] = {46, 46, 46, 255},
		[NK_COLOR_BUTTON] = {83, 48, 111, 255},
		[NK_COLOR_BUTTON_HOVER] = {93, 58, 121, 255},
		[NK_COLOR_BUTTON_ACTIVE] = {98, 63, 126, 255},
		[NK_COLOR_TOGGLE] = {58, 50, 61, 255},
		[NK_COLOR_TOGGLE_HOVER] = {53, 45, 56, 255},
		[NK_COLOR_TOGGLE_CURSOR] = {83, 48, 111, 255},
		[NK_COLOR_SELECT] = {67, 57, 61, 255},
		[NK_COLOR_SELECT_ACTIVE] = {83, 48, 111, 255},
		[NK_COLOR_SLIDER] = {58, 50, 61, 255},
		[NK_COLOR_SLIDER_CURSOR] = {83, 48, 111, 245},
		[NK_COLOR_SLIDER_CURSOR_HOVER] = {88, 53, 116, 255},
		[NK_COLOR_SLIDER_CURSOR_ACTIVE] = {93, 58, 121, 255},
		[NK_COLOR_PROPERTY] = {58, 50, 61, 255},
		[NK_COLOR_EDIT] = {58, 50, 61, 225},
		[NK_COLOR_EDIT_CURSOR] = {210, 210, 210, 255},
		[NK_COLOR_COMBO] = {58, 50, 61, 255},
		[NK_COLOR_CHART] = {58, 50, 61, 255},
		[NK_COLOR_CHART_COLOR] = {83, 48, 111, 255},
		[NK_COLOR_CHART_COLOR_HIGHLIGHT] = {0, 255, 0, 255},
		[NK_COLOR_SCROLLBAR] = {58, 50, 61, 255},
		[NK_COLOR_SCROLLBAR_CURSOR] = {83, 48, 111, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = {88, 53, 116, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = {93, 58, 121, 255},
		[NK_COLOR_TAB_HEADER] = {83, 48, 111, 255},
	},
	[UI_THEME_DRACULA] = {
		[NK_COLOR_TEXT] = {248, 248, 242, 255},
		[NK_COLOR_WINDOW] = {68, 71, 90, 255},
		[NK_COLOR_HEADER] = {40, 42, 54, 255},
		[NK_COLOR_BORDER] = {0, 0, 0, 255},
		[NK_COLOR_BUTTON] = {98, 114, 164, 255},
		[NK_COLOR_BUTTON_HOVER] = {255, 121, 198, 255},
		[NK_COLOR_BUTTON_ACTIVE] = {255, 85, 85, 255},
		[NK_COLOR_TOGGLE] = {40, 42, 54, 255},
		[NK_COLOR_TOGGLE_HOVER] = {255, 121, 198, 255},
		[NK_COLOR_TOGGLE_CURSOR] = {255, 85, 85, 255},
		[NK_COLOR_SELECT] = {98, 114, 164, 255},
		[NK_COLOR_SELECT_ACTIVE] = {255, 85, 85, 255},
		[NK_COLOR_SLIDER] = {40, 42, 54, 255},
		[NK_COLOR_SLIDER_CURSOR] = {98, 114, 164, 255},
		[NK_COLOR_SLIDER_CURSOR_HOVER] = {255, 121, 198, 255},
		[NK_COLOR_SLIDER_CURSOR_ACTIVE] = {255, 85, 85, 255},
		[NK_COLOR_PROPERTY] = {40, 42, 54, 255},
		[NK_COLOR_EDIT] = {40, 42, 54, 255},
		[NK_COLOR_EDIT_CURSOR] = {255, 184, 108, 255},
		[NK_COLOR_COMBO] = {40, 42, 54, 255},
		[NK_COLOR_CHART] = {40, 42, 54, 255},
		[NK_COLOR_CHART_COLOR] = {68, 71, 90, 255},
		[NK_COLOR_CHART_COLOR_HIGHLIGHT] = {255, 184, 108, 255},
		[NK_COLOR_SCROLLBAR] = {40, 42, 54, 255},
		[NK_COLOR_SCROLLBAR_CURSOR] = {98, 114, 164, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = {255, 121, 198, 255},
		[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = {255, 85, 85, 255},
		[NK_COLOR_TAB_HEADER] = {255, 85, 85, 255},
	}
};

static void ui_set_color_theme(struct nk_context* ctx, ui_color_theme theme) {
	assert(theme >= 0 && theme < UI_THEME_MAX);
	if (theme != UI_THEME_DEFAULT) {
		nk_style_from_table(ctx, ui_color_themes[theme]);
	}
	else {
		nk_style_default(ctx);
	}
}

#endif