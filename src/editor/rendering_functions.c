#include "colors.h"
#include "emojis.h"
#include "filessystem.h"
#include <blade.h>
#include <stdio.h>
#include <string.h>

// Possible modes in the editor!
static char *modes[] = {"NORMAL     ", "VISUAL     ", "INSERT     ",
	"FILEBROWSER", "COMMAND    ", "MPLAYER"};

char *get_modeascstr(editorMode mode) {
	if (mode <= MPLAYER) {
		return modes[mode];
	}
	return "NONE";
}

static int evenize(const char *s) {
	int length = strlen(s);
	return (length % 2) ? length : length + 1;
}

void editor_render_details(Lines_renderer *line_ren, char *_path,
						   editorMode mode_, char *notification) {
	char details_buffer[LINE_SZ] = {0};
	memset(details_buffer, 0, LINE_SZ);
	char *mode = get_modeascstr(mode_);

	// THE COMMAND BAR
	mvchgat(line_ren->win_h - 2, 0, line_ren->win_w, A_NORMAL, COMMAND_PROMPT_PAIR,
		 NULL);
	switch (mode_) {
		case FILEBROWSER: {
			char *User = getenv("USER");

			sprintf(details_buffer, "#%s %s", User, _path);
			// Display the mode.
			mvprintw(line_ren->win_h - 1, 0, " %s ", mode);
			mvchgat(line_ren->win_h - 1, 0, strlen(mode) + 2, A_NORMAL, BLADE_MODE_PAIR,
		   NULL);

			// Display notification.
			mvprintw(line_ren->win_h - 1, strlen(mode) + 3, " %s", notification);

			// Display details.
			mvprintw(line_ren->win_h - 1, line_ren->win_w - strlen(details_buffer) - 1,
			"%s", details_buffer);
			mvchgat(line_ren->win_h - 1, strlen(mode) + 2, line_ren->win_w, A_NORMAL,
		   DETAILS_BAR_PAIR, NULL);
		} break;

		case COMMAND: {
			char *User = getenv("USER");

			sprintf(details_buffer, "#%s %s", User, _path);
			// Display the mode.
			mvprintw(line_ren->win_h - 1, 0, " %s ", mode);
			mvchgat(line_ren->win_h - 1, 0, strlen(mode) + 2, A_NORMAL, BLADE_MODE_PAIR,
		   NULL);

			// Display notification.
			if (*notification)
				mvprintw(line_ren->win_h - 1, strlen(mode) + 3, " %s", notification);

			// Display details.
			mvprintw(line_ren->win_h - 1, line_ren->win_w - strlen(details_buffer) - 1,
			"%s", details_buffer);
			mvchgat(line_ren->win_h - 1, strlen(mode) + 2, line_ren->win_w, A_NORMAL,
		   DETAILS_BAR_PAIR, NULL);
		} break;
		default: {
			sprintf(details_buffer, "(%d, %d)[%d]", line_ren->current->y + 1,
		   line_ren->current->x + 1, line_ren->count);
			mvprintw(line_ren->win_h - 1, 0, " %s ", mode);
			mvchgat(line_ren->win_h - 1, 0, strlen(mode) + 2, A_NORMAL, BLADE_MODE_PAIR,
		   NULL);
			mvprintw(line_ren->win_h - 1, strlen(mode) + 3, " %s %s", _path,
			notification);
			mvprintw(line_ren->win_h - 1, line_ren->win_w - strlen(details_buffer) - 1,
			"%s", details_buffer);
			mvchgat(line_ren->win_h - 1, strlen(mode) + 2, line_ren->win_w, A_NORMAL,
		   DETAILS_BAR_PAIR, NULL);
			move(line_ren->current->y, line_ren->current->x);
		};
	}
}

