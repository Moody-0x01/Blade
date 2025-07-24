#include <logger.h>
#include <blade.h>
#include <stdio.h>
#include <stdlib.h>

WINDOW *init_ncurses_window(EditorConfig_t *cfg) {
	WINDOW *win = initscr();

	raw();
	keypad(stdscr, TRUE);
	noecho();
	// cbreak();
	init_colors(cfg);

	wbkgd(win, COLOR_PAIR(MAIN_THEME_PAIR));
	set_escdelay(0);
	{
		// Mouse stuff.
		mousemask(ALL_MOUSE_EVENTS, NULL);
		mouseinterval(0);
	}

	return win;
}

void reinit_renderer(char *file_path, Lines_renderer *line_ren) {
	// TODO: Free the lines that r currently stored.
	free_lines(line_ren->origin);

	line_ren->origin = Alloc_line_node(0);
	line_ren->count = 0;
	line_ren->start = line_ren->origin;
	line_ren->end = line_ren->origin;
	line_ren->current = line_ren->origin;
	line_ren->max_padding = 2;

	// If the path that was passed was a file, or if it does not exist. we assign.
	line_ren->file_type = get_file_type(file_path);
}

int load_file(char *file_path, Lines_renderer *line_ren) {
	char c;
	FILE *Stream = fopen(file_path, "r");
	char line_number[LINE_NUM_MAX] = {0};

	if (line_ren->count > 0)
		reinit_renderer(file_path, line_ren);
	if (!Stream) {
		line_ren->count++;
		goto SET_PROPS;
	}

	while ((c = fgetc(Stream)) != EOF) {
		if (c == '\n') {
			editor_new_line(line_ren, false);
			if (line_ren->current->y - line_ren->start->y ==
				line_ren->win_h - MENU_HEIGHT_) {
				line_ren->end = line_ren->current;
			}

			continue;
		}

		line_push_char(line_ren->current, c, true);
		if (!line_ren->count)
			line_ren->count++;
	}

	if (line_ren->count > 1 && (line_ren->start == line_ren->end)) {
		line_ren->end = line_ren->current;
	}

SET_PROPS:
	line_ren->max_padding =
		sprintf(line_number, "%u", line_ren->current->y + 1) + 1;
	// if (line_ren->max_padding < 1) line_ren->max_padding++;
	line_ren->current = line_ren->start;
	if (Stream)
		fclose(Stream);
	return line_ren->count;
}

int save_file(char *file_path, Line *lines, bool release) {
	FILE *Stream = fopen(file_path, "w+");
	int bytes_saved = 0;
	Line *next = NULL;
	Line *current = lines;

	for (; current;) {
		bytes_saved += fwrite(current->content, 1, current->size, Stream);
		bytes_saved += fwrite("\n", 1, 1, Stream);
		next = current->next;
		if (release) {
			free(current->content);
			free(current);
		}
		current = next;
	}

	fclose(Stream);
	return bytes_saved;
}

void editor_dl(Line *line) {
	// NORMAL del.
	if (line->x > 0) {
		memmove(line->content + line->x, line->content + line->x + 1,
		  line->size - line->x);

		line->x--;
		line->size--;
		line->content[line->size] = 0;
		return;
	}
}

void editor_backspace(Lines_renderer *line_ren) {
	// NORMAL backspacing.
	if (line_ren->current->x > 0) {
		memmove(line_ren->current->content + line_ren->current->x - 1,
		  line_ren->current->content + line_ren->current->x,
		  line_ren->current->size - line_ren->current->x);

		line_ren->current->content[line_ren->current->size] = 0;
		line_ren->current->x--;
		line_ren->current->size--;

		return;
	}

	// if the cursor is on the first col of a line and we need to shift lines and
	// data  backward
	if (line_ren->current->x == 0 && line_ren->current->prev) {
		if (line_ren->current->size) {
			// move all the data locateed in the curr line to the end of the prev
			// line!
			memmove(line_ren->current->prev->content + line_ren->current->prev->size,
		   line_ren->current->content, line_ren->current->size);

			line_ren->current->prev->x = line_ren->current->prev->size;
			line_ren->current->prev->size += line_ren->current->size;
		}

		line_disconnect_from_ren(line_ren);
	}

	// Don't uncomment this line, it is a fucking bug..
	// line_ren->current->content[line_ren->current->x] = 0;
}

