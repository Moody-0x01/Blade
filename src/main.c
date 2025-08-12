// #include "parser.h"
// #include <cstdio>
#include <fcntl.h>
#include <linux/limits.h>
#include <blade.h>
#include <stdio.h>
#define INIT_SCRIPT_PATH "init.mi"
static char *temp = NULL;
// TODO: Implement -v/--version flags and --help
void editor_help(char *program) {
	printf("\nusage => %s <file/directory>\n", program);
	printf("flags:\n");
	printf("-h, --help    => Prints the help message into stdout.\n");
	printf("-v, --version => Prints the version of the editor.\n\n");
}

bool check_args(int argc, char **argv) {
	char *program = *argv;
	bool flag = true;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
			printf(MI_V);
			flag = false;
		}
		if (!strcmp(argv[i], "-h") || !strcmp(argv[1], "--help")) {
			editor_help(program);
			flag = false;

		}
		if (!strcmp(argv[i], "-c") || !strcmp(argv[1], "--cfg")) {
			if (i + 1 < argc) {
				temp = argv[i + 1];
				flag = true;
			} else {
				fprintf(stderr, "-c || --cfg was specified but no file path was specified.");
				flag = false;
			}
		}
	}

	return flag;
}

#define EXP

#ifndef EXP
	int main(int argc, char **argv) {
		int ret = 0;
		if (check_args(argc, argv)) {
			ret = editor(argv, temp);
			CLIPBOARD_FREE();
			// THIS IS BECAUSE THE EDITOR SCREWS UP THE TERMINAL.
			if (!ret)
				system("reset");
		}
		return ret;
	}
#else
	int main()
	{
		cfg_parse_config("/home/lazmoud/.config/blade/blade.cfg");
		return (0);
	}
#endif