static int editor_render_help(int x, int y, char *error) {
	// render the welcome window in the middle of the screen.
	char *prompt = "Enter the path to the file that u want to edit: ";
	char *save_p = "[ F1 ] to save the edited file";
	char *quit_p = "[ F2 ] to quit without saving";

	attron(A_STANDOUT);
	mvprintw(y, x - evenize(WLCM_BUFF) / 2, WLCM_BUFF);
	attroff(A_STANDOUT);
	mvprintw(y + 3, x - evenize(save_p) / 2, "%s", save_p);
	mvprintw(y + 5, x - evenize(quit_p) / 2, "%s", quit_p);

	if (error) {
		mvprintw(y + 7, x - evenize(error) / 2, "%s", error);
		mvchgat(y + 7, x - evenize(error) / 2, strlen(error), A_NORMAL, ERROR_PAIR,
		  NULL);
	}

	attron(A_UNDERLINE);
	int ret = mvprintw(y * 2, 0, "%s", prompt);
	while (ret == ERR) {
		ret = mvprintw(y * 2 - 1, 0, "%s", prompt);
	}
	attroff(A_UNDERLINE);

	return (strlen(prompt));
}

char *editor_render_startup(int x, int y, size_t width) {
	int prompt_offset = editor_render_help(x, y, NULL);
	Result *res = NULL;
	char *file_path = (calloc(1, LINE_SZ));
	// log_into_f("x: %i, y: %i", x, y);

	while (true) {
		res = make_prompt_buffer(prompt_offset, y * 2, width);
		switch (res->type) {
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

// EDITOR RENDERER.
void editor_render(BladeEditor *E) {
	// Check exit signal
	if (E->exit_pressed) {
		return;
	}

	// Check if a key was delted.
	if (E->char_deleted) {
		delch();
		E->char_deleted = false;
	}

	if (E->mode != MPLAYER) {
		erase();
	}
	switch (E->mode) {
		case FILEBROWSER: {
			render_file_browser(E);
		} break;
		case VISUAL: {
			render_lines(E->renderer);
			E->highlighted_data_length =
				highlight_until_current_col(E->highlighted_start, E->renderer);
			// sprintf(E->notification_buffer, "(*) %d bytes were selected\n",
			// E->highlighted_data_length);
		} break;
		case NORMAL: {
			if (E->binding_queue.size == MAX_KEY_BINDIND) {
				// TODO: Process Key E->binding_queue.
				editor_handle_binding(E->renderer, &E->binding_queue);
				memset(E->binding_queue.keys, 0, E->binding_queue.size);
				E->binding_queue.size = 0;
			}
			if (E->fb->type != DIR__) {
				render_lines(E->renderer);
			} else {
				render_file_browser(E);
			}
		} break;
		case MPLAYER: {
			sprintf(E->notification_buffer, "PLAYING %s", E->mplayer->file);
			editor_render_details(E->renderer, E->fb->open_entry_path, E->mode,
						 E->notification_buffer);
			refresh();
			if (strlen(E->notification_buffer) > 0)
				memset(E->notification_buffer, 0, 1024);
			return;
		} break;
		default: {
			render_lines(E->renderer);
		} break;
	}
	// render..
	// E->notification_buffer = strcat(E->notification_buffer, );
	editor_render_details(E->renderer, E->fb->open_entry_path, E->mode,
					   E->notification_buffer);
	if (strlen(E->notification_buffer) > 0)
		memset(E->notification_buffer, 0, 1024);
	if (E->mode == FILEBROWSER || E->mode == MPLAYER)
		return;
	editor_apply_move(E->renderer);
	refresh();
}

// LINE RENDERER<>
static void token_highlight(BladeToken *token, int y, int y_offset, int x_offset) {
	switch (token->kind) {
		case C_UNION:
		case C_STRUCT:
		case C_ENUM:
		case C_TYPEDEF: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			SPECIAL_TOKEN);
		} break;
		case TYPE: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			CHI_GONG_RED_PAIR);
		} break;
		case KEYWORD: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			YELLOW_PAIR);
		} break;
		case C_INCLUDE_FILE: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			BRIGHT_YELLOW_PAIR);
		} break;
		case C_INCLUDE: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			BRIGHT_GREEN_PAIR);
		} break;
		case C_TAG: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			TAG_PAIR);
		} break;
		case STR_LIT: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			STRING_LIT_PAIR);
		} break;
		case CALL: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			CALL_SYNTAX_PAIR);

		} break;
		case COMMENT: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			COMENT_PAIR);
		} break;
		case _GENERIC_NULL: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			NUM_PAIR);
		} break;
		case NUMBER_LIT: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			NUM_PAIR);
		} break;
		default: {
			colorize(y - y_offset, token->xstart + x_offset, token->xend + x_offset,
			MAIN_THEME_PAIR);
		} break;
	}
}

