#include "common.h"
#include "filessystem.h"
#include <blade.h>

// TODO: Make a collector for string lits, number, id tokens.. and also
// keywords....
char *get_token_kind_s(BladeTokenType t) {
	switch (t) {
		case STR_LIT:
			return "STR_LIT";
		case KEYWORD:
			return "KEYWORD";
		case NUMBER_LIT:
			return "NUMBER_LIT";
		case ID:
			return "ID";
		case STR_LIT_INCOM:
			return "STR_LIT_INCOM";
		case EQ:
			return "EQ";
		case GT:
			return "GT";
		case LT:
			return "LT";
		case AST:
			return "AST";
		case AND:
			return "AND";
		case PIPE:
			return "PIPE";
		case HASHTAG:
			return "HASHTAG";
		case OPAR_:
			return "OPAR_";
		case CPAR_:
			return "CPAR_";
		case OCERLY_:
			return "OCERLY_";
		case CCERLY_:
			return "CCERLY_";
		case OBRAC_:
			return "OBRAC_";
		case CBRAC_:
			return "CBRAC_";
		case COMA:
			return "COMA";
		case PERIOD:
			return "PERIOD";
		case DOLLARSIGN:
			return "DOLLARSIGN";
		case AT:
			return "AT";
		case BANG:
			return "BANG";
		case PLUS:
			return "PLUS";
		case MINUS:
			return "MINUS";
		case SEMICOLON:
			return "SEMICOLON";
		case COLON:
			return "COLON";
		case FSLASH:
			return "FSLASH";
		case BSLASH:
			return "BSLASH";
		case OTHER_PUNCT:
			return "OTHER_PUNCT";
		case CALL:
			return "CALL";
		case COMMENT:
			return "COMMENT";
		case C_TAG:
			return "C_TAG";
		case C_INCLUDE_FILE:
			return "C_INCLUDE_FILE";
		case C_ENUM:
			return "C_ENUM";
		case C_TYPEDEF:
			return "C_TYPEDEF";
		case C_STRUCT:
			return "C_STRUCT";
		case C_UNION:
			return "C_UNION";
		case TYPE:
			return "TYPE";
		default:
			return "UNKNOWN_TOKEN_KIND";
	}
}

void token_list_append(TokenList *list, BladeTokenType kind, int xstart,
					   int xend) {
	if (list->size >= list->cap) {
		list->cap *= 2;
		list->_list = realloc(list->_list, sizeof(list->_list[0]) * list->cap);
	}

	BladeToken *current_tok = (list->_list + list->size);
	current_tok->xend = xend;
	current_tok->xstart = xstart;
	current_tok->kind = kind;
	list->size++;
}

static int trim_spaces_left(char *buff, int curr) {
	int i = curr;
	while (isspace(buff[i]) && buff[i])
		i++;
	return i;
}

// Reinit the tokens.
static void reinit_line_toks(Line *line) {

	for (int i = 0; i < line->token_list.size; i++) {
		if (line->token_list._list[i].data)
			free(line->token_list._list[i].data);
	}
	if (line->token_list.size > 0) {
		line->token_list._list =
			memset(line->token_list._list, 0,
		  sizeof(line->token_list._list[0]) * line->token_list.cap);
		line->token_list.size = 0;
	}
}

static int collect_string_lit(Line *line, int current_x) {
	int xstart = current_x++;
	int xend;

	while (current_x < line->size) {
		switch (line->content[current_x]) {
			case '\'':
			case '\"': {
				xend = current_x;
				current_x++;
				goto NEXT;
			} break;
			case '\\': {
				if (current_x + 1 < line->size)
					current_x = current_x + 2;
			} break;
			default: {
				current_x++;
			} break;
		}
	}
	xend = current_x;
	current_x++;
NEXT:
	if (line->content[xend] == line->content[xstart]) {
		token_list_append(&(line->token_list), STR_LIT, xstart, xend);
	} else {
		token_list_append(&(line->token_list), STR_LIT_INCOM, xstart, xend);
	}

	line->token_list._list[line->token_list.size - 1].data =
		memxcpy(line->content + xstart, (xend - xstart) + 1);
	return current_x;
}

static int ishexdigit(char c) {
	return ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'));
}

