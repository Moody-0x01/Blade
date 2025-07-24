#ifndef BLADE_H
#define BLADE_H

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */
#include <fcntl.h>
#include <colors.h>
#include <common.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <locale.h>
#include <assert.h>
#include <clipboard.h>
#include <ctype.h>
#include <emojis.h>
#include <file_browser.h>
#include <cfg.h>
#include <filessystem.h>
#include <logger.h>
#include <player.h>
#include <ncurses.h>
#include <signals_.h>
#include <xstring.h>
#include <Array.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>


#define INIT_COMMAND_CAP 25

#define ESC 0x1b
#define KEY_CAP 'u'
#define KEY_COMMAND_ ':'
#define KEY_COMMAND_O ';'
#define KEY_DOT '.'
#define KEY_COPY_ 'y'
#define KEY_CUT_ 'c'
#define KEY_PASTE_ 'p'
#define KEY_INSERT_ 'i'
#define KEY_VISUAL_ 'v'
#define KEY_SAVE_ 'w'

#define KEY_QUIT 'q'
#define KEY_QUIT_SAVE 'x'
#define KEY_DEL 0x014a
#define MENU_HEIGHT_ 3
#define L_SHIFT 0x189
#define R_SHIFT 0x192

// color editor pairs
#define CTRL(x) ((x) & 037)
#define SHIFT(x) ((x) - 32)

#define CTRL_Z CTRL('z')
#define CTRL_C CTRL('c')
#define CTRL_V CTRL('c')

#define LINE_SZ 512
#define LINE_NUM_MAX 8
#define MAX_TOKENS 64
#define TAB '\t'
#define NL '\n'

#define OPAR '('
#define CPAR ')'
#define OCERLY '{'
#define CCERLY '}'

#define OBRAC '['
#define CBRAC ']'

#define DQUOTE '\"'
#define SQUOTE '\''
#define WLCM_BUFF "(WELLCOME TO MI EDITOR V0.1!)"
#define MI_V "Blade 0.0.1\n"
// command

typedef enum MotionKey
{
	KEY_J = 'j',
	KEY_JU = SHIFT(KEY_J),

	KEY_H = 'h',
	KEY_HU = SHIFT(KEY_H),

	KEY_K = 'k',
	KEY_KU = SHIFT(KEY_K),

	KEY_L = 'l',
	KEY_LU = SHIFT(KEY_L),
	KEY_B = 'b',
	KEY_W = 'w'
} MotionKey;

typedef struct eCommand {
	char *name;
	char **argv;
	size_t cap;
	size_t size;
} eCommand;

typedef enum charType { NUMBER, SYMBOL, ALPHABET, SPACE, UNK } charType;

charType get_class(int key);

typedef enum ResultType { SUCCESS, ERROR } ResultType;

typedef enum ErrorType { NONE, EXIT_SIG, EMPTY_BUFF } ErrorType;

char *get_modeascstr(editorMode mode);

#define MAX_KEY_BINDIND 2

typedef enum bindingKind {
	COPY_LINE, // yy
	DEL_LINE,  // dd || cc
	INDENT_LINE,
	UNINDENT_LINE,
	NOT_VALID
} bindingKind;

typedef struct Path {
	int count, capacity;
	char **items;
	BrowseEntryT type;
} Path;

// It is okay cuz it does not need to be dynamic..
typedef struct KeywordList {
	int size;
	char *extension;
	char *_list[100];
} KeywordList;

// TODO: IMPLEMET FUNCTIONS THAT CAN COLLECT THESE TOKENS..
typedef enum BladeTokenType {
	/* Groups */
	STR_LIT_INCOM,
	STR_LIT,
	NUMBER_LIT,
	KEYWORD,
	ID,
	/* Special tokens. */
	CALL,
	C_INCLUDE,
	C_TAG,
	C_INCLUDE_FILE,
	C_ENUM,
	C_TYPEDEF,
	C_STRUCT,
	C_UNION,
	TYPE,
	_GENERIC_NULL,
	COMMENT,
	/* Syms */
	EQ,          // =
	GT,          // >
	LT,          // <
	AST,         // *
	AND,         // &
	PIPE,        // |
	HASHTAG,     // #
	OPAR_,       // (
	CPAR_,       // )
	OCERLY_,     // {
	CCERLY_,     // }
	OBRAC_,      // [
	CBRAC_,      // ]
	COMA,        // ,
	PERIOD,      // .
	DOLLARSIGN,  // $
	AT,          // @
	BANG,        // !
	PLUS,        // +
	MINUS,       // -
	SEMICOLON,   // ;
	COLON,       // :
	FSLASH,      // /
	BSLASH,      // '\' /
	OTHER_PUNCT, // Any unknown punct.
} BladeTokenType;

typedef struct vKeyBindingQueue {
	char keys[MAX_KEY_BINDIND];
	int size;
	bindingKind kind;
} vKeyBindingQueue;

typedef struct BladeToken {
	BladeTokenType kind;
	int xstart, xend;
	char *data;
} BladeToken;

typedef struct TokenList {
	BladeToken *_list;
	int size, cap;
} TokenList;

typedef struct Chunk {
	char *data;
	int cap, size, lines;
} Chunk;

typedef struct Line Line;

typedef struct Vec2 {
	int x, y;
	Line *_line;
} Vec2;

typedef struct Line {
	int x, y, size, padding, cap, tokens_size;
	char line_number[LINE_NUM_MAX];
	char *content;
	Line *next, *prev;
	TokenList token_list;
} Line;

