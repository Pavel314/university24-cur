#ifndef CHIP8_H
#define CHIP8_H
#include "all.h"

/******************************************
 __________GENERAL CHIP FONT API___________
*******************************************/
typedef struct {
	const char* data;
	int padx, pady;
	int glyphw, glyphh;
	char glyphch;
} chip_font_info;

static int chip_font_info_glyph_count(const chip_font_info* font) {
	int glyph_len = (font->glyphw + font->padx) * (font->glyphh + 2 * font->pady);
	int unbordered_len = (int)strlen(font->data) - (font->glyphh + 2 * font->pady);
	assert((unbordered_len % glyph_len) == 0);
	return unbordered_len / glyph_len;
}

static size_t chip_font_info_export_to_memory(const chip_font_info* font, void* out) {
	unsigned char* out_bytes = (unsigned char*)out;
	int w = font->glyphw, h = font->glyphh;
	int padx = font->padx, pady = font->pady;
	const int glyph_count = chip_font_info_glyph_count(font);
	size_t wr_ind = 0;
	for (int i = 0;i < glyph_count;++i)
	{
		for (int y = 0;y < font->glyphh;++y) {
			int byte = 0;
			for (int x = 0;x < font->glyphw;++x) {
				int offx = i * (w + padx) + padx + x;
				int offy = 0 * (h + pady) + pady + y;
				char c = font->data[offx + offy * (glyph_count * (w + padx) + padx)];
				byte |= ((int)(c == font->glyphch)) << (7 - (x % 8));
				if (((x + 1) % 8) == 0 || (x == w - 1)) {
					if (out_bytes) out_bytes[wr_ind] = (unsigned char)byte;
					++wr_ind;
				}
			}
		}
	}
	return wr_ind;
}

static const char CHIP8_STANDARD_FONT_DATA[] =
"-------------------------------------------------------------------------------------------------------------------------------------------------"
"|xxxx....|..x.....|xxxx....|xxxx....|x..x....|xxxx....|xxxx....|xxxx....|xxxx....|xxxx....|xxxx....|xxx.....|xxxx....|xxx.....|xxxx....|xxxx....|"
"|x..x....|.xx.....|...x....|...x....|x..x....|x.......|x.......|...x....|x..x....|x..x....|x..x....|x..x....|x.......|x..x....|x.......|x.......|"
"|x..x....|..x.....|xxxx....|xxxx....|xxxx....|xxxx....|xxxx....|..x.....|xxxx....|xxxx....|xxxx....|xxx.....|x.......|x..x....|xxxx....|xxxx....|"
"|x..x....|..x.....|x.......|...x....|...x....|...x....|x..x....|.x......|x..x....|...x....|x..x....|x..x....|x.......|x..x....|x.......|x.......|"
"|xxxx....|.xxx....|xxxx....|xxxx....|...x....|xxxx....|xxxx....|.x......|xxxx....|xxxx....|x..x....|xxx.....|xxxx....|xxx.....|xxxx....|x.......|"
"-------------------------------------------------------------------------------------------------------------------------------------------------"
;

static const char SCHIP_STANDARD_FONT_DATA[] =
"-------------------------------------------------------------------------------------------------------------------------------------------------"
"|xxxxxxxx|...xx...|xxxxxxxx|xxxxxxxx|xx....xx|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxxxx|.xxxxxx.|xxxxxx..|..xxxx..|xxxxxx..|xxxxxxxx|xxxxxxxx|"
"|xxxxxxxx|.xxxx...|xxxxxxxx|xxxxxxxx|xx....xx|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxx..|xxxxxxxx|xxxxxxx.|xxxxxxxx|xxxxxxxx|"
"|xx....xx|.xxxx...|......xx|......xx|xx....xx|xx......|xx......|......xx|xx....xx|xx....xx|xx....xx|xx....xx|xx....xx|xx....xx|xx......|xx......|"
"|xx....xx|...xx...|......xx|......xx|xx....xx|xx......|xx......|......xx|xx....xx|xx....xx|xx....xx|xx....xx|xx......|xx....xx|xx......|xx......|"
"|xx....xx|...xx...|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxxxx|.....xx.|xxxxxxxx|xxxxxxxx|xx....xx|xxxxxx..|xx......|xx....xx|xxxxxxxx|xxxxxxxx|"
"|xx....xx|...xx...|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxxxx|....xx..|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxx..|xx......|xx....xx|xxxxxxxx|xxxxxxxx|"
"|xx....xx|...xx...|xx......|......xx|......xx|......xx|xx....xx|...xx...|xx....xx|......xx|xxxxxxxx|xx....xx|xx......|xx....xx|xx......|xx......|"
"|xx....xx|...xx...|xx......|......xx|......xx|......xx|xx....xx|...xx...|xx....xx|......xx|xx....xx|xx....xx|xx....xx|xx....xx|xx......|xx......|"
"|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxxxx|......xx|xxxxxxxx|xxxxxxxx|...xx...|xxxxxxxx|xxxxxxxx|xx....xx|xxxxxx..|xxxxxxxx|xxxxxxx.|xxxxxxxx|xx......|"
"|xxxxxxxx|xxxxxxxx|xxxxxxxx|xxxxxxxx|......xx|xxxxxxxx|xxxxxxxx|...xx...|xxxxxxxx|xxxxxxxx|xx....xx|xxxxxx..|..xxxx..|xxxxxx..|xxxxxxxx|xx......|"
"-------------------------------------------------------------------------------------------------------------------------------------------------";

