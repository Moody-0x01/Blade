#include <mi.h>
// int fgetc(FILE *stream);
Line *Alloc_line_node(size_t row)
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

WINDOW *init_editor(char *file_name)
{
    int h, w;

    WINDOW *win = initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
    cbreak();
    getmaxyx(win, h, w);

    for (int i = 0; i < w; ++i) {
        mvprintw(h - 3, i, "_");
    }

    mvprintw(h - 2, 0, "Editing %s", file_name);
    move(0, 0);
    return win;
}

size_t load_file(char *file_path, Line *lines)
{
    size_t line_count = 0;
    Line *new = NULL;
    Line *current = lines;
    char c;
    FILE *Stream = fopen(file_path, "r");

    if (!Stream) {
        return line_count;
    }

    while((c = fgetc(Stream)) != EOF) {
        if (c == '\n') {
            new = Alloc_line_node(current->y + 1);
            current->next = new;
            new->prev = current;
            current = new;
            line_count++;
        } else {
            current->content[current->x++] = c;
            current->size++;
        }

        if (!line_count) line_count++;
    }

    fclose(Stream);
    return line_count;
}

void save_file(char *file_path, Line *lines, size_t save_count) {
    FILE *Stream = fopen(file_path, "w+");
    Line *next = NULL;
    Line *current = lines;

    for (size_t i = 0; (i < save_count && current); ++i) {
        fprintf(Stream, "%s\n", current->content);
        next = current->next;
        free(current);
        current = next;
    }

    fclose(Stream);
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

void render_line(Line *line)
{
    size_t x = 0, n = 0;
    char line_number[LINE_NUM_MAX] = { 0 };
    line->padding = (size_t)(sprintf(line_number, "%zu  ", line->y + 1));

    for (n = 0; n < line->padding; n++) {
        mvaddch(line->y, n, line_number[n]);
    }

    for (x = 0; x < line->size; ++x) {
        mvaddch(line->y, x + line->padding, line->content[x]);
    }
}

void render_lines(Line *lines)
{
    Line *current = lines;
    for (; current;) {
        render_line(current);
        current = current->next;
    }
}

void debugger(WINDOW *win, size_t x, size_t y, size_t size)
{
    int h, w;
    getmaxyx(win, h, w);
    mvprintw(h - 2, w - (12 + 20), "x -> %zu y -> %zu s -> %zu", x, y, size);
    move(y, x);
}


void line_push_char(Line *line, char c)
{
    memmove((line->content + line->x + 1), 
            (line->content + line->x),
            line->size - line->x);
    line->content[line->x++] = c;
    line->size++;
}


void editor_tabs(Line *line)
{
    int tab_count = 2;
    for (int n = 0; n < tab_count; ++n) {
        line_push_char(line, TAB);
    }
}