static int collect_digit(Line *line, int current_x) {
	int xstart = current_x++;
	int xend;

	while (
		isdigit(line->content[current_x]) ||
		((line->content[current_x] == 'x' || line->content[current_x] == 'X'))) {

		if (line->content[current_x] == 'x' ||
			(line->content[current_x] == 'X' && (xstart - current_x == 1))) {
			// NOTE: (xstart - current_x == 1) means that x is the second char. so it
			// could be a potential hex number.
			// TODO: Collect the hex digits.
			// skip the x/X
			current_x++;
			while (isdigit(line->content[current_x]) ||
				ishexdigit(line->content[current_x])) {
				current_x++;
			}
			break;
		}

		current_x++;
	}

	xend = (current_x - 1);
	token_list_append(&(line->token_list), NUMBER_LIT, xstart, xend);
	line->token_list._list[line->token_list.size - 1].data =
		memxcpy(line->content + xstart, (xend - xstart) + 1);
	return current_x;
}

void retokenize_line(Line *line, FileType file_type) {
	char temp[512] = {0};
	int xend = 0, xstart = 0, data_idx = 0, x = 0;
	KeywordList *keywords_list = get_keywords_list(file_type);
	reinit_line_toks(line);

	for (; x < line->size;) {
		data_idx = 0;
		memset(temp, 0, 512);
		x = trim_spaces_left(line->content, x);
		xstart = x;

		// Collect a string lit.
		if (line->content[x] == '\"' || line->content[x] == '\'') {
			x = collect_string_lit(line, xstart);
			continue;
		}

		if (isdigit(line->content[x])) {
			x = collect_digit(line, xstart);
			continue;
		}

		// For ids, keywords and other syntaxes!
		if (isalpha(line->content[x]) || line->content[x] == '_') {
			temp[data_idx++] = line->content[x++];
			while ((isalnum(line->content[x]) || (line->content[x] == '_')) &&
				x < line->size) {
				xend = (x);
				temp[data_idx++] = line->content[x++];
			}

			if (file_type == C) {
				if (line->token_list.size) {
					if (line->token_list._list[line->token_list.size - 1].kind ==
						HASHTAG) {
						// TODO: Get the word and see the type of the c_tag is it an
						// #include or a condition #if #endif #ifndef,
						if (!strcmp(temp, "include")) { // #Include
							line->token_list._list[line->token_list.size - 1].kind =
								C_INCLUDE;
						} else {
							line->token_list._list[line->token_list.size - 1].kind =
								C_TAG; // #ifdef #define #...
						}

						line->token_list._list[line->token_list.size - 1].xend = xend;
						continue;
					}
				}

				if (!strcmp(temp, "enum")) {
					token_list_append(&(line->token_list), C_ENUM, xstart, xend);
					continue;
				}

				if (!strcmp(temp, "typedef")) {
					token_list_append(&(line->token_list), C_TYPEDEF, xstart, xend);
					continue;
				}

				if (!strcmp(temp, "struct")) {
					token_list_append(&(line->token_list), C_STRUCT, xstart, xend);
					continue;
				}

				if (!strcmp(temp, "union")) {
					token_list_append(&(line->token_list), C_UNION, xstart, xend);
					continue;
				}

				if (!strcmp(temp, "NULL")) {
					token_list_append(&(line->token_list), _GENERIC_NULL, xstart, xend);
					continue;
				}
			}

			if (file_type == GO) {
				if (!strcmp(temp, "nil")) {
					token_list_append(&(line->token_list), _GENERIC_NULL, xstart, xend);
					continue;
				}
			}
			if (file_type == UNSUP) {
				token_list_append(
					&(line->token_list),
					ID,
					xstart, xend);
			} else {
				token_list_append(
					&(line->token_list),
					(is_keywrd(keywords_list->_list, temp, keywords_list->size)) ? KEYWORD : ID,
					xstart, xend);
			}
			line->token_list._list[line->token_list.size - 1].data = 
				string_dup(temp);
			// IDENTIFY TYPES FOR C.
			if (file_type == C) {
				if (line->token_list.size > 1) {
					if (line->token_list._list[line->token_list.size - 1].kind == ID) {
						if (line->token_list._list[line->token_list.size - 2].kind ==
							C_STRUCT ||
							line->token_list._list[line->token_list.size - 2].kind ==
							C_ENUM ||
							line->token_list._list[line->token_list.size - 2].kind ==
							C_UNION ||
							line->token_list._list[line->token_list.size - 2].kind ==
							CCERLY_) {
							line->token_list._list[line->token_list.size - 1].kind = TYPE;

						} else if (line->token_list._list[line->token_list.size - 2].kind ==
							ID) {
							line->token_list._list[line->token_list.size - 2].kind = TYPE;
						}
					}
				}
			}

			continue;
		}

		if (ispunct(line->content[x])) {
			switch (line->content[x]) {
				case '=': {
					// ID ID EQ
					token_list_append(&(line->token_list), EQ, x, x);
					x++;
				} break;

				case '>': {
					token_list_append(&(line->token_list), GT, x, x);
					x++;
				} break;
				case '<': {
					if (file_type == C) { // gather an include source.
						xstart = x;

						while (x < line->size && line->content[x] != '>') {
							x++;
						}

						if (line->content[x] == '>') {
							token_list_append(&(line->token_list), C_INCLUDE_FILE, xstart, x);
							x++;
							continue;
						}
						x = xstart;
					}

					token_list_append(&(line->token_list), LT, x, x);
					x++;
				} break;

				case '*': {
					if (line->token_list.size > 0 && file_type == C) {
						int type_index = line->token_list.size - 1;
						if (line->token_list._list[type_index].kind == ID) {
							line->token_list._list[type_index].kind = TYPE;
						}
					}

					token_list_append(&(line->token_list), AST, x, x);
					x++;
				} break;

				case '&': {
					token_list_append(&(line->token_list), AND, x, x);
					x++;
				} break;

				case '|': {
					token_list_append(&(line->token_list), PIPE, x, x);
					x++;
				} break;

				case '#': {
					if (file_type == PYTHON) {
						token_list_append(&(line->token_list), COMMENT, x, line->size);
						return;
					}
					token_list_append(&(line->token_list), HASHTAG, x, x);
					x++;
				} break;

				case OPAR: {
					if (line->token_list.size > 0) {
						if (line->token_list._list[line->token_list.size - 1].kind == ID) {
							line->token_list._list[line->token_list.size - 1].kind = CALL;
						}

						if (file_type == C && line->token_list.size > 1) {
							int type_index = line->token_list.size - 2;
							if (line->token_list._list[type_index].kind == ID) {
								line->token_list._list[type_index].kind = TYPE;
							}
						}
					}

					token_list_append(&(line->token_list), OPAR_, x, x);
					x++;
				} break;

				case CPAR: {
					token_list_append(&(line->token_list), CPAR_, x, x);
					x++;
				} break;

				case OCERLY: {
					token_list_append(&(line->token_list), OCERLY_, x, x);
					x++;
				} break;

				case CCERLY: {
					token_list_append(&(line->token_list), CCERLY_, x, x);
					x++;
				} break;

				case OBRAC: {
					token_list_append(&(line->token_list), OBRAC_, x, x);
					x++;
				} break;

				case CBRAC: {
					token_list_append(&(line->token_list), CBRAC_, x, x);
					x++;
				} break;

				case ',': {
					token_list_append(&(line->token_list), COMA, x, x);
					x++;
				} break;

				case '.': {
					token_list_append(&(line->token_list), PERIOD, x, x);
					x++;
				} break;

				case '$': {
					token_list_append(&(line->token_list), DOLLARSIGN, x, x);
					x++;
				} break;

				case '@': {
					token_list_append(&(line->token_list), AT, x, x);
					x++;
				} break;

				case '!': {
					token_list_append(&(line->token_list), BANG, x, x);
					x++;
				} break;

				case '+': {
					token_list_append(&(line->token_list), PLUS, x, x);
					x++;
				} break;

				case '-': {
					token_list_append(&(line->token_list), MINUS, x, x);
					x++;
				} break;

				case ';': {
					token_list_append(&(line->token_list), SEMICOLON, x, x);
					x++;
				} break;
				case ':': {
					token_list_append(&(line->token_list), COLON, x, x);
					// printf("COLON FOUND:\n");
					x++;
				} break;

				case '/': {
					if ((x + 1 < line->size)) {
						if ((file_type == C || file_type == GO || file_type == JS) &&
							line->content[x + 1] == '/') {
							// WE Collect a comment.
							token_list_append(&(line->token_list), COMMENT, x, line->size);
							return;
						}
					}
					token_list_append(&(line->token_list), FSLASH, x, x);
					x++;
				} break;

				case '\\': {
					token_list_append(&(line->token_list), BSLASH, x, x);
					x++;
				} break;
				default: {
					token_list_append(&(line->token_list), OTHER_PUNCT, x, x);
					x++;
				}
			}
		}
	}
}

bool is_keywrd(char *keywords[], char *word, int keywords_sz) {

	for (int it = 0; it < keywords_sz; ++it) {
		if (strcmp(keywords[it], word) == 0) {
			return true;
		}
	}
	return false;
}