void line_disconnect_from_ren(Lines_renderer *line_ren) {
	line_ren->current = disconnect_line(line_ren->current); // go back one line.
	line_ren->count--;
	if (line_ren->end->next != NULL) {
		line_ren->end = (line_ren->end->next);
	}
}

// isalnum,  isalpha, isascii, isblank, iscntrl, isdigit, isgraph, islower,
// isprint, ispunct, isspace, isupper, isxdigit, isalnum_l, isalpha_l,
// isascii_l, isblank_l, iscntrl_l, isdigit_l, isgraph_l, islower_l, isprint_l,
// ispunct_l, isspace_l, isupper_l, isxdigit_l  -  character  classification
// functions.

void editor_new_line(Lines_renderer *line_ren, bool reset_borders) {
	Line *new, *next;
	char line_number[LINE_NUM_MAX] = {0};
	int new_max;
	new = Alloc_line_node(line_ren->current->y + 1);
	new_max = sprintf(line_number, "%u", line_ren->current->y + 1) + 1;

	if (new_max > line_ren->max_padding)
		line_ren->max_padding = new_max;

	if (!(line_ren->current->next) &&
		line_ren->current->x == line_ren->current->size) {
		line_ren->current->next = new;
		new->prev = line_ren->current;
		line_ren->current = new;
		if (reset_borders)
			line_ren->end = new;

		if (line_ren->current->y - line_ren->start->y >
			line_ren->win_h - MENU_HEIGHT_ &&
			reset_borders) {
			line_ren->start = line_ren->start->next;
		}

		line_ren->count++;
		return;
	}

	if (line_ren->current->next &&
		line_ren->current->x == line_ren->current->size) {
		next = line_ren->current->next;
		new->prev = line_ren->current; // correct
		new->next = next;              // correct.
		line_ren->current->next = new; // correct
		next->prev = new;

		lines_shift(new->next, 1);

		// BUG: in this case the end changes incorrectly.
		if (line_ren->current == line_ren->end &&
			line_ren->end->y - line_ren->start->y >
			line_ren->win_h - MENU_HEIGHT_ &&
			reset_borders) {
			line_ren->end = new;
			line_ren->start = line_ren->start->next;
		}

		if (line_ren->end->y - line_ren->start->y >
			line_ren->win_h - MENU_HEIGHT_ &&
			reset_borders) {
			line_ren->end = line_ren->end->prev;
		}

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
		memmove(new->content, line_ren->current->content + line_ren->current->x,
		  new->size);
		lines_shift(new->next, 1);

		// BUG: in this case the end changes incorrectly.
		if (line_ren->current == line_ren->end &&
			line_ren->end->y - line_ren->start->y >
			line_ren->win_h - MENU_HEIGHT_ &&
			reset_borders) {
			line_ren->end = new;
			line_ren->start = line_ren->start->next;
		}

		if (line_ren->end->y - line_ren->start->y >
			line_ren->win_h - MENU_HEIGHT_ &&
			reset_borders) {
			line_ren->end = line_ren->end->prev;
		}
	} else {
		line_ren->current->next = new;
		new->prev = line_ren->current;
		new->size = line_ren->current->size - line_ren->current->x;
		line_ren->current->size -= new->size;

		memmove(new->content, line_ren->current->content + line_ren->current->x,
		  new->size);

		line_ren->end = new;
		lines_shift(new->next, 1);
		if (line_ren->end->y - line_ren->start->y >
			line_ren->win_h - MENU_HEIGHT_ &&
			reset_borders) {
			line_ren->start = line_ren->start->next;
		}
	}

	// if (line_ren->end->y - line_ren->start->y >= line_ren->win_h - MENU_HEIGHT_
	// - 1 && reset_borders) {
	//     line_ren->start = line_ren->start->next;
	//     line_ren->end   = line_ren->end->next;
	// }

	memset(line_ren->current->content + line_ren->current->x, 0, new->size);
	line_ren->current = line_ren->current->next;
	line_ren->count++;
}