typedef enum {
	CHIP_FONT_STANDARD_CHIP8,
	CHIP_FONT_STANDARD_SCHIP,
	CHIP_FONT_MAX
} chip_font_kind;

static const chip_font_info chip_fonts[CHIP_FONT_MAX] = {
	[CHIP_FONT_STANDARD_CHIP8] = {CHIP8_STANDARD_FONT_DATA, 1, 1, 8, 5, 'x'},
	[CHIP_FONT_STANDARD_SCHIP] = {SCHIP_STANDARD_FONT_DATA, 1, 1, 8, 10, 'x'},
};

/*END OF GENERAL CHIP FONT API*/

/******************************************
 __________GENERAL CHIP ROM API____________
*******************************************/
typedef struct {
	unsigned char* mem;
	size_t size;
	free_fn _free_fn;
	//TODO maybe store reccomened cycles per frames
} chip_rom;

static chip_rom chip_rom_init(unsigned char* mem, size_t size, free_fn free_fn) {
	assert(mem && size > 0);
	chip_rom ret = { mem, size, free_fn };
	return ret;
}

static bool chip_rom_from_file(chip_rom* out, const char* path, size_t size_limit) {
	if (!path) return false;
	FILE* f = fopen(path, "rb");
	if (!f) return false;
	fseek(f, 0, SEEK_END);
	const long size = ftell(f);
	if (size <= 0 || (size_limit>0 && size>size_limit)) {
		fclose(f);
		return false;
	}
	fseek(f, 0, SEEK_SET);
	unsigned char* buffer = (unsigned char*)malloc(size);
	if (!buffer) {
		fclose(f);
		return false;
	}
	size_t read_bytes = fread(buffer, 1, size, f);
	fclose(f);
	if (read_bytes != (size_t)size) {
		free(buffer);
		return false;
	}
	*out = chip_rom_init(buffer, size, free);
	return true;
}

static void chip_rom_free(chip_rom* st) {
	if (!st || !st->mem) return;
	if (st->_free_fn) st->_free_fn(st->mem);
	memset(st, 0, sizeof * st);
}

/*END OF GENERAL CHIP ROM API*/

/******************************************
 ____________CHIP CONFIG API_______________
*******************************************/
typedef enum {
	CHIP_Q__8XY6_8XYE_SHIFT_VY =				1 << 0,
	CHIP_Q__FX55_FX65_INC_I_BY_X =				1 << 1,
	CHIP_Q__FX55_FX65_INC_I_BY_1 =				1 << 2,
	CHIP_Q__FX55_FX65_INC_I_BY_X1 =				CHIP_Q__FX55_FX65_INC_I_BY_X | CHIP_Q__FX55_FX65_INC_I_BY_1,
	CHIP_Q__FX1E_VF_OVERFLOW =					1 << 3, /*https://github.com/Chromatophore/HP48-Superchip/issues/2 */
	CHIP_Q__BXNN_HIGH_NIBBLE_JUMP =				1 << 4,
	CHIP_Q__DXYN_LORES_SPRITES_WRAP_ON =		1 << 5, /*without this flag, clip logic will be use*/
	CHIP_Q__DXYN_HIRES_SPRITES_WRAP_ON =	    1 << 6, /*without this flag, clip logic will be use*/
	CHIP_Q__DXYN_SPRITES_WRAP_ON =				CHIP_Q__DXYN_LORES_SPRITES_WRAP_ON | CHIP_Q__DXYN_HIRES_SPRITES_WRAP_ON,
	CHIP_Q__8XY1_8XY2_8XY3_LOGIC_OP_RESET_VF =	1 << 7,
	CHIP_Q__DXYN_ALLOW_DXY0 =					1 << 8,
	CHIP_Q__DXYN_HIRES_COLLISION_COUNT_VF =		1 << 9, /*HP48-accurate quirks*/
	CHIP_Q__DXYN_HIRES_COLLISION_BOTTOM_VF =	1 << 10, /*HP48-accurate quirks*/
	/*TODO HIRES_VBLANK, LORES_VBLANK*/
} CHIP_QUIRKS;

typedef enum {
	CHIP_F__EXIT =				1 << 0, /*bool halted; 00FD -> halted = true; */
	CHIP_F__RPL =				1 << 1, /*uint8_t rpl[8]; FX75 - LD R, Vx; FX85 - LD Vx, R*/
	CHIP_F__HIRES_FONT =		1 << 2, /*FX30 [in hires]*/
	CHIP_F__DISPLAY_SWAPPING =	1 << 3, /*00FE - LOW, 00FF - HIGH*/
	CHIP_F__DISPLAY_SCROLLING = 1 << 4, /*In both modes(lores/hires) 0x00CN - SCD, 0x00FB - SCR, 0x00FC - SCL*/
	CHIP_F_SCHIP_1_1 = CHIP_F__EXIT | CHIP_F__RPL | CHIP_F__HIRES_FONT | CHIP_F__DISPLAY_SWAPPING | CHIP_F__DISPLAY_SCROLLING
} CHIP_FEATURES;

typedef enum {
	CHIP_B__MEMORY_RANDOMIZATION =	1 << 0, /*memory will be inited in anycase, it can be zeroing or randomization. Only The Binding of COSMAC need uncleared random memory*/
	CHIP_B__VIDEO_CLEAR_ON_SWAP =	1 << 1, /*video memory will be zeroing after display swapping*/
} CHIP_BEHAVIOUR;

typedef int(*chip_rnd_fn)(void* userdata);
static int chip_c89_rand(void* userdata) {
	UNUSED_192F(userdata);
	return rand();
}

