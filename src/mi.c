#include <mi.h>

static char *modes[] = { 
    "NORMAL",
    "VISUAL",
    "INSERT"
};

Line *Alloc_line_node(int row)
{
    Line *line = (Line *)malloc(sizeof(Line));

    line->next = NULL;
    line->prev = NULL;
    line->x    = 0;
    line->size = 0;
    line->y    = row;

    for (int i = 0; i < LINE_SZ; ++i)
        line->content[i] = 0x0;

    return (line);
}

static void lines_shift(Line *head, int num) {

    Line *curr = head;
    for (;curr;) {
        curr->y += num;
        curr = curr->next;
    }
}
static void init_colors() 
{
    start_color();
    // pair_number, foreground, background
    init_color(COLOR_BLACK, 62, 94, 125);
    init_color(COLOR_YELLOW, 996, 905, 82);
    init_color(COLOR_BLUE, 0, (44 * 1000) / 255, (84 * 1000)/255);
    init_pair(MAIN_THEME_PAIR, COLOR_WHITE, COLOR_BLACK);
    init_pair(SECONDARY_THEME_PAIR, COLOR_BLACK, COLOR_YELLOW);
    init_pair(ERROR_PAIR, COLOR_WHITE, COLOR_RED);
    init_pair(BLUE_PAIR, COLOR_WHITE, COLOR_BLUE);
}

WINDOW *init_editor()
{
    WINDOW *win = initscr();

    raw();
	keypad(stdscr, TRUE);
	noecho();
    cbreak();
    init_colors();

    wbkgd(win, COLOR_PAIR(COLOR_BLACK));

    return win;
}

int load_file(char *file_path, Lines_renderer *line_ren)
{
    Line *new = NULL;
    char c;
    FILE *Stream = fopen(file_path, "r");
    char line_number[LINE_NUM_MAX] = { 0 };

    if (!Stream) { 
        line_ren->count++;
        goto SET_PROPS;
    }

    while((c = fgetc(Stream)) != EOF) {
        if (c == '\n') {
            new = Alloc_line_node(line_ren->current->y + 1);
            line_ren->current->next = new;
            new->prev = line_ren->current;
            line_ren->current = new;

            if (line_ren->current->y - line_ren->start->y == line_ren->win_h - 4) { 
                // when we reach the last row that will be rendered we nark i.
                line_ren->end = line_ren->current;
            }

            line_ren->count++;
        } else {
            line_ren->current->content[line_ren->current->x++] = c;
            line_ren->current->size++;
        }

        if (!line_ren->count) line_ren->count++;
    }
    
    if (line_ren->count > 1 && (line_ren->start == line_ren->end)) {
        line_ren->end = line_ren->current;
    }

SET_PROPS:
    line_ren->max_padding = sprintf(line_number, "%u", line_ren->current->y + 1) + 1;
    if (line_ren->max_padding < 1) line_ren->max_padding++;
    line_ren->current = line_ren->start;
    if (Stream) fclose(Stream);
    return line_ren->count;
}


int save_file(char *file_path, Line *lines, bool release)
{
    FILE *Stream = fopen(file_path, "w+");
    int  bytes_saved = 0;
    Line *next = NULL;
    Line *current = lines;

    for (; current;) {
        bytes_saved += fprintf(Stream, "%s\n", current->content);
        next = current->next;

        if (release)
            free(current);
        
        current = next;
    }

    fclose(Stream);
    return bytes_saved;
}

void free_lines(Line *lines) {
    Line *next = NULL;
    Line *current = lines;

    for (; (current);) {
        next = current->next;
        free(current);
        current = next;
    }
}

static void render_line(Line *line, int offset, int max_padding)
{
    int x = 0, n = 0;
    char line_number[LINE_NUM_MAX] = {0};
    line->padding = sprintf(line_number, "%u", line->y + 1);

    for (n = 0; n < line->padding; n++) {
        mvaddch(line->y - offset, n, line_number[n]);
    }

    for (; n < max_padding; n++) {
        mvaddch(line->y - offset, n, ' ') ;
    }
    
    for (x = 0; x < line->size; ++x) {
         mvaddch(line->y - offset, x + n, line->content[x]);
    }
}