static void highlight__(int y, int x, int size) {
	mvchgat(y,    // Which line
		 x,    // Which col to start from
		 size, // where or howmuch to highlight after x.. which is position x +
		 // size
		 A_NORMAL, HIGHLIGHT_VMODE_PAIR, NULL);
}

int highlight_until_current_col(Vec2 start, Lines_renderer *line_ren) {
	int highlight_count = 0;
	int x = start.x, y = start.y;
	Line *current = line_ren->current;
	Line *line = NULL;

	if (y - line_ren->start->y == current->y - line_ren->start->y) {
		if (current->x > x) {
			highlight__(y - line_ren->start->y, x + line_ren->max_padding,
			   current->x - x);

			highlight_count = current->x - x;
			return highlight_count;
		}
		highlight__(y - line_ren->start->y, current->x + line_ren->max_padding,
			  x - current->x);
		highlight_count = (x - current->x + 1);
		return highlight_count;
	}

	if (y < current->y) {

		// Highligh current.
		highlight__(y - line_ren->start->y, x + line_ren->max_padding,
			  (start._line)->size + 1);

		highlight_count += (start._line)->size;
		// iterate thro all the lines and highlight them until the current line!
		line = (start._line)->next;
		while (line != current) {
			highlight__(line->y - line_ren->start->y, line_ren->max_padding,
			   (line)->size + 1);
			line = line->next;
			highlight_count += (line)->size;
		}

		highlight__(line->y - line_ren->start->y, 0 + line_ren->max_padding,
			  line->x);
		highlight_count += (line)->x;
		return highlight_count;
	}

	if (y > current->y) {

		// Highligh current.
		highlight__(y - line_ren->start->y, line_ren->max_padding, x);

		highlight_count += current->size;
		// iterate thro all the lines and highlight them until the current line!
		line = (start._line)->prev;
		while (line != current) {
			highlight__(line->y - line_ren->start->y, 0 + line_ren->max_padding,
			   (line)->size + 1);
			line = line->prev;
			highlight_count += (line)->size;
		}

		highlight__(line->y - line_ren->start->y, line->x + line_ren->max_padding,
			  line->size - line->x);

		highlight_count += line->size - line->x;
		return highlight_count;
	}

	return highlight_count;
}

static void get_string_chunk(Chunk *c, char *s, int start, int end, int size) {
	int n = (size - start) - (size - end);
	if (n) {
		for (int it = start; it < end; ++it)
			chunk_append_char(c, s[it]);
	}
}

static int cut_data(char *buffer, int cstart, int cend, int size) {
	memmove(buffer + cstart, buffer + cend, size - cend);
	return (cend - cstart);
}

