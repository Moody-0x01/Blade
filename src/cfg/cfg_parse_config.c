#include <blade.h>
#include <linux/limits.h>
// #include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static Line *cfg_tokenize_config(char *file)
{
	Line *line; // Wraps the data
	int stream;

	if (!file)
		return (NULL);
	line = Alloc_line_node(0);
	free(line->content);
	stream = open(file, O_RDONLY);
	line->content = read_file(stream);
	line->size = strlen(line->content);
	retokenize_line(line, UNSUP);
	close(stream);
	return (line);
}

EditorConfig_t *cfg_parse_config(char *file)
{
	EditorConfig_t *cfg;
	Line *content;

	cfg = alloc_default_cfg();
	cfg->cfg_path = string_dup(file);
	content = cfg_tokenize_config(file);
	for (int index = 0; index < content->token_list.size; index++)
		printf("[KIND: %s, DATA: %s]\n", get_token_kind_s(content->token_list._list[index].kind), content->token_list._list[index].data);
	printf("\n");
	return (cfg);
}

// EditorConfig_t *load_editor_config(char *file)
// {
// 	// EditorConfig_t *cfg;
// 	// bool stx_err = false;
// 	// BladeTokenType sep_kind;
// 	// char *lhs, *rhs;
// 	// int stream;
// 	// Line *line;
// 	// TokenList *tokens;
// 	// int i = 0;
// 	// if (!file)
// 	// 	return NULL;
// 	// cfg = alloc_default_cfg();
// 	// cfg->cfg_path = string_dup(file);
// 	// stream = open(cfg->cfg_path, O_RDONLY);
// 	// if (stream < 0) {
// 	// 	free(cfg);
// 	// 	return (NULL);
// 	// }
// 	//
// 	// line = Alloc_line_node(0);
// 	// free(line->content);
// 	// for (char *next = read_next_line(stream); next;
// 	// (next = read_next_line(stream)), i++) {
// 	// 	char *escd;
// 	// 	line->content = next;
// 	// 	line->size = strlen(next);
// 	// 	retokenize_line(line, UNSUP);
// 	// 	tokens = &line->token_list;
// 	//
// 	// 	rhs = tokens->_list[0].data;
// 	// 	if (tokens->_list[0].kind == HASHTAG || !*rhs)
// 	// 		continue;
// 	// 	sep_kind = tokens->_list[1].kind;
// 	// 	if (sep_kind != COLON && sep_kind != EQ) {
// 	// 		stx_err = true;
// 	// 		fprintf(
// 	// 			stderr,
// 	// 			"INVALID SYNTAX: there should have been `=` or `:` after %s token\n",
// 	// 			rhs);
// 	// 		break;
// 	// 	}
// 	// 	lhs = tokens->_list[2].data;
// 	// 	if (strcmp(rhs, "autosave") == 0) {
// 	// 		cfg->autosave = atoi(lhs);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "indent_char") == 0) {
// 	// 		assert(*lhs == '"');
// 	// 		assert(*(lhs + 1) != 0);
// 	// 		escd = xescape(lhs);
// 	// 		cfg->indent_char = *(escd + 1);
// 	// 		// printf("%x\n", cfg->indent_char);
// 	// 		free(escd);
// 	// 		// getc(stdin);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "indent_count") == 0) {
// 	// 		cfg->indent_count = atoi(lhs);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "background") == 0) {
// 	// 		cfg->theme.background = (int)np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	//
// 	// 	if (strcmp(rhs, "foreground") == 0) {
// 	// 		cfg->theme.foreground = (int)np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	//
// 	// 	if (strcmp(rhs, "keyword_color") == 0) {
// 	// 		cfg->theme.keyword_color = (int)np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "type_color") == 0) {
// 	// 		cfg->theme.type_color = (int)np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "funcall_color") == 0) {
// 	// 		cfg->theme.funcall_color = (int)np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "special_token_color") == 0) {
// 	// 		cfg->theme.special_token_color = (int)np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "string_lit_color") == 0) {
// 	// 		cfg->theme.string_lit_color = (int)np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "comment_color") == 0) {
// 	// 		cfg->theme.comment_color = (int)np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "highlight_bg_fb") == 0) {
// 	// 		cfg->theme.highlight_bg_fb = np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "highlight_fg_fb") == 0) {
// 	// 		cfg->theme.highlight_fg_fb = np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "highlight_fg_vmode") == 0) {
// 	// 		cfg->theme.highlight_fg_vmode = np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "highlight_bg_vmode") == 0) {
// 	// 		cfg->theme.highlight_bg_vmode = np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "details_bar_bg") == 0) {
// 	// 		cfg->theme.details_bar_bg = np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "details_bar_fg") == 0) {
// 	// 		cfg->theme.details_bar_fg = np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "command_prompt_bg") == 0) {
// 	// 		cfg->theme.command_prompt_bg = np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "command_prompt_fg") == 0) {
// 	// 		cfg->theme.command_prompt_fg = np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "blade_mode_bg") == 0) {
// 	// 		cfg->theme.blade_mode_bg = np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// 	if (strcmp(rhs, "blade_mode_fg") == 0) {
// 	// 		cfg->theme.blade_mode_fg = np_atoi_base(lhs, NULL);
// 	// 		continue;
// 	// 	}
// 	// }
// 	//
// 	// close(stream);
// 	// free((line->token_list)._list);
// 	// free(line);
// 	// if (!stx_err)
// 	// 	return (cfg);
// 	// free(cfg->cfg_path);
// 	// free(cfg);
// 	// return NULL;
// }