typedef struct Lines_renderer {
	Line *origin;
	Line *start;
	Line *end;
	Line *current;
	int win_h, win_w;
	int max_padding;
	FileType file_type;
	int count;
} Lines_renderer;

typedef struct Result {
	ResultType type;
	ErrorType etype;
	char *data;
} Result;

typedef struct BladeEditor {
	Lines_renderer *renderer;
	Vec2 highlighted_start,
	highlighted_end; // The chordinated of highlighted text.
	int highlighted_data_length;
	char notification_buffer[4096];
	bool exit_pressed, char_deleted;
	editorMode mode;
	WINDOW *ewindow;
	vKeyBindingQueue binding_queue;
	FileBrowser *fb;
	BladeAudioPlayer *mplayer;
	volatile sig_atomic_t resized;
	char *cfg_path;
	EditorConfig_t *cfg;
} BladeEditor;

int editor(char **argv, const char *cfg_path);
WINDOW *init_ncurses_window(EditorConfig_t *cfg);
int load_file(char *file_path, Lines_renderer *line_ren);
int save_file(char *file_path, Line *lines, bool release);
void reinit_renderer(char *file_path, Lines_renderer *line_ren);
void render_lines(Lines_renderer *line_ren);
void editor_tabs(Line *line);
Line *disconnect_line(Line *head);
void editor_backspace(Lines_renderer *line_ren);
void editor_push_data_from_clip(Lines_renderer *line_ren);
char *editor_render_startup(int x, int y, size_t w);
void lines_shift(Line *head, int num);
void unindent_line(Line *line);
void indent_line(Line *line);
void log_line(const char *op, Line *line);
Line *cut_line(Lines_renderer *line_ren, Line *line, size_t start, size_t end);
void editor_dl(Line *line);

void editor_apply_move(Lines_renderer *line_ren);

void editor_new_line(Lines_renderer *line_ren, bool reset_borders);
void free_lines(Line *lines);

void indent_lines(Line *start, Line *end);
void unindent_lines(Line *start, Line *end);
void line_push_char(Line *line, char c, bool pasted);
void uncapitalize_region(Vec2 start, Vec2 end);
void capitalize_region(Vec2 start, Vec2 end);
void line_disconnect_from_ren(Lines_renderer *line_ren);

Line *Alloc_line_node(int row);

Result *make_prompt_buffer(int x, int y, size_t w);
int highlight_until_current_col(Vec2 start, Lines_renderer *line_ren);
void editor_paste_content(Vec2 start, Vec2 end, Lines_renderer *line_ren);
void clipboard_save_chunk(Vec2 start, Vec2 end);
void clipboard_cut_chunk(Lines_renderer *line_ren, Vec2 start, Vec2 end);

// Editor movement stuff..
void editor_up(Lines_renderer *line_ren);
void editor_left(Lines_renderer *line_ren);
void editor_down(Lines_renderer *line_ren);
void editor_right(Lines_renderer *line_ren);

void editor_insert(int c, BladeEditor *E);
void editor_visual(int c, BladeEditor *E);
void editor_normal(int c, BladeEditor *E);
void editor_handle_move(int c, BladeEditor *E);
void editor_command_(BladeEditor *E);

/*bool is_move(int key);*/

bool is_move(int key, editorMode mode);
bool is_motion_move(int key);
void editor_render_details(Lines_renderer *line_ren, char *_path,
						   editorMode mode_, char *notification);
void editor_handle_binding(Lines_renderer *line_ren,
						   vKeyBindingQueue *bindings);
void editor_identify_binding(vKeyBindingQueue *bindings);
void editor_command_execute(BladeEditor *E, char *command, editorMode mode);

// EDITOR REGISTRY.
BladeEditor *editor_get(void);
BladeEditor *editor_register_(BladeEditor *E);

// CHUNK
void chunk_append_s(Chunk *c, char *str);
void chunk_append_char(Chunk *c, char chr);
Chunk *chunk_new(void);
void chunk_distroy(Chunk *c);

// Token List ops
void token_list_append(TokenList *list, BladeTokenType kind, int xstart, int xend);
void retokenize_line(Line *line, FileType file_type);
KeywordList *get_keywords_list(FileType s);
bool is_keywrd(char *keywords[], char *word, int keywords_sz);
char *get_token_kind_s(BladeTokenType t);

FileType get_file_type(char *spath);
char *file_type_as_str(FileType s);
BladeEditor *init_editor(char *path, EditorConfig_t *cfg);
void editor_load_layout(BladeEditor *E);
void release_editor(BladeEditor *E);
void editor_refresh(BladeEditor *E);
void editor_render(BladeEditor *E);
void editor_update(int c, BladeEditor *E);
void render_file_browser(BladeEditor *E);
void editor_file_browser(int c, BladeEditor *E);

// mplayer
void editor_player_update(BladeEditor *E, int c);
void *editor_player_update__internal(void *E);
void editor_init_player_routine(BladeEditor *E, char *mp3_file);

// PATH PARSING.
Path *path_alloc(int cap);
void parse_path_internal(Path *p, char *buffer, char *delim);
void pprint(Path *p);
void parse_path(Path *p, char *path);
void editor_make_apply_path_tree(Path *p);
void release_path(Path *p);
// eCommand

void command_distroy(eCommand *c);
eCommand *command_parse(char *command);
eCommand *command_alloc(size_t cap);

#endif // BLADE_H