Line *cut_line(Lines_renderer *line_ren, Line *line, size_t start, size_t end) {
	int n = cut_data(line->content, start, end, line->size);
	Line *local_line = line;
	bool is_origin = (line_ren->origin == line);
	bool is_start = (line_ren->start == line);
	bool is_end = (line_ren->end == line);

	local_line->size = local_line->size - n;
	local_line->x = start;

	if (is_origin && is_start) {
		// We save the next line after the first line then we switch the start to
		// it..
		if (is_end) {
			memset(local_line->content, 0x0, LINE_SZ);
			memset(local_line->token_list._list, 0x0, sizeof(BladeToken) * MAX_TOKENS);
			local_line->size = 0;
			local_line->x = 0;
			return local_line;
		}

		local_line = disconnect_line(line);

		// NOTE: This function fails for some reason so we need to cache the next
		// lne.
		if (local_line == NULL) {
			fprintf(stderr, "Error cutting some line,");
			exit(1);
		}

		line_ren->origin = local_line;
		line_ren->start = local_line;
		line_ren->current = local_line;

		if (line_ren->end->next)
			line_ren->end = line_ren->end->next;
		line_ren->count--;
		return local_line;
	}

	if (local_line->size == 0) {
		local_line = disconnect_line(line);
		line_ren->current = local_line;
		if (is_start && is_end) {
			line_ren->start = local_line;
			line_ren->end = local_line;
		} else {
			if (is_start) {
				line_ren->start = local_line;
				if (line_ren->end->next)
					line_ren->end = line_ren->end->next;
			} else if (is_end) {
				line_ren->end = local_line;
			} else {
				if (line_ren->end->next)
					line_ren->end = line_ren->end->next;
			}
		}
	}

	// local_line->x = start;
	line_ren->count--;
	return local_line;
}

void clipboard_cut_chunk(Lines_renderer *line_ren, Vec2 start, Vec2 end) {
	Vec2 temp = {.x = start.x, .y = start.y, ._line = start._line};
	Line *curr = NULL;
	Chunk *chunk = chunk_new();

	if (start.y == end.y) { // We need to copy one line!
		curr = start._line;
		int start_idx = MIN(start.x, end.x);
		int end_idx = MAX(start.x, end.x);
		get_string_chunk(chunk, curr->content, start_idx, end_idx, curr->size);
		cut_line(line_ren, curr, start.x, end.x);
		goto SET_AND_EX;
	}

	if (start.y > end.y) {
		start = end;
		end = temp;
	}

	curr = start._line;
	get_string_chunk(chunk, curr->content, start.x, curr->size, curr->size);

	chunk_append_char(chunk, '\n');

	curr = cut_line(line_ren, curr, start.x, curr->size);

	while (curr != end._line) {
		get_string_chunk(chunk, curr->content, 0, curr->size, curr->size);

		curr = cut_line(line_ren, curr, 0, curr->size);
		chunk_append_char(chunk, '\n');
	}

	if (curr) {
		get_string_chunk(chunk, curr->content, 0, end.x, curr->size);

		cut_line(line_ren, curr, 0, end.x);
		chunk_append_char(chunk, '\n');
	}

	// Return to the position in which we started copying.
	// line_ren->current->x = start.x;
SET_AND_EX:
	CLIPBOARD_SET(chunk->data);
	free(chunk);
}

void clipboard_save_chunk(Vec2 start, Vec2 end) {
	Vec2 temp = {.x = start.x, .y = start.y, ._line = start._line};
	Line *curr = NULL;
	Chunk *chunk = chunk_new();
	// Use this to cut the current line. Line *disconnect_line(Line *head)

	if (start.y == end.y) { // We need to copy one line!
		curr = start._line;
		int start_idx = MIN(start.x, end.x);
		int end_idx = MAX(start.x, end.x);
		get_string_chunk(chunk, curr->content, start_idx, end_idx, curr->size);
		goto SET_AND_EX;
	}

	if (start.y > end.y) {
		start = end;
		end = temp;
	}

	curr = start._line;

	get_string_chunk(chunk, curr->content, start.x, curr->size, curr->size);

	chunk_append_char(chunk, '\n');
	curr = curr->next;

	while (curr != end._line) {

		get_string_chunk(chunk, curr->content, 0, curr->size, curr->size);
		chunk_append_char(chunk, '\n');
		curr = curr->next;
	}

	if (curr) {
		get_string_chunk(chunk, curr->content, 0, end.x, curr->size);
		chunk_append_char(chunk, '\n');
	}

SET_AND_EX:
	CLIPBOARD_SET(chunk->data);
	free(chunk);
}
