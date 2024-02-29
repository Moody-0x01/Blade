#ifndef MI_H
#define MI_H
#include <logger.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ESC         0x1b
#define KEY_COPY_   'y'
#define KEY_PASTE_  'p'
#define KEY_INSERT_ 'i'
#define KEY_VISUAL_ 'v'
#define KEY_SAVE_   'w'
#define KEY_DEL     0x014a
#define MENU_HEIGHT_ 1
#define L_SHIFT 0x189
#define R_SHIFT 0x192

// clipboard api
typedef enum ClipBoardEvent {
    CFREE,
    CGET,
    CSET
} ClipBoardEvent;

void *clipboard(ClipBoardEvent e, char *data);
#define CLIPBOARD_SET(data) clipboard(CSET, data);
#define CLIPBOARD_GET() (char *) clipboard(CGET, NULL);
#define CLIPBOARD_FREE() clipboard(CFREE, NULL);

// color editor pairs
#define SECONDARY_THEME_PAIR 1
#define MAIN_THEME_PAIR      2
#define ERROR_PAIR           3
#define BLUE_PAIR            4
#define HIGHLIGHT_THEME      5 

#define CTRL(x) ((x) & 037)
#define LINE_SZ 64
#define LINE_NUM_MAX 8
#define TAB '\t'
#define NL  '\n'

#define OPAR '('
#define CPAR ')'
#define OCERLY '{'
#define CCERLY '}'

#define OBRAC '['
#define CBRAC ']'

#define DQUOTE '\"'
#define SQUOTE '\''
#define WLCM_BUFF "(WELLCOME TO MI EDITOR V0.1!)"

typedef enum charType {
    NUMBER,
    SYMBOL,
    ALPHABET,
    SPACE,
    UNK
} charType;

charType get_class(int key);

typedef enum ResultType {
    SUCCESS,
    ERROR
} ResultType;

typedef enum ErrorType {
    NONE,
    EXIT_SIG,
    EMPTY_BUFF
} ErrorType;

typedef enum editorMode {
    NORMAL = 0,
    VISUAL,
    INSERT
} editorMode;

#define MAX_KEY_BINDIND 2

typedef enum bindingKind {
    COPY_LINE, // yy
    DEL_LINE,  // dd || cc
    NOT_VALID
} bindingKind;

typedef enum MITokenType {
    STR,
    KEYWORD
} MITokenType;


typedef struct vKeyBindingQueue {
    char keys[MAX_KEY_BINDIND];
    int size;
    bindingKind kind;
} vKeyBindingQueue;

typedef struct MIToken {
    int xstart, xend, y;
    char data[512];
    MITokenType kind;
} MIToken;

typedef struct Chunk {
    char *data;
    int cap, size;
} Chunk;

typedef struct Line Line;

typedef struct Vec2 {
    int x, y;
    Line *_line;
} Vec2;

typedef struct Line {
    int     x, y, size, padding, cap;
    char    line_number[LINE_NUM_MAX];
    char    *content;
    Line    *next, *prev;
    MIToken *Tokens;
} Line;
    
typedef struct Lines_renderer {
    Line   *origin;
    Line   *start;
    Line   *end;
    Line   *current;
    int    win_h, win_w;
    int    max_padding;
    int    count;
} Lines_renderer;

typedef struct Result {
    ResultType type;
    ErrorType  etype;
    char *data;
} Result;
typedef struct Editor {
    Lines_renderer line_ren;
    editorMode mode;
} Editor;
WINDOW *init_editor();
int load_file(char *file_path, Lines_renderer *line_ren);
int save_file(char *file_path, Line *lines, bool release);
void render_lines(Lines_renderer *line_ren);
void editor_tabs(Line *line);
void editor_backspace(Lines_renderer *line_ren);
void editor_push_data_from_clip(Lines_renderer *line_ren);
char *editor_render_startup(int x, int y);
void lines_shift(Line *head, int num);


void editor_dl(Line *line);

void editor_apply_move(Lines_renderer *line_ren);

void editor_new_line(Lines_renderer *line_ren, bool reset_borders);
void free_lines(Line *lines);
void line_push_char(Line *line, char c, bool pasted);
void line_disconnect_from_ren(Lines_renderer *line_ren);

Line *Alloc_line_node(int row);

Result *make_prompt_buffer(int x, int y);
int    highlight_until_current_col(Vec2 start, Lines_renderer *line_ren);
void   editor_paste_content(Vec2 start, Vec2 end, Lines_renderer *line_ren);
void  clipboard_save_chunk(Vec2 start, Vec2 end);

// Editor movement stuff..
void editor_up(Lines_renderer *line_ren);
void editor_left(Lines_renderer *line_ren);
void editor_down(Lines_renderer *line_ren);
void editor_right(Lines_renderer *line_ren);
void handle_move(int c, Lines_renderer *line_ren);
bool is_move(int key);
void editor_details(Lines_renderer *line_ren, char *file_path, editorMode mode, char *notification);
void editor_handle_binding(Lines_renderer *line_ren, vKeyBindingQueue *bindings);
void editor_identify_binding(vKeyBindingQueue *bindings);
// CHUNK

void  chunk_append_s(Chunk *c, char *str);
void  chunk_append_char(Chunk *c, char chr);
Chunk *chunk_new();
void  chunk_distroy(Chunk *c);
#endif // MI_H

