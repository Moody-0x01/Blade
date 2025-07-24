#include <blade.h>

void clipboard_add_line(char *line_content) {
	Chunk *c = chunk_new();
	chunk_append_char(c, NL);
	chunk_append_s(c, line_content);
	CLIPBOARD_SET(c->data);
	chunk_distroy(c);
}

void editor_handle_binding(Lines_renderer *line_ren, vKeyBindingQueue *bindings)
{
	editor_identify_binding(bindings);

	switch (bindings->kind)
	{
		case COPY_LINE: {
			clipboard_add_line(line_ren->current->content); 
		} break;

		case INDENT_LINE: {
			indent_line(line_ren->current); 
		} break;

		case UNINDENT_LINE: {
			unindent_line(line_ren->current);
		} break;

		case DEL_LINE: {
			int rcol = line_ren->current->x;
			clipboard_add_line(line_ren->current->content); 
			cut_line(line_ren, line_ren->current, 0, line_ren->current->size);
			if (rcol < line_ren->current->size) {
				line_ren->current->x = rcol;
			} else {
				line_ren->current->x = rcol;
			}
		} break;
		case NOT_VALID: {
		} break;
		default: {} break;
	}
}


void editor_identify_binding(vKeyBindingQueue *bindings)
{
	if (!bindings->size)
	{
		bindings->kind = NOT_VALID;
		return;
	}

	if (bindings->keys[0] == 'y' && bindings->keys[1] == 'y') { // Copy the current line into clipboard.
		bindings->kind = COPY_LINE;
		return;
	}

	if (bindings->keys[0] == 'd' && bindings->keys[1] == 'd') { // delete the current line into clipboard.
		bindings->kind = DEL_LINE;
		return;
	}

	if (bindings->keys[0] == '>' && bindings->keys[1] == '>') { // delete the current line into clipboard.
		bindings->kind = INDENT_LINE;
		return;
	}

	if (bindings->keys[0] == '<' && bindings->keys[1] == '<') { // delete the current line into clipboard.
		bindings->kind = UNINDENT_LINE;
		return;
	}

	bindings->kind = NOT_VALID;
	return;
}

