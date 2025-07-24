#include <blade.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>

bool write_default_cfg(char *xdg_cfg_path) {
	int default_fd, fd;
	char *default_cfg;

	default_fd = open(DEFAULT_CFG_PATH, O_RDWR); 
	if (default_fd <= 0) {
		fprintf(stderr, "Could not open DEFAULT_CFG_PATH=`%s` to be read\n", DEFAULT_CFG_PATH);
		return false;
	}
	fd = open(xdg_cfg_path, O_RDWR | O_CREAT, S_IRWXU);
	if (fd <= 0) {
		fprintf(stderr, "Could not read XDG_CFG_PATH=`%s`\n", xdg_cfg_path);
		close(default_fd);
		return false;
	}
	default_cfg = read_file(default_fd);
	if (default_cfg == NULL) {
		fprintf(stderr, "Empty default config. please use a valid config in path `%s`\n", DEFAULT_CFG_PATH);
		close(fd);
		close(default_fd);
		return false;
	}

	write(fd, default_cfg, strlen(default_cfg));
	close(fd);
	close(default_fd);
	free(default_cfg);
	return true;
}

EditorConfig_t *alloc_default_cfg(void)
{
	EditorConfig_t *cfg = malloc(sizeof(*cfg));
	{
		// NOTE: the problem is that I need to always change this if 
		// I change the default config
		cfg->autosave =     1;
		cfg->indent_char =  ' ';
		cfg->indent_count = 6;
		// Theme.
		cfg->theme.background         = 0x1e232d;
		cfg->theme.foreground         = 0xffffff;
		cfg->theme.highlight_bg_fb    = 0xffffff;
		cfg->theme.highlight_fg_fb    = 0x000000;
		cfg->theme.highlight_fg_vmode = 0x000000;
		cfg->theme.highlight_bg_vmode = 0xffffff;
		cfg->theme.details_bar_bg     = 0xfca311;
		cfg->theme.details_bar_fg     = 0x000000;
		cfg->theme.command_prompt_bg  = 0x0d1e42;
		cfg->theme.command_prompt_fg  = 0x00ff9c;
		cfg->theme.blade_mode_bg      = 0x1a2c51;
		cfg->theme.blade_mode_fg      = 0xeeeeee;
		// Syntax ;3
		cfg->theme.keyword_color       = 0xf9ca24;
		cfg->theme.type_color          = 0xff7675;
		cfg->theme.funcall_color       = 0x00ffff;
		cfg->theme.special_token_color = 0x01a3a4;
		cfg->theme.string_lit_color    = 0x6ab04c;
		cfg->theme.comment_color       = 0x3f553c;
	}
	return cfg;
}