typedef struct {
	uint_least32_t quirks;
	uint_least32_t features;
	uint_least32_t behaviour;
	const chip_font_info* lores_font; /*standard 4x5 chip8 font*/
	const chip_font_info* hires_font; /*it will be used only if CHIP_F__HIRES_FONT is present*/
	chip_rnd_fn rnd_fn; void* rnd_userdata;
} chip_config;

/*Validation is part of the interpreter, here we will only provide a few convenient methods*/
static bool chip_config_validate_font(const chip_font_info* font, int w, int h, int min_glyph_cnt, int max_glyph_cnt) {
	assert(font);
	if (font->glyphw != w || font->glyphh != h) return false;
	const int glyphs = chip_font_info_glyph_count(font);
	if (glyphs<min_glyph_cnt || glyphs>max_glyph_cnt) return false;
	return true;
}

static void chip_config_normalize(chip_config* st) {
	assert(st);
	if (!st->lores_font) st->lores_font = &chip_fonts[CHIP_FONT_STANDARD_CHIP8];
	if (!st->hires_font) st->hires_font = &chip_fonts[CHIP_FONT_STANDARD_SCHIP];
	if (!st->rnd_fn) { st->rnd_fn = chip_c89_rand; }
}
/*END OF CHIP CONFIG API*/

/******************************************
 ____________CHIP PRESETS API______________
*******************************************/
typedef enum {
	//DataBase Platforms. Must match to the platform's description from https://github.com/chip-8/chip-8-database/blob/master/database/platforms.json
	CHIP_PRESET_ORIGINAL_CHIP8,
	CHIP_PRESET_HYBRID_CHIP8,
	CHIP_PRESET_MODERN_CHIP8,
	CHIP_PRESET_CHIP8X,
	CHIP_PRESET_CHIP48,
	CHIP_PRESET_SCHIP_1_0,
	CHIP_PRESET_SCHIP,
	//Custom platforms
	CHIP_PRESET_SCHIP_WRAP,
	CHIP_PRESET_OCTO_CHIP,
	CHIP_PRESET_SPACEFIGHT2091_CHIP, /*This solves the drawing problem, but the game may still hang*/
	CHIP_PRESET_MAX,
} chip_preset_kind;


static const chip_config chip2_presets[CHIP_PRESET_MAX] = {
	[CHIP_PRESET_SCHIP_WRAP] = {
		.quirks = CHIP_Q__DXYN_SPRITES_WRAP_ON | CHIP_Q__DXYN_ALLOW_DXY0,
		.features = CHIP_F_SCHIP_1_1,
		.behaviour = CHIP_B__MEMORY_RANDOMIZATION,
	},
	[CHIP_PRESET_OCTO_CHIP] = {
		.quirks = CHIP_Q__8XY6_8XYE_SHIFT_VY | CHIP_Q__FX55_FX65_INC_I_BY_X1 | CHIP_Q__DXYN_SPRITES_WRAP_ON | CHIP_Q__DXYN_ALLOW_DXY0,
		.features = CHIP_F_SCHIP_1_1,
		.behaviour = CHIP_B__MEMORY_RANDOMIZATION,
	},
	[CHIP_PRESET_SPACEFIGHT2091_CHIP] = {
		.quirks = CHIP_Q__FX1E_VF_OVERFLOW | CHIP_Q__DXYN_ALLOW_DXY0,
		.features = CHIP_F_SCHIP_1_1,
		.behaviour = CHIP_B__MEMORY_RANDOMIZATION,
	},
	/*********************************************/
	[CHIP_PRESET_ORIGINAL_CHIP8] = {
		.quirks = CHIP_Q__8XY6_8XYE_SHIFT_VY | CHIP_Q__FX55_FX65_INC_I_BY_X1 | CHIP_Q__8XY1_8XY2_8XY3_LOGIC_OP_RESET_VF,
		.features = 0,
		.behaviour = CHIP_B__MEMORY_RANDOMIZATION,
	},
	[CHIP_PRESET_HYBRID_CHIP8] = {
		.quirks = CHIP_Q__8XY6_8XYE_SHIFT_VY | CHIP_Q__FX55_FX65_INC_I_BY_X1 | CHIP_Q__8XY1_8XY2_8XY3_LOGIC_OP_RESET_VF,
		.features = 0,
		.behaviour = CHIP_B__MEMORY_RANDOMIZATION,
	},
	[CHIP_PRESET_MODERN_CHIP8] = {
		.quirks = CHIP_Q__8XY6_8XYE_SHIFT_VY | CHIP_Q__FX55_FX65_INC_I_BY_X1,
		.features = 0,
		.behaviour = 0,
	},
	[CHIP_PRESET_CHIP8X] = {
		.quirks = CHIP_Q__8XY6_8XYE_SHIFT_VY | CHIP_Q__FX55_FX65_INC_I_BY_X1 | CHIP_Q__8XY1_8XY2_8XY3_LOGIC_OP_RESET_VF,
		.features = 0,
		.behaviour = CHIP_B__MEMORY_RANDOMIZATION,
	},
	[CHIP_PRESET_CHIP48] = {
		.quirks = CHIP_Q__FX55_FX65_INC_I_BY_X | CHIP_Q__BXNN_HIGH_NIBBLE_JUMP,
		.features = 0,
		.behaviour = CHIP_B__MEMORY_RANDOMIZATION,
	},
	[CHIP_PRESET_SCHIP_1_0] = {
		.quirks = CHIP_Q__FX55_FX65_INC_I_BY_X | CHIP_Q__BXNN_HIGH_NIBBLE_JUMP /*my part*/ | CHIP_Q__DXYN_ALLOW_DXY0,
		.features = CHIP_F__EXIT | CHIP_F__RPL | CHIP_F__DISPLAY_SWAPPING,
		.behaviour = CHIP_B__MEMORY_RANDOMIZATION,
	},
	[CHIP_PRESET_SCHIP] = {
		.quirks = CHIP_Q__BXNN_HIGH_NIBBLE_JUMP /*my part*/ | CHIP_Q__DXYN_ALLOW_DXY0,
		.features = CHIP_F_SCHIP_1_1,
		.behaviour = CHIP_B__MEMORY_RANDOMIZATION | CHIP_B__VIDEO_CLEAR_ON_SWAP,
	},
};