void editor_backspace(Lines_renderer *line_ren)
{
    Line *next, *prev, *current;

    // NORMAL backspacing. 
    if (line_ren->current->x > 0) {
        memmove(
            line_ren->current->content + line_ren->current->x - 1,
            line_ren->current->content + line_ren->current->x,
            line_ren->current->size - line_ren->current->x
        );
   
        line_ren->current->x--;
        line_ren->current->size--;
        line_ren->current->content[line_ren->current->size] = 0;
        return;
    }
    
    // if the cursor is on the first col of a line and we need to shift lines and data  backward
    if (line_ren->current->x == 0 && line_ren->current->prev) {
        current = line_ren->current;
        prev = current->prev;
        next = current->next;

        if (current->size) {
            // move all the data locateed in the curr line to the end of the prev line!
            memmove(
                prev->content + prev->size,
                current->content,
                current->size
            );
            prev->size += current->size;
        }

        
        prev->next = next;

        if (next)
            next->prev = prev;
        
        free(current);
    
        line_ren->current = prev; // go back one line.
        lines_shift(next, -1); 
        return;
    }

    line_ren->current->content[line_ren->current->x] = 0;
}
void render_lines(Lines_renderer *line_ren)
{
    Line *current = line_ren->start;
    while (current) {
        render_line(current, line_ren->start->y, line_ren->max_padding);
        if (current == line_ren->end) break;
        current = current->next;
    }
}

void editor_details(Lines_renderer *line_ren, char *file_path, editorMode mode_, char *notification)
{

    char details_buffer[LINE_SZ] = { 0 };
    char *mode = modes[mode_];
    sprintf(details_buffer, "row: %d col: %d line_count: %d ", line_ren->current->x + 1, line_ren->current->y + 1, line_ren->count);
    mvprintw(line_ren->win_h - 1, 0, " %s ", mode);
    mvchgat(line_ren->win_h - 1, 0, strlen(mode) + 2, A_NORMAL, BLUE_PAIR, NULL);
    mvprintw(line_ren->win_h - 1, strlen(mode) + 3, " %s %s", file_path, notification);
    mvprintw(line_ren->win_h - 1, line_ren->win_w - strlen(details_buffer) - 1, details_buffer);
    mvchgat(line_ren->win_h - 1, strlen(mode) + 2, line_ren->win_w, A_NORMAL, SECONDARY_THEME_PAIR, NULL);
    move(line_ren->current->y, line_ren->current->x);
}

void char_inject(Line *line, char c)
{
    memmove((line->content + line->x + 1),
        (line->content + line->x),
        line->size - line->x);
    line->content[line->x++] = c;
    line->size++;
}

void line_push_char(Line *line, char c)
{
    char_inject(line, c);

    switch (c) {
        case OPAR: {
            char_inject(line, CPAR);
            line->x--;
        } break;
        case OCERLY: {
            char_inject(line, CCERLY);
            line->x--;
        } break;
        case OBRAC: {
            char_inject(line, CBRAC);
            line->x--;
        } break;
        case DQUOTE: {
            char_inject(line, DQUOTE);
            line->x--;
        } break;
        case SQUOTE: {
            char_inject(line, SQUOTE);
            line->x--;
        } break;
        default: { } break;
    }
}

void editor_tabs(Line *line)
{
    for (int i = 0; i < 4; ++i)
        line_push_char(line, ' ');
}

void editor_new_line(Lines_renderer *line_ren)
{
    Line *new, *next;
    char line_number[LINE_NUM_MAX] = { 0 };
    int  new_max;
    new = Alloc_line_node(line_ren->current->y + 1);
    new_max = sprintf(line_number, "%u", line_ren->current->y + 1) + 1;

    if (new_max > line_ren->max_padding)
        line_ren->max_padding = new_max;

    if (!(line_ren->current->next) && line_ren->current->x == line_ren->current->size) 
    {
        line_ren->current->next = new;
        new->prev = line_ren->current;
        line_ren->end = new;
        line_ren->current = new;
        
        if (line_ren->end->y - line_ren->start->y > line_ren->win_h - 4) {
            line_ren->start = line_ren->start->next;
        }

        line_ren->count++;
        return;
    }

    if (line_ren->current->next && line_ren->current->x == line_ren->current->size) {
        next = line_ren->current->next;
        new->prev = line_ren->current; // correct
        new->next = next; // correct.
        line_ren->current->next = new; // correct
        next->prev = new;
        lines_shift(new->next, 1);
        if (line_ren->end->y - line_ren->start->y > line_ren->win_h - 4) 
            line_ren->end = line_ren->end->prev;

        line_ren->current = line_ren->current->next;
        line_ren->count++;
        return;
    }
    
    // Allocate new line
    if (line_ren->current->next) {
        // prev = line_ren->current->prev;
        next = line_ren->current->next;
        new->next = next;
        next->prev = new;
        new->prev = line_ren->current;
        line_ren->current->next = new;
        new->size = line_ren->current->size - line_ren->current->x;
        line_ren->current->size -= new->size;
        
        // move memory
        memmove(new->content,
            line_ren->current->content + line_ren->current->x,
            new->size
        );

        if (line_ren->end->y - line_ren->start->y > line_ren->win_h - 4) 
            line_ren->end = line_ren->end->prev;

        lines_shift(new->next, 1);
    } else {
        line_ren->current->next = new;
        new->prev = line_ren->current;
        new->size = line_ren->current->size - line_ren->current->x;
        line_ren->current->size -= new->size;
        memmove(new->content,
                line_ren->current->content + line_ren->current->x,
                new->size
        );

        line_ren->end = new;
        if (line_ren->end->y - line_ren->start->y > line_ren->win_h - 4) {
            line_ren->start = line_ren->start->next;
        }
    }

    memset(line_ren->current->content + line_ren->current->x, 0, new->size);
    line_ren->current = line_ren->current->next;
    line_ren->count++;
}

