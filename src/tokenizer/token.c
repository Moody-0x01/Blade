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
		case NL_TOKEN:
			return "NEW_LINE";
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
	while (isspace(buff[i]) && buff[i] != NL)
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
	int x;
	KeywordList *keywords_list;

	x = 0;
	keywords_list = get_keywords_list(file_type);
	reinit_line_toks(line);
	for (; x < line->size;)
	{
		x = trim_spaces_left(line->content, x);

		// Collect a string lit.
		if (line->content[x] == '\"' || line->content[x] == '\'') {
			x = collect_string_lit(line, x);
			continue;
		}

		if (isdigit(line->content[x])) {
			x = collect_digit(line, x);
			continue;
		}

		// For ids, keywords and other syntaxes!
		if (isalpha(line->content[x]) || line->content[x] == '_') {
			construct_identifier(line, file_type, &x, keywords_list);
			continue;
		}
		if (ispunct(line->content[x]))
		{
			construct_punct_token(line, file_type, &x);
			continue ;
		}
		if (line->content[x] == NL)
			token_list_append(&(line->token_list), NL_TOKEN, x, x);
		else
			token_list_append(&(line->token_list), UNKNOWN_TOKEN_KIND, x, x);
		x++;
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