/*END OF CHIP PRESETS API*/

/******************************************
 ________________CHIP8 API_________________
*******************************************/
typedef enum {
	CHIP8_OPCODE_SIZE = 2,
	CHIP8_KEYS = 16,
	CHIP8_REGISTERS = 16,
	CHIP8_RPL_REGISTERS = 8, /*it will be used only if CHIP_F__RPL is present*/

	CHIP8_DISPLAY_WIDTH = 128, /*We do not use the HI prefix because it determines the size of the video buffer.*/
	CHIP8_DISPLAY_HEIGHT = 64,
	CHIP8_DISPLAY_STRIDE = CHIP8_DISPLAY_WIDTH,
	CHIP8_DISPLAY_SIZE = CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT,
	CHIP8_DISPLAY_LOWIDTH = 64,
	CHIP8_DISPLAY_LOHEIGHT = 32,
	CHIP8_DISPLAY_LOSIZE = CHIP8_DISPLAY_LOWIDTH * CHIP8_DISPLAY_LOHEIGHT,

	CHIP8_LOFONT_HEIGHT = 5,
	CHIP8_LOFONT_SIZE = 80,
	CHIP8_HIFONT_HEIGHT = 10,
	CHIP8_LOSPRITE_WIDTH = 8,
	CHIP8_HISPRITE_WIDTH = 16,
	CHIP8_LOFONT_BASE = 0x50,
	CHIP8_HIFONT_BASE = CHIP8_LOFONT_BASE + CHIP8_LOFONT_SIZE,
	CHIP8_BASE_ROM = 0x200,
	CHIP_INTERNAL_STORAGE_SIZE = CHIP8_BASE_ROM,

	CHIP8_STACK_FRAMES = 16,
	CHIP8_MEMORY = 4096,
	CHIP8_MAX_ROM_SIZE = CHIP8_MEMORY - CHIP_INTERNAL_STORAGE_SIZE,
} chip8_parameters;

#define CHIP8_UNPACK_X_Y_Z(code, x_name, y_name, z_name) \
	const uint8_t x_name=(uint8_t)((code & 0x0F00) >> 8); \
	const uint8_t y_name=(uint8_t)((code & 0x00F0) >> 4); \
	const uint8_t z_name=(uint8_t)((code & 0x000F) >> 0);

#define CHIP8_UNPACK_X_YZ(code, x_name, yz_name) \
	const uint8_t x_name=(uint8_t)((code & 0x0F00) >> 8); \
	const uint8_t yz_name=(uint8_t)((code & 0x00FF) >> 0);

#define CHIP8_UNPACK_XYZ(code, xyz_name) \
	const uint16_t xyz_name=(uint16_t)((code & 0x0FFF));

typedef enum {
	CHIP_STATUS_RUNNING = 0, //Denote that the VM extracts and run the opcodes
	CHIP_STATUS_EXITED,  //00FD was reached(only if CHIP_F__EXIT present)
	CHIP_STATUS_PAUSED, //user pause
	//TODO prgram done[opcodes array is ended]
	//CHIP_RUN_STATUS_WAIT_KEY,      // Fx0A
	//CHIP_STATUS_ERROR          // invalid opcode, stack error, etc
} chip_status;

static bool chip_status_is_good(chip_status st) {
	return (st == CHIP_STATUS_RUNNING) || (st == CHIP_STATUS_PAUSED);
}
static bool chip_status_is_running(chip_status st) {
	return st == CHIP_STATUS_RUNNING;
}

typedef struct {
	uint8_t regs[CHIP8_REGISTERS];
	uint8_t mem[CHIP8_MEMORY];
	uint16_t index;
	uint16_t pc;
	uint16_t stack[CHIP8_STACK_FRAMES];
	uint8_t sp;
	uint8_t delay_tim, sound_tim;
	bool keys[CHIP8_KEYS];
	bool video[CHIP8_DISPLAY_SIZE];
	bool hires;
	uint8_t rpl[CHIP8_RPL_REGISTERS];
	chip_status status;
	chip_config config;
	//todo error callback(unknown instruction and innocorrect instruction callback)
} chip8;

static bool chip8_validate_config(const chip_config* conf) {
	assert(conf);
	if (!conf->lores_font) return false;
	if (!chip_config_validate_font(conf->lores_font, 8, 5, 16, 16)) return false;
	if (conf->features & CHIP_F__HIRES_FONT) {
		if (!conf->hires_font) return false;
		if (!chip_config_validate_font(conf->hires_font, 8, 10, 10, 16)) return false;
	}
	if (!conf->rnd_fn) return false;
	return true;
}

