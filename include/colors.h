#ifndef COLORS_H
#define COLORS_H

// COLOR INDECES
#include <cfg.h>
typedef enum color_e {
  COLOR_GREY_ = 8,
  COLOR_YELLOW_,
  COLOR_APPLE,
  COLOR_COMMENT,
  COLOR_SILVER_,
  COLOR_TAG,
  COLOR_BRIGHT_GREEN,
  COLOR_BRIGHT_YELLOW,
  COLOR_GREY_LIGHT,
  COLOR_SPECIAL_TOKENS_COLOR,
  COLOR_LIGHT_YELLOW,
  CHI_GONG_RED,
  COLOR_DRACULA,

  // @Integrated
  COLOR_BACKGROUND,
  COLOR_FOREGROUND,
  COLOR_HIGHLIGHT_FG_FB,
  COLOR_HIGHLIGHT_BG_FB,
  COLOR_HIGHLIGHT_BG_VMODE,
  COLOR_HIGHLIGHT_FG_VMODE,
  COLOR_DETAILS_BAR_BG,
  COLOR_DETAILS_BAR_FG,
  COLOR_COMMAND_PROMPT_BG,
  COLOR_COMMAND_PROMPT_FG,
  COLOR_BLADE_MODE_BG,
  COLOR_BLADE_MODE_FG
} color_t;

// PAIRS INDECES
typedef enum pair_e {
  SECONDARY_THEME_PAIR = 0,
  YELLOW_BG_PAIR,
  MAIN_THEME_PAIR,  
  ERROR_PAIR,       
  BLUE_PAIR,        
  HIGHLIGHT_THEME,  
  YELLOW_PAIR,      
  STRING_LIT_PAIR,  
  CALL_SYNTAX_PAIR, 
  COMENT_PAIR,      
  NUM_PAIR,         
  TAG_PAIR,         
  BRIGHT_GREEN_PAIR,
  BRIGHT_YELLOW_PAIR,  
  HIGHLIGHT_FB_PAIR,
  SPECIAL_TOKEN,    
  LIGHT_YELLOW_PAIR,
  CHI_GONG_RED_PAIR,
  DRACULA_PAIR,     
  HIGHLIGHT_VMODE_PAIR,
  DETAILS_BAR_PAIR,
  COMMAND_PROMPT_PAIR,
  BLADE_MODE_PAIR
} pair_t;


#include <stdint.h>

// #define 
void make_new_color(int index, int r, int g, int b);
void make_new_color_u32(int index, uint32_t color);
void init_colors(EditorConfig_t *cfg);
void colorize(int y, int xs, int xe, int pair);

#endif // COLORS_H
