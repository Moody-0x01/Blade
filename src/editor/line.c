#include <blade.h>

void indent_line(Line *line) {
	int x = line->x;
	line->x = 0;
	editor_tabs(line);
	line->x += x;
}

void unindent_line(Line *line) {
	int x = line->x;
	EditorConfig_t *cfg = cfg_interface(CFG_GET, NULL); 
	line->x = 0;

	while (line->content[line->x] == cfg->indent_char && line->x < cfg->indent_count) {
		line->x++;
	}

	memmove(line->content, line->content + line->x, line->size - line->x);

	line->size -= line->x;
	line->x = (x - line->x);
}

void indent_lines(Line *start, Line *end) {
	Line *tmp = end;
	if (start->y > end->y) {
		end = start;
		start = tmp;
	}

	while (true) {
		indent_line(start);
		if (start == end)
			break;
		start = start->next;
	}
}

void unindent_lines(Line *start, Line *end) {

	Line *tmp = end;
	if (start->y > end->y) {
		end = start;
		start = tmp;
	}

	while (true) {
		unindent_line(start);
		if (start == end)
			break;
		start = start->next;
	}
}

Line *Alloc_line_node(int row) {
	Line *line = (Line *)malloc(sizeof(Line));

	line->next = NULL;
	line->prev = NULL;
	line->x = 0;
	line->size = 0;
	line->cap = LINE_SZ;
	line->y = row;
	line->content = malloc(LINE_SZ);
	memset(line->content, 0x0, LINE_SZ);

	line->token_list = (TokenList){0};
	(line->token_list)._list = malloc(sizeof(BladeToken) * MAX_TOKENS);
	memset(line->token_list._list, 0x0, sizeof(BladeToken) * MAX_TOKENS);
	(line->token_list).cap = MAX_TOKENS;
	return (line);
}

void lines_shift(Line *head, int num) {

	Line *curr = head;
	for (; curr;) {
		curr->y += num;
		curr = curr->next;
	}
}

void free_lines(Line *lines) {
	Line *next = NULL;
	Line *current = lines;

	for (; (current);) {
		next = current->next;
		free(current->content);
		free((current->token_list)._list);
		free(current);
		current = next;
	}
}

Line *disconnect_line(Line *head) {
	Line *current = head;
	Line *prev = current->prev;
	Line *next = current->next;

	if (!prev && !next) {
		return current;
	}

	if (prev)
		prev->next = next;

	if (next)
		next->prev = prev;

	free(current->content);
	free(current);
	lines_shift(next, -1);
	if (next)
		return (next);
	if (prev)
		return (prev);
	return (NULL);
}

void char_inject(Line *line, char c) {
	if (isprintable(c)) {
		memmove((line->content + line->x + 1), (line->content + line->x),
		  line->size - line->x);
		line->content[line->x++] = c;
		line->size++;
	}
}

void line_push_char(Line *line, char c, bool pasted) {
	if (line->size + 1 == line->cap) {
		line->cap *= 2;
		line->content = realloc(line->content, line->cap);
	}

	char_inject(line, c);
	if (pasted)
		return;

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
		default: {
		} break;
	}
}

void editor_tabs(Line *line) {
	EditorConfig_t *cfg = cfg_interface(CFG_GET, NULL); 
	assert(cfg != NULL && "NO CONFIG WAS REGISTERED");
	for (int i = 0; i < cfg->indent_count; ++i)
		line_push_char(line, cfg->indent_char, true);
}

// TODO: Finish the impl of the next Functions... L178, L194
static void transform_chars(Vec2 start, Vec2 end, int (*f)(int)) {
	Line *line = start._line;
	Vec2 tmp = start; // Does that copy?

	if (start.y > end.y) {
		start = end;
		end = tmp;
		line = end._line;
	}

	while (true) {
		size_t istart = 0;
		size_t iend = line->size;

		if (line == start._line) {
			istart = start.x;
		}

		if (line == end._line) {
			iend = end.x;
		}

		if (end.y == start.y) {
			istart = MIN(start.x, end.x);
			iend = MAX(start.x, end.x);
		}

		for (; istart < iend && istart < (size_t)line->size; istart++)
			line->content[istart] = f(line->content[istart]);

		if (line == end._line)
			break;
		line = line->next;
	}
}

void uncapitalize_region(Vec2 start, Vec2 end) {
	transform_chars(start, end, tolower);
}

void capitalize_region(Vec2 start, Vec2 end) {
	transform_chars(start, end, toupper);
}