/******************************************
 ~~~~CHIP8 BUILT-IN DISAPLY API SECTION~~~~
*******************************************/
//Note. All work with video memory should be done through these functions only
static void chip8__display_clear(chip8* st) {
	memset(st->video, false, sizeof(st->video));
}
static void chip8__display_init(chip8* st, bool hires) {
	st->hires = hires;
	chip8__display_clear(st);
}

//state-free function
static void chip8__display_loind_2_hiind(int x, int y, int* arr4) {
	const int stride = CHIP8_DISPLAY_STRIDE;
	x = x * 2; y = y * 2;
	arr4[0] = utils_xy2ind(x, y, stride);
	arr4[1] = utils_xy2ind(x + 1, y, stride);
	arr4[2] = utils_xy2ind(x + 1, y + 1, stride);
	arr4[3] = utils_xy2ind(x, y + 1, stride);
}

static void chip8__display_swap(chip8* st, bool hires) {
	if (st->hires == hires) return;
	if (st->config.behaviour & CHIP_B__VIDEO_CLEAR_ON_SWAP) {
		chip8__display_clear(st);
		st->hires = hires;
		return;
	}
	if (!st->hires && hires) { st->hires = true; return; }
	
	st->hires = false;
	bool* video = st->video;
	for (int y = 0; y < CHIP8_DISPLAY_LOHEIGHT; ++y)
		for (int x = 0; x < CHIP8_DISPLAY_LOWIDTH; ++x) {
			int inds[4];
			chip8__display_loind_2_hiind(x, y, inds);
			bool haspx = video[inds[0]] || video[inds[1]] || video[inds[2]] || video[inds[3]];
			video[inds[0]] = video[inds[1]] = video[inds[2]] = video[inds[3]] = haspx;
		}
}

//in lores x y should be [0..63], [0..31]
static void chip8__display_set_px(chip8* st, int x, int y, bool v) {
	if (st->hires) {
		st->video[utils_xy2ind(x, y, CHIP8_DISPLAY_STRIDE)] = v;
	}
	else {
		bool* video = st->video;
		int inds[4];
		chip8__display_loind_2_hiind(x, y, inds);
		video[inds[0]] = video[inds[1]] = video[inds[2]] = video[inds[3]] = v;
	}
}

static bool chip8__display_get_px(chip8* st, int x, int y) {
	if (st->hires)
		return st->video[utils_xy2ind(x, y, CHIP8_DISPLAY_STRIDE)];
	else
		return st->video[utils_xy2ind(x*2, y*2, CHIP8_DISPLAY_STRIDE)];
}


static void chip8__display_scroll_lr(chip8* st, bool left_to_right) {
	const unsigned w = CHIP8_DISPLAY_WIDTH;
	const unsigned h = CHIP8_DISPLAY_HEIGHT;
	const unsigned scrstep = 4;
	const size_t row_bytes = (w - scrstep) * sizeof(bool);
	for (unsigned y = 0; y < h; ++y) {
		bool* row = &st->video[y*w];
		if (left_to_right) {
			memmove(row+scrstep, row, row_bytes);
			memset(row, false, scrstep * sizeof(bool));
		}
		else {
			memmove(row, row + scrstep, row_bytes);
			memset(row + w - scrstep, false, scrstep * sizeof(bool));
		}
	}
}

static void chip8__display_scroll_down(chip8* st, unsigned n) {
	const unsigned w = CHIP8_DISPLAY_WIDTH;
	const unsigned h = CHIP8_DISPLAY_HEIGHT;
	const size_t row_bytes = w * sizeof(bool);
	n = n < h ? n : h;
	for (unsigned y = h; y-- > n; )
		memmove(&st->video[y * w], &st->video[(y - n) * w], row_bytes);
	memset(st->video, false, n * row_bytes);
}

static void chip8_get_display_wh(chip8* st, int* w, int* h) {
	if (w) *w = st->hires ? CHIP8_DISPLAY_WIDTH : CHIP8_DISPLAY_LOWIDTH;
	if (h) *h = st->hires ? CHIP8_DISPLAY_HEIGHT : CHIP8_DISPLAY_LOHEIGHT;
}
/*END OF ~CHIP8 BUILT-IN DISAPLY API~*/




static void chip8__write_fonts(chip8* st, const chip_font_info* lores_font, const chip_font_info* hires_font) {
	if (lores_font)
		assert(chip_font_info_export_to_memory(lores_font, st->mem + CHIP8_LOFONT_BASE) == CHIP8_LOFONT_SIZE);
	if (hires_font) {
		assert(CHIP8_HIFONT_BASE >= CHIP8_LOFONT_BASE + CHIP8_LOFONT_SIZE);
		chip_font_info_export_to_memory(hires_font, st->mem + CHIP8_HIFONT_BASE);
	}
}

static void chip8__write_rom(chip8* st, const chip_rom* rom) {
	assert(rom);
	const unsigned rom_max_size = CHIP8_MAX_ROM_SIZE;
	halt_assert(rom->size <= rom_max_size, "Rom too large, max size is %u bytes", rom_max_size);
	memcpy(st->mem + CHIP8_BASE_ROM, rom->mem, rom->size * sizeof(rom->mem[0]));
}

static void chip8__clear_members(chip8* st, const chip_config* conf) {
	memset(st->regs, 0, sizeof st->regs);
	memset(st->mem, 0, sizeof st->mem);
	//TODO memory randomization
	memset(st->keys, false, sizeof st->keys);
	memset(st->rpl, 0, sizeof st->rpl);/*Note. RPL is reset here, but it may be explicitly set again*/
	chip8__display_clear(st);
}

