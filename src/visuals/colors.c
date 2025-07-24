#include <blade.h>

void colorize(int y, int xs, int xe, int pair) {
	mvchgat(y, xs, xe, A_NORMAL, pair, NULL);
}

void make_new_color(int index, int r, int g, int b) {
	if (index > 255)
		return;
	init_color(index, ((r * 1000) / 255), ((g * 1000) / 255), ((b * 1000) / 255));
}

void make_new_color_u32(int index, uint32_t color) {
	make_new_color(index, (color >> 16) & 0xFF, (color >> 8) & 0xFF,
				(color >> 0) & 0xFF);
}
/* TODO: 
 * Make sure everything is customizable.
 * Make sure there is a defualt for every color in the config.
 * Make sure it looks beautiful.
 * */
void init_colors(EditorConfig_t *cfg) {
	start_color();
	make_new_color_u32(COLOR_DRACULA, 0x001900);
	make_new_color_u32(COLOR_LIGHT_YELLOW, 0xfdcb6e);
	make_new_color_u32(CHI_GONG_RED, 0xff7675);
	make_new_color_u32(COLOR_SILVER_, 0x4bcffa);
	make_new_color_u32(COLOR_GREY_, 0x000000);
	make_new_color_u32(COLOR_GREY_LIGHT, 0xFFEDD8);
	make_new_color_u32(COLOR_YELLOW_, 0xf9ca24); // #f9ca24
	make_new_color_u32(COLOR_APPLE, 0x6ab04c);
	make_new_color_u32(COLOR_COMMENT, 0x3f553c);
	make_new_color_u32(COLOR_TAG, 0xe74c3c);
	make_new_color_u32(COLOR_BRIGHT_GREEN, 0x2ecc71);
	make_new_color_u32(COLOR_SPECIAL_TOKENS_COLOR, 0x01a3a4);
	make_new_color_u32(COLOR_BRIGHT_YELLOW, 0xf39c12);
	make_new_color(COLOR_BLUE, 0, 44, 84);
	make_new_color(COLOR_RED, 210, 31, 60);

	// config integration: @COLOLRS
	make_new_color_u32(COLOR_BACKGROUND, cfg->theme.background);
	make_new_color_u32(COLOR_FOREGROUND, cfg->theme.foreground);
	make_new_color_u32(COLOR_HIGHLIGHT_FG_FB, cfg->theme.highlight_fg_fb);
	make_new_color_u32(COLOR_HIGHLIGHT_BG_FB, cfg->theme.highlight_bg_fb);
	make_new_color_u32(COLOR_HIGHLIGHT_FG_VMODE, cfg->theme.highlight_fg_vmode);
	make_new_color_u32(COLOR_HIGHLIGHT_BG_VMODE, cfg->theme.highlight_bg_vmode);
	make_new_color_u32(COLOR_DETAILS_BAR_BG, cfg->theme.details_bar_bg);
	make_new_color_u32(COLOR_DETAILS_BAR_FG, cfg->theme.details_bar_fg);
	make_new_color_u32(COLOR_COMMAND_PROMPT_BG,  cfg->theme.command_prompt_bg);
	make_new_color_u32(COLOR_COMMAND_PROMPT_FG, cfg->theme.command_prompt_fg);
	make_new_color_u32(COLOR_BLADE_MODE_BG,  cfg->theme.blade_mode_bg);
	make_new_color_u32(COLOR_BLADE_MODE_FG, cfg->theme.blade_mode_fg);

	// config integration: @PAIRS
	init_pair(MAIN_THEME_PAIR, COLOR_FOREGROUND, COLOR_BACKGROUND);
	init_pair(HIGHLIGHT_FB_PAIR, COLOR_HIGHLIGHT_FG_FB, COLOR_HIGHLIGHT_BG_FB);
	init_pair(HIGHLIGHT_VMODE_PAIR, COLOR_HIGHLIGHT_FG_VMODE, COLOR_HIGHLIGHT_BG_VMODE);
	init_pair(DETAILS_BAR_PAIR, COLOR_DETAILS_BAR_FG, COLOR_DETAILS_BAR_BG);
	init_pair(COMMAND_PROMPT_PAIR, COLOR_COMMAND_PROMPT_FG, COLOR_COMMAND_PROMPT_BG);
	init_pair(BLADE_MODE_PAIR, COLOR_BLADE_MODE_FG, COLOR_BLADE_MODE_BG);
	init_pair(YELLOW_BG_PAIR, COLOR_FOREGROUND, COLOR_YELLOW_);

	// =============================================================================
	// pair_number, foreground, background
	init_pair(DRACULA_PAIR, COLOR_WHITE, COLOR_DRACULA);
	init_pair(CHI_GONG_RED_PAIR, CHI_GONG_RED, COLOR_BACKGROUND);
	init_pair(LIGHT_YELLOW_PAIR, COLOR_LIGHT_YELLOW, COLOR_BACKGROUND);

	init_pair(SPECIAL_TOKEN, COLOR_SPECIAL_TOKENS_COLOR, COLOR_BACKGROUND);
	init_pair(BRIGHT_GREEN_PAIR, COLOR_BRIGHT_GREEN, COLOR_BACKGROUND);
	init_pair(BRIGHT_YELLOW_PAIR, COLOR_BRIGHT_YELLOW, COLOR_BACKGROUND);
	init_pair(TAG_PAIR, COLOR_TAG, COLOR_BACKGROUND);

	// init_pair(MAIN_THEME_PAIR, COLOR_WHITE, COLOR_GREY_);
	init_pair(HIGHLIGHT_THEME, COLOR_BLACK, COLOR_GREY_LIGHT);
	init_pair(SECONDARY_THEME_PAIR, COLOR_GREY_, COLOR_YELLOW_);
	init_pair(ERROR_PAIR, COLOR_WHITE, COLOR_RED);
	init_pair(BLUE_PAIR, COLOR_WHITE, COLOR_BLUE);

	init_pair(STRING_LIT_PAIR, COLOR_APPLE, COLOR_BACKGROUND);
	init_pair(COMENT_PAIR, COLOR_COMMENT, COLOR_BACKGROUND);
	init_pair(NUM_PAIR, COLOR_SILVER_, COLOR_BACKGROUND);


	init_pair(CALL_SYNTAX_PAIR, COLOR_CYAN, COLOR_BACKGROUND);
	init_pair(YELLOW_PAIR, COLOR_YELLOW_, COLOR_BACKGROUND);
}