static int evenize(const char *s) {
    int length = strlen(s);
    return (length % 2) ? length : length + 1;
}

static int editor_render_help(int x, int y, char *error)
{
    // render the welcome window in the middle of the screen.
    char *prompt = "Enter the path to the file that u want to edit: ";
    char *save_p = "[ F1 ] to save the edited file";
    char *quit_p = "[ F2 ] to quit without saving";
    
    attron(A_STANDOUT);	
    mvprintw(y, x - evenize(WLCM_BUFF)/2, WLCM_BUFF);
    attroff(A_STANDOUT);  
    mvprintw(y + 3, x - evenize(save_p)/2, save_p);
    mvprintw(y + 5, x - evenize(quit_p)/2, quit_p);
    
    if (error) {
        mvprintw(y + 7, x - evenize(error)/2, error);
        mvchgat(y + 7, x - evenize(error)/2, strlen(error), A_NORMAL, ERROR_PAIR, NULL);	
    }
    
    attron(A_UNDERLINE);
    mvprintw(y * 2, 0, prompt);
    attroff(A_UNDERLINE);

    return (strlen(prompt));
}

Result *make_prompt_buffer(int x, int y)
{
    Result *result = malloc(sizeof(Result));
    bool deleted = false;
    
    result->data = malloc(LINE_SZ);
    result->type = SUCCESS;
    result->etype = NONE;

    int buffer_idx = 0;
    int size = 0, byte = 0;
    
    if (result == NULL || result->data == NULL)
        return NULL;
    
    byte = getch();
    while (true) { 
        switch(byte) {
            case KEY_F(2): {
                result->type = ERROR;
                result->etype = EXIT_SIG;
                return result;
            } break;
            case NL: {
                if (size == 0) {
                    result->type = ERROR;
                    result->etype = EMPTY_BUFF;
                    result->data = strcpy(result->data, "File path/name can not be empty!");
                    return result;
                }
                result->type = OK;
                return result;
            } break;
            case KEY_BACKSPACE: {
                if (buffer_idx == size && size) {
                    result->data[buffer_idx--] = 0;
                    size--;
                } else if (buffer_idx && buffer_idx < size) {
                    memmove(
                        result->data + buffer_idx - 1,
                        result->data + buffer_idx,
                        size - buffer_idx
                    );
                    size--;
                    buffer_idx--;
                } else {
                    result->data[buffer_idx] = 0;
                }

                deleted = true;
            } break;
            case KEY_RIGHT: {
                if (buffer_idx < size) buffer_idx++; 
            } break;
            case KEY_LEFT: {
                if (buffer_idx > 0) buffer_idx--; 
            } break;
			case KEY_HOME:{
                buffer_idx = 0;
            } break;
			case KEY_END: {
                buffer_idx = size;
			} break;
            default: {
                if (buffer_idx == size) {
                    buffer_idx++;
                    result->data[size++] = byte;
                } else if (buffer_idx < size) {
                    memmove(result->data + buffer_idx + 1,
                        result->data + buffer_idx,
                        size - buffer_idx);

                    result->data[buffer_idx++] = byte;
                    size++;
                }
            } break;
        }

        for (int i = 0; i < size; ++i)
            mvaddch (y, x + i, result->data[i]);


        if (deleted || (!size && !buffer_idx)) {
            delch();
            deleted = false;
        }

        move(y, x + buffer_idx);
        byte = getch();
    }
}

char *editor_render_startup(int x, int y)
{
    int prompt_offset = editor_render_help(x, y, NULL);
    Result *res = NULL;
    char *file_path = malloc(LINE_SZ);
    while (true) {
        res = make_prompt_buffer(prompt_offset, y * 2);
        switch(res->type) {
            case SUCCESS: {
                strcpy(file_path, res->data);
                free(res->data);
                free(res);
                return file_path;
            } break;
            case ERROR: {
                if (res->etype == EXIT_SIG) {    
                    free(res->data);
                    free(res);
                    free(file_path);
                    return NULL;
                } else if (res->etype == EMPTY_BUFF) {
                    prompt_offset = editor_render_help(x, y, res->data);
                    free(res->data);
                    free(res);
                } else {
                    printf("Unreachable code\n");
                    exit(1);
                }
            } break;
            default: {
                printf("Unreachable code\n");
                exit(1);
            }
        }
    }
}