static void chip8_init(chip8* st, const chip_config* config, const chip_rom* rom) {
	halt_assert(config!=NULL, "config was null");
	halt_assert(rom != NULL, "rom was null");
	chip_config cfg = *config;
	chip_config_normalize(&cfg);
	halt_assert(chip8_validate_config(&cfg), "config was not validated");
	memset(st, 0, sizeof * st);
	st->pc = CHIP8_BASE_ROM;
	st->config = cfg;
	st->status = CHIP_STATUS_RUNNING;
	chip8__display_init(st, false);
	chip8__clear_members(st, &cfg);
	chip8__write_fonts(st, cfg.lores_font, cfg.hires_font);
	chip8__write_rom(st, rom);
}


static void chip8_free(chip8* st) {
	if (!st) return;
	memset(st, 0, sizeof * st);
}

static void chip8_reinit(chip8* st, const chip_config* config, const chip_rom* rom, bool soft_rest) {
	uint8_t saved_rpl[CHIP8_RPL_REGISTERS];
	chip_config cfg = config ? *config : st->config;
	const bool keep_rpl = soft_rest && (st->config.features & CHIP_F__RPL) && (cfg.features & CHIP_F__RPL);
	if (keep_rpl) memcpy(saved_rpl, st->rpl, sizeof(saved_rpl));
	chip8_free(st);
	chip8_init(st, &cfg, rom);
	if (keep_rpl) memcpy(st->rpl, saved_rpl, sizeof(saved_rpl));
}

static bool chip8_needs_sound(chip8* st) {return chip_status_is_running(st->status) && st->sound_tim > 0;}
static void chip8_set_status(chip8* st, chip_status status) { st->status = status; }
static void chip8_toggle_pause(chip8* st) {
	if (!chip_status_is_good(st->status)) return;
	chip_status stat = chip_status_is_running(st->status) ? CHIP_STATUS_PAUSED : CHIP_STATUS_RUNNING;
	chip8_set_status(st, stat);
}

static int chip8_input_get_key(chip8* st) {
	for (int i = 0;i < CHIP8_KEYS;++i) if (st->keys[i]) return i;
	return -1;
}
static void chip8_input_put_key(chip8* st, int key, bool press) {
	halt_assert(key >= 0 && key < CHIP8_KEYS, "Unexpected key: %d", key);
	st->keys[key] = press;
}

/******************************************
 ~~~~~~~~CHIP8 INSTUCTIONS SECTION~~~~~~~~
*******************************************/

static void chip8__0xxx(chip8* st, uint16_t code) {
	/*0x00CN - SCD, 0x00FB - SCR, 0x00FC - SCL*/
	const uint_least32_t feats = st->config.features;
	switch (code) {
	case 0x00E0: chip8__display_clear(st); break;
	case 0x00EE: 
		if (st->sp == 0) {
			//Some programs use this as an exit. But I'm not sure. Currently, it test for the By the moon
			chip8_set_status(st, CHIP_STATUS_EXITED);
			break;
		}
		//halt_assert(st->sp > 0, "Stack corrupted");
		st->pc = st->stack[--st->sp]; break; //linked to 2NNN
	case 0x00FB:
	case 0x00FC: {
		if (feats & CHIP_F__DISPLAY_SCROLLING) chip8__display_scroll_lr(st, code == 0x00FB);
		else halt_assert(0, "Unknown instruction(scrolling is disabled): %d", code);
		break;
	}
	case 0x00FD: {
		if (feats & CHIP_F__EXIT) chip8_set_status(st, CHIP_STATUS_EXITED);  //st->status = CHIP_STATUS_EXITED;
		else halt_assert(0, "Unknown instruction(exit is disabled): %d", code);
		break;
	}
	case 0x00FE:
	case 0x00FF: {
		if (feats & CHIP_F__DISPLAY_SWAPPING) chip8__display_swap(st, code == 0x00ff);
		else halt_assert(0, "Unknown instruction(display swapping is disabled): %d", code);
		break;
	}

	default: {
		CHIP8_UNPACK_X_Y_Z(code, ign1, scd, scroll_n);
		if (scd == 0xC)
		{
			if (feats & CHIP_F__DISPLAY_SCROLLING) chip8__display_scroll_down(st, scroll_n);
			else halt_assert(0, "Unknown instruction(scrolling is disabled): %d", code);
			break;
		}
		halt_assert(0, "Unknown instruction: %d", code);
	}
	//TODO quark for 0nnn [It is ignored by modern interpreters" - from Cowgod's Chip-8]
	}
}

static void chip8__1xxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_XYZ(code, nnn);
	st->pc = nnn;
}

static void chip8__2xxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_XYZ(code, nnn);
	halt_assert(st->sp < CHIP8_STACK_FRAMES, "Stack overflow");
	st->stack[st->sp++] = st->pc;
	st->pc = nnn;
}

static void chip8__3xxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_YZ(code, x, kk);
	if (st->regs[x] == kk) st->pc += CHIP8_OPCODE_SIZE;
}

static void chip8__4xxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_YZ(code, x, kk);
	if (st->regs[x] != kk) st->pc += CHIP8_OPCODE_SIZE;
}

static void chip8__5xxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_Y_Z(code, x, y, zero);
	halt_assert(zero == 0, "Unknown instruction: %d", code);
	if (st->regs[x] == st->regs[y]) st->pc += CHIP8_OPCODE_SIZE;
}

static void chip8__6xxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_YZ(code, x, kk);
	st->regs[x] = kk;
}

static void chip8__7xxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_YZ(code, x, kk);
	st->regs[x] += kk;
}