static void add_syntax_(Line *current, Lines_renderer *line_ren) {
	// retokenize the line..
	FileType file_type = line_ren->file_type;
	if (file_type == UNSUP)
		return; // Make sure that the script is supported..
	retokenize_line(current, file_type);
	for (int it = 0; it < (current->token_list).size; ++it) {
		token_highlight(&(current->token_list._list[it]), current->y,
				  line_ren->start->y, line_ren->max_padding);
	}
}

static void render_line(Line *line, int offset, int max_padding) {
	int x = 0, n = 0;
	char line_number[LINE_NUM_MAX] = {0};
	line->padding = sprintf(line_number, "%u", line->y + 1);

	for (n = 0; n < line->padding; n++) {
		mvaddch(line->y - offset, n, line_number[n]);
	}
	// TODO: COLORIZE THE LINE NUMBER WITH A BETTER COLOR. (line->y - offset, 0,
	// max_padding, PAIR, NULL);
	for (; n < max_padding; n++) {
		mvaddch(line->y - offset, n, ' ');
	}

	for (x = 0; x < line->size; ++x) {
		mvaddch(line->y - offset, x + n, line->content[x]);
	}
}

void render_lines(Lines_renderer *line_ren) {
	Line *current = line_ren->start;

	while (current) {
		render_line(current, line_ren->start->y, line_ren->max_padding);
		add_syntax_(current, line_ren);
		if (current == line_ren->end)
			break;
		current = current->next;
	}
}

// FILE BROWSER
void render_file_browser(BladeEditor *E) {
	erase();
	curs_set(0);

	size_t xpadding = FILE_BROWSER_XPADDING;
	size_t ypadding = FILE_BROWSER_YPADDING;
	size_t row = E->fb->cur_row;
	Emoji *emoji = NULL;

	if (E->fb->type == DIR__) {
		// TODO: Render only what is visible and not out of bound
		for (size_t y = E->fb->start; y <= E->fb->end; y++) {
			xpadding = FILE_BROWSER_XPADDING ;
			BrowseEntry entry = E->fb->entries[y];
			int length = strlen(entry.value);
			switch (entry.etype) {
				// TODO: render a file emoji if it is a file, otherwise render a folder
				// emoji.
				case DIR__: {
					emoji = emoji_get(E_FOLDER);
				} break;
				default: {
					switch (entry.ftype) {
						case C: {
							emoji = emoji_get(E_C_FILE);
						} break;
						case PYTHON: {
							emoji = emoji_get(E_PYFILE);
						} break;
						case MP3: {
							emoji = emoji_get(E_MUSIC);
						} break;
						default: {
							emoji = emoji_get(E_FILE);
						};
					}
				};
			}

			// NOTE: latest possible col = y + ypadding + ``
			// Render the size of each entry.
			mvprintw(y + ypadding - E->fb->start, xpadding, "%3s ", emoji->decoded);
			// colorize the icon.
			if (entry.etype == FILE__)
				colorize(y + ypadding - E->fb->start, xpadding, emoji->size,
			 CALL_SYNTAX_PAIR);
			else
				colorize(y + ypadding - E->fb->start, xpadding, emoji->size,
			 YELLOW_PAIR);
			xpadding += emoji->size;
			mvprintw(y + ypadding - E->fb->start, xpadding, "%s", entry.value);
			if (entry.selected) {
				emoji = emoji_get(E_CHECK);
				mvprintw(y + ypadding - E->fb->start, xpadding + length, " %3s", emoji->decoded);  
				colorize(y + ypadding - E->fb->start, xpadding, emoji->size,
			 CALL_SYNTAX_PAIR);
				length += emoji->size + 1;
			}
			if (row == y) {
				colorize(row + ypadding - E->fb->start, xpadding, length,
			 HIGHLIGHT_FB_PAIR);
			}
		}
	}
}
