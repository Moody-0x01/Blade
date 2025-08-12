#include <blade.h>

void construct_punct_token(Line *line, FileType file_type, int *x)
{
	switch (line->content[*x])
	{
		case '=': {
			token_list_append(&(line->token_list), EQ, *x, *x);
			(*x)++;
		} break;

		case '>': {
			token_list_append(&(line->token_list), GT, *x, *x);
			(*x)++;
		} break;
		case '<': {
			if (file_type == C) { // gather an include source.
				int start = *x;

				while (*x < line->size && line->content[*x] != '>') {
					(*x)++;
				}

				if (line->content[*x] == '>') {
					token_list_append(&(line->token_list), C_INCLUDE_FILE, start, *x);
					(*x)++;
					return ;
				}
				*x = start;
			}
			token_list_append(&(line->token_list), LT, *x, *x);
			(*x)++;
		} break;

		case '*': {
			if (line->token_list.size > 0 && file_type == C) {
				int type_index = line->token_list.size - 1;
				if (line->token_list._list[type_index].kind == ID) {
					line->token_list._list[type_index].kind = TYPE;
				}
			}

			token_list_append(&(line->token_list), AST, *x, *x);
			(*x)++;
		} break;

		case '&': {
			token_list_append(&(line->token_list), AND, *x, *x);
			(*x)++;
		} break;

		case '|': {
			token_list_append(&(line->token_list), PIPE, *x, *x);
			(*x)++;
		} break;

		case '#': {
			if (file_type == PYTHON)
			{
				token_list_append(&(line->token_list), COMMENT, *x, line->size);
				*x = line->size;
				return;
			}
			token_list_append(&(line->token_list), HASHTAG, *x, *x);
			(*x)++;
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

			token_list_append(&(line->token_list), OPAR_, *x, *x);
			(*x)++;
		} break;

		case CPAR: {
			token_list_append(&(line->token_list), CPAR_, *x, *x);
			(*x)++;
		} break;

		case OCERLY: {
			token_list_append(&(line->token_list), OCERLY_, *x, *x);
			(*x)++;
		} break;

		case CCERLY: {
			token_list_append(&(line->token_list), CCERLY_, *x, *x);
			(*x)++;
		} break;

		case OBRAC: {
			token_list_append(&(line->token_list), OBRAC_, *x, *x);
			(*x)++;
		} break;

		case CBRAC: {
			token_list_append(&(line->token_list), CBRAC_, *x, *x);
			(*x)++;
		} break;

		case ',': {
			token_list_append(&(line->token_list), COMA, *x, *x);
			(*x)++;
		} break;

		case '.': {
			token_list_append(&(line->token_list), PERIOD, *x, *x);
			(*x)++;
		} break;

		case '$': {
			token_list_append(&(line->token_list), DOLLARSIGN, *x, *x);
			(*x)++;
		} break;

		case '@': {
			token_list_append(&(line->token_list), AT, *x, *x);
			(*x)++;
		} break;

		case '!': {
			token_list_append(&(line->token_list), BANG, *x, *x);
			(*x)++;
		} break;

		case '+': {
			token_list_append(&(line->token_list), PLUS, *x, *x);
			(*x)++;
		} break;

		case '-': {
			token_list_append(&(line->token_list), MINUS, *x, *x);
			(*x)++;
		} break;

		case ';': {
			token_list_append(&(line->token_list), SEMICOLON, *x, *x);
			(*x)++;
		} break;
		case ':': {
			token_list_append(&(line->token_list), COLON, *x, *x);
			// printf("COLON FOUND:\n");
			(*x)++;
		} break;

		case '/': {
			if ((*x + 1 < line->size)) {
				if ((file_type == C || file_type == GO || file_type == JS) &&
					line->content[*x + 1] == '/') {
					token_list_append(&(line->token_list), COMMENT, *x, line->size);
					*x = line->size;
					return;
				}
			}
			token_list_append(&(line->token_list), FSLASH, *x, *x);
			(*x)++;
		} break;

		case '\\': {
			token_list_append(&(line->token_list), BSLASH, *x, *x);
			(*x)++;
		} break;
		default: {
			token_list_append(&(line->token_list), OTHER_PUNCT, *x, *x);
			(*x)++;
		}
	}

}

void construct_identifier(Line *line, FileType file_type, int *x, KeywordList *keywords_list)
{
	int data_idx = 0;
	int start, end;
	char temp[512] = {0};

	start = (*x);
	temp[data_idx++] = line->content[(*x)++];
	while ((isalnum(line->content[*x]) || (line->content[*x] == '_')) &&
		*x < line->size) {
		temp[data_idx++] = line->content[(*x)++];
	}
	end = (*x - 1);
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

				line->token_list._list[line->token_list.size - 1].xend = end;
				return ;
			}
		}
		if (!strcmp(temp, "enum")) {
			token_list_append(&(line->token_list), C_ENUM, start, end);
			return ;
		}

		if (!strcmp(temp, "typedef")) {
			token_list_append(&(line->token_list), C_TYPEDEF, start, end);
			return ;
		}

		if (!strcmp(temp, "struct")) {
			token_list_append(&(line->token_list), C_STRUCT, start, end);
			return ;
		}

		if (!strcmp(temp, "union")) {
			token_list_append(&(line->token_list), C_UNION, start, end);
			return ;
		}

		if (!strcmp(temp, "NULL")) {
			token_list_append(&(line->token_list), _GENERIC_NULL, start, end);
			return ;
		}
	}

	if (file_type == GO) {
		if (!strcmp(temp, "nil")) {
			token_list_append(&(line->token_list), _GENERIC_NULL, start, end);
			return ;
		}
	}
	if (file_type == UNSUP) {
		token_list_append(
			&(line->token_list),
			ID,
			start, end);
	} else {
		token_list_append(
			&(line->token_list),
			(is_keywrd(keywords_list->_list, temp, keywords_list->size)) ? KEYWORD : ID,
			start, end);
	}
	line->token_list._list[line->token_list.size - 1].data = 
		string_dup(temp);
	// IDENTIFY TYPES FOR C.
	// TODO: Instead of this way I should define a bunch of rules and then decide what is that token based on these rules...
	if (file_type == C) 
	{
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
}