static void chip8__8xxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_Y_Z(code, x, y, op);
	uint8_t* v = st->regs;
	const bool logic_reset = st->config.quirks & CHIP_Q__8XY1_8XY2_8XY3_LOGIC_OP_RESET_VF;
	bool c;
	switch (op) {
	case 0x0: v[x] = v[y]; break;
	case 0x1: v[x] |= v[y]; if (logic_reset) v[0xF] = 0; break;
	case 0x2: v[x] &= v[y]; if (logic_reset) v[0xF] = 0; break;
	case 0x3: v[x] ^= v[y]; if (logic_reset) v[0xF] = 0; break;
	case 0x4: c = (v[x] + v[y]) > 0xff; v[x] += v[y]; v[0xF] = c;  break;
	case 0x5: c = (v[x] >= v[y]); v[x] = v[x] - v[y]; v[0xF] = c; break;
	case 0x7: c = (v[y] >= v[x]); v[x] = v[y] - v[x]; v[0xF] = c; break;
	case 0x6: 
	case 0xE: {
		const uint8_t src = (st->config.quirks & CHIP_Q__8XY6_8XYE_SHIFT_VY) ? y : x;
		if (op == 0x6) { c = (v[src] & 0x01); v[x] = v[src] >> 1; v[0xF] = c; } else
		if (op == 0xE) { c = (v[src] & 0x80); v[x] = v[src] << 1; v[0xF] = c; } else assert(0);
		break;
	}
	default: halt_assert(0, "Unknown instruction: %d", code);
	}
}

static void chip8__9xxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_Y_Z(code, x, y, zero);
	halt_assert(zero == 0, "Unknown instruction: %d", code);
	if (st->regs[x] != st->regs[y]) st->pc += CHIP8_OPCODE_SIZE;
}

static void chip8__axxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_XYZ(code, nnn);
	st->index = nnn;
}

static void chip8__bxxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_XYZ(code, nnn);
	if (!(st->config.quirks & CHIP_Q__BXNN_HIGH_NIBBLE_JUMP)) {
		st->pc = st->regs[0] + nnn;
	}
	else {
		st->pc = st->regs[nnn>>8] + nnn;
	}
}

static void chip8__cxxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_YZ(code, x, kk);
	uint8_t rnd = (uint8_t)(st->config.rnd_fn(st->config.rnd_userdata) & 0xFF);
	st->regs[x] = rnd & kk;
}


static void chip8__dxxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_Y_Z(code, x, y, n);
	int w_int, h_int;
	chip8_get_display_wh(st, &w_int, &h_int);
	const unsigned w = (unsigned)w_int;
	const unsigned h = (unsigned)h_int;

	const uint_least32_t quirks = st->config.quirks;
	const bool need_wrap = quirks & (st->hires ? CHIP_Q__DXYN_HIRES_SPRITES_WRAP_ON : CHIP_Q__DXYN_LORES_SPRITES_WRAP_ON);
	const bool need_coll_cnt = st->hires && (quirks & CHIP_Q__DXYN_HIRES_COLLISION_COUNT_VF);
	const bool need_coll_bot_cnt = st->hires && (quirks & CHIP_Q__DXYN_HIRES_COLLISION_BOTTOM_VF);
	
	unsigned sprite_width, sprite_height;
	if ((quirks & CHIP_Q__DXYN_ALLOW_DXY0) && (n == 0)) {
		sprite_width = sprite_height = CHIP8_HISPRITE_WIDTH;
	}
	else
	{
		sprite_width = CHIP8_LOSPRITE_WIDTH;
		sprite_height = n;
	}

	unsigned vx = st->regs[x] % w;
	unsigned vy = st->regs[y] % h;

	unsigned coll_bot_cnt = 0, coll_cnt = 0;
	for (unsigned row = 0; row < sprite_height; ++row) {
		unsigned pxrow = 0;
		if (sprite_width == CHIP8_HISPRITE_WIDTH)  // DXY0
			pxrow = (st->mem[st->index + row * 2] << 8) | st->mem[st->index + row * 2 + 1];
		else
			pxrow = st->mem[st->index + row];

		unsigned dy = (vy + row);
		if (dy >= h) {
			coll_bot_cnt+=sprite_width;
			if (need_wrap) dy = dy % h; else continue;
		}
		for (unsigned i = 0;i < sprite_width;++i) {
			unsigned dx = (vx + i);
			if (dx >= w) {
				if (need_wrap) dx = dx % w; else continue;
			}
			bool oldpx = chip8__display_get_px(st, dx, dy); 
			bool newpx = oldpx ^ ((pxrow >> (sprite_width - 1 - i)) & 1);
			chip8__display_set_px(st, dx, dy, newpx);
			coll_cnt += oldpx && !newpx;
		}
	}
	st->regs[0xf] = need_coll_cnt? (uint8_t)coll_cnt : ((bool)coll_cnt);
	if (need_coll_bot_cnt)
		st->regs[0xf] += (uint8_t)coll_bot_cnt;
}

static void chip8__exxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_YZ(code, x, kk);
	const bool key = st->keys[st->regs[x] & 0xF];
	switch (kk) {
		case 0x9E: if (key) st->pc += CHIP8_OPCODE_SIZE; break;
		case 0xA1: if (!key) st->pc += CHIP8_OPCODE_SIZE; break;
		default:halt_assert(0, "Unknown instruction: %d", code); break;
	}
}

