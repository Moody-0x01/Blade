// #include "filessystem.h"
#include <blade.h>
#include <ncurses.h>
#include <stdio.h>

static Vec2 vec2(void) { return (Vec2){.x = 0, .y = 0, ._line = NULL}; }

static void init_window(BladeEditor *E) {
	E->renderer->win_w = 0;
	E->renderer->win_h = 0;
	// NOTE: This bootstrap function maybe is neccessary for some terminals.
	while (E->renderer->win_w == 0 && E->renderer->win_h == 0) {

		E->ewindow = init_ncurses_window(E->cfg);
		editor_load_layout(E);
	}

}

BladeEditor *init_editor(char *path, EditorConfig_t *cfg) {
	BladeEditor *E = malloc(sizeof(BladeEditor));
	char *pathBuff = NULL;

	init_signals();
	memset(E, 0, sizeof(*E));
	E->cfg = cfg; 
	E->renderer = malloc(sizeof(*(E->renderer)));
	memset(E->renderer, 0, sizeof(*(E->renderer)));
	init_window(E);
	// If the caller did not supply a file then we ask him in a seperate screen.
	if (path == NULL) {
		pathBuff = editor_render_startup(
			E->renderer->win_w / 2, E->renderer->win_h / 2, E->renderer->win_w);
		if (pathBuff == NULL) {
			release_editor(E);
			return NULL;
		}
	} else {
		pathBuff = string_dup(path);
	}

	E->fb = new_file_browser(pathBuff, E->renderer->win_h);
	free(pathBuff);

	E->highlighted_end = vec2();   // x=0, Y=0
	E->highlighted_start = vec2(); // x=0, Y=0
	E->highlighted_data_length = 0;
	E->exit_pressed = false;
	E->char_deleted = false;
	E->mode = NORMAL;
	E->renderer->origin = Alloc_line_node(0);
	E->renderer->count = 0;
	E->renderer->start = E->renderer->origin;
	E->renderer->end = E->renderer->origin;
	E->renderer->current = E->renderer->origin;
	E->renderer->max_padding = 2;

	memset(E->notification_buffer, 0, 1024);
	// If the path that was passed was a file, or if it does not exist. we assign.
	if (E->fb->type == FILE__ || E->fb->type == NOT_EXIST) {
		E->renderer->file_type = get_file_type(E->fb->open_entry_path);
		if (E->renderer->file_type == MP3) {
			editor_init_player_routine(E, E->fb->open_entry_path);
			E->fb = realloc_fb(E->fb, "..", E->renderer->win_h);
			E->mode = FILEBROWSER;
		} else {
			load_file(E->fb->open_entry_path, E->renderer);
		}
	}

	// Init binding queue.
	E->binding_queue = (vKeyBindingQueue){0x0};
	if (E->fb->type == FILE__ || E->fb->type == NOT_EXIST) {
		editor_refresh(E);
	} else {
		E->mode = FILEBROWSER;
		render_file_browser(E);
		editor_render_details(E->renderer, E->fb->open_entry_path, E->mode,
						E->notification_buffer);
	}
	// set_cursor_style(BLOCK_STEADY);
	editor_register_(E);
	return (E);
}

BladeEditor *editor_register_(BladeEditor *E) {
	static BladeEditor *E_ = NULL;

	if (E != NULL && E_ == NULL) {
		E_ = E;
	}

	return E_;
}

BladeEditor *editor_get(void) { return editor_register_(NULL); }

void editor_load_layout(BladeEditor *E) {
	// Set the dimentions of the edittor..
	int h, w;
	getmaxyx(E->ewindow, h, w);
	E->renderer->win_h = h;
	E->renderer->win_w = w;
}

void release_editor(BladeEditor *E) {
	erase();
	if (!E)
		return;
	if (E->fb)
		release_fb(E->fb);
	if (E->renderer) {
		free_lines(E->renderer->origin);
		free(E->renderer);
	}
	release_cfg(E->cfg);
	free(E);
	endwin();
}

void editor_refresh(BladeEditor *E) {
	erase();
	render_lines(E->renderer);
	editor_render_details(E->renderer, E->fb->open_entry_path, E->mode,
					   E->notification_buffer);
	editor_apply_move(E->renderer);
	refresh();
}
// wdjiwdjiwjijdw
void editor_update(int c, BladeEditor *E) {
	// Globals.
	switch (c) {
		case ESC: {
			// set_cursor_style(BLOCK_STEADY);
			editor_left(E->renderer);
			E->mode = NORMAL;
			return;
		} break;
	}
	// Actions that depend on the mode.
	switch (E->mode) {
		case VISUAL: {
			editor_visual(c, E);
		} break;
		case NORMAL: {
			editor_normal(c, E);
		} break;
		case INSERT: {
			editor_insert(c, E);
		} break;
		default: {
		} break;
	}
}