static void chip8__fx0a_impl(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_YZ(code, x, ign);
	int key = chip8_input_get_key(st);
	if (key >= 0) st->regs[x] = (uint8_t)key;
	else st->pc -= CHIP8_OPCODE_SIZE; //We are waiting for a key
}

static void chip8__fx33_impl(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_YZ(code, x, ign);
	uint8_t* v = st->regs;
	st->mem[st->index + 0] = v[x] / 100;
	st->mem[st->index + 1] = (v[x] / 10) % 10;
	st->mem[st->index + 2] = v[x] % 10;
}

static void chip8__fx55_fx65_impl(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_YZ(code, x, op);
	const size_t copy_cnt = (utils_clampi(x + 1, 1, CHIP8_REGISTERS)) * sizeof(st->regs[0]);
	const bool inc_byx = st->config.quirks & CHIP_Q__FX55_FX65_INC_I_BY_X;
	const bool inc_by1 = st->config.quirks & CHIP_Q__FX55_FX65_INC_I_BY_1;
	switch (op) {
		case 0x55: memcpy(&st->mem[st->index], st->regs, copy_cnt); break;//todo check correctness of memory
		case 0x65: memcpy(st->regs, &st->mem[st->index], copy_cnt); break;
		default: halt_assert(0, "Unknown instruction: %d", code); break;
	}
	if (inc_byx) st->index += x;
	if (inc_by1) st->index += 1;
}

static void chip8__fx75_fx85_impl(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_YZ(code, x, op);
	const bool has_rpl = st->config.features & CHIP_F__RPL;
	if (!has_rpl) halt_assert(0, "Unknown instruction(rpl is disabled): %d", code);
	const size_t copy_cnt = (utils_clampi(x+1, 1, CHIP8_RPL_REGISTERS)) * sizeof(st->rpl[0]);
	switch (op) {
		case 0x75: memcpy(st->rpl, st->regs, copy_cnt); break;
		case 0x85: memcpy(st->regs, st->rpl, copy_cnt); break;
		default: halt_assert(0, "Unknown instruction: %d", code); break;
	}
}

/*
//Mask I first to handle cases where I is already > 0x0FFF.
uint16_t nexti = (st->index & 0x0FFF) + v[x]; st->index = nexti & 0x0FFF;
if (has_overflow) v[0xF] = nexti > 0x0FFF;*/

static void chip8__fxxx(chip8* st, uint16_t code) {
	CHIP8_UNPACK_X_YZ(code, x, op);
	uint8_t* v = st->regs;
	const bool has_overflow = st->config.quirks & CHIP_Q__FX1E_VF_OVERFLOW;
	const bool has_hifont = st->config.features & CHIP_F__HIRES_FONT;
	switch (op) {
		case 0x02: break;
		case 0x07:v[x] = st->delay_tim;  break;
		case 0x0A: chip8__fx0a_impl(st, code); break;
		case 0x15: st->delay_tim = v[x]; break;
		case 0x18: st->sound_tim = v[x]; break;
		case 0x1E: st->index += v[x]; if (has_overflow) { v[0xF] = (st->index > 0x0FFF); } break;
		case 0x29: st->index = CHIP8_LOFONT_BASE + v[x] * CHIP8_LOFONT_HEIGHT; break;
		case 0x30: if (has_hifont) st->index = CHIP8_HIFONT_BASE + v[x] * CHIP8_HIFONT_HEIGHT; else halt_assert(0, "Unknown instruction(hires font not loaded): %d", code); break;
		case 0x33: chip8__fx33_impl(st, code); break;
		case 0x55:
		case 0x65: chip8__fx55_fx65_impl(st, code); break;
		case 0x75: 
		case 0x85: chip8__fx75_fx85_impl(st, code); break;
		default:halt_assert(0, "Unknown instruction: %d", code); break;
	}
}
/*END OF ~CHIP8 INSTUCTIONS SECTION~*/

static inline uint16_t chip8_fetch_opcode(chip8* st, bool advance) {
	halt_assert(st->pc < CHIP8_MEMORY, "program counter(pc) out of memory size, pc: %d", (int)st->pc);
	uint16_t opcode = (st->mem[st->pc] << 8) | st->mem[st->pc + 1];
	if (advance) st->pc += CHIP8_OPCODE_SIZE;
	return opcode;
}

static void chip8_step(chip8* st) {
	if (st->status != CHIP_STATUS_RUNNING) return;
	typedef void (*instruction) (chip8* st, uint16_t code);
	static const instruction instructions[] = {
		[0x0] = chip8__0xxx, [0x1] = chip8__1xxx, [0x2] = chip8__2xxx, [0x3] = chip8__3xxx,
		[0x4] = chip8__4xxx, [0x5] = chip8__5xxx, [0x6] = chip8__6xxx, [0x7] = chip8__7xxx,
		[0x8] = chip8__8xxx, [0x9] = chip8__9xxx, [0xa] = chip8__axxx, [0xb] = chip8__bxxx,
		[0xc] = chip8__cxxx, [0xd] = chip8__dxxx, [0xe] = chip8__exxx, [0xf] = chip8__fxxx,
	};
	uint16_t code = chip8_fetch_opcode(st, true);
	uint16_t op = code >> 12;
	halt_assert(op < COUNTOF_192F(instructions), "Unknown instruction: %d", code);
	instructions[op](st, code);
}


static void chip8_timer_tick(chip8* st) {
	if (st->status != CHIP_STATUS_RUNNING) return;
	if (st->delay_tim > 0) st->delay_tim--;
	if (st->sound_tim > 0) st->sound_tim--;
}
#endif