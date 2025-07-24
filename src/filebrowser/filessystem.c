#include <blade.h>

char *join_path(char *left, char *right) {
	int ls = strlen(left), rs = strlen(right);
	char *joined = malloc(ls + rs + 2);
	memset(joined, 0, ls + rs + 2);
	joined = strcpy(joined, left);

	if (left[ls - 1] != '/')  {
		joined = strcat(joined, "/");
	}

	joined = strcat(joined, right);
	return (joined);
}

filessystem_t *slurp_fs(char *path)
{
	filessystem_t *fs = NULL;
	char **entries = NULL;
	char *tmp      = NULL;

	if(path == NULL)
		return (fs);

	fs            = malloc(sizeof(*fs));
	fs->parent    = NULL;
	memset(fs, 0, sizeof(*fs));
	fs->full_path = realpath(path, NULL);
	fs->value     = string_dup(path);
	fs->etype     = get_entry_type(fs->full_path);

	if (fs->etype == FILE__) {
		fs->ftype = get_file_type(fs->value);
		return (fs);
	}

	if (fs->full_path == NULL) {
		fs->full_path = string_dup(path);
		fs->etype     = SYM_LINK__;
		return (fs);
	}

	entries = read_entire_dir(fs->full_path);

	if (entries == NULL) {
		return (fs);
	}

	fs->children = malloc(sizeof(*fs) * FILE_SYSTEM_CAP);
	fs->ifs_cap  = FILE_SYSTEM_CAP;

	memset(fs->children, 0, FILE_SYSTEM_CAP * sizeof(fs->children));
	for (int i = 0; entries[i] != NULL; i++) {
		// > Resolve the path. [DONE]
		// > if the path is for a directory u call 
		// the current func on it and store the result in fs->ifs[i];
		// > if it is a file then u just add it normally.
		if (strcmp(entries[i], ".") && strcmp(entries[i], "..")) {
			if ((size_t)fs->ifs_size >= fs->ifs_cap - 1) {
				fs->ifs_cap  = (fs->ifs_cap * 2) + 1;
				fs->children = realloc(fs->children,  fs->ifs_cap * sizeof(*fs));
			}

			tmp = join_path(fs->full_path, entries[i]);
			fs->children[fs->ifs_size] = slurp_fs(tmp);
			fs->children[fs->ifs_size]->parent = fs;
			// > retrieve the name of the entry.
			free(fs->children[fs->ifs_size]->value);
			fs->children[fs->ifs_size++]->value = string_dup(entries[i]);
			free(tmp);
		}

		free(entries[i]);
	} 

	free(entries);
	fs->children[fs->ifs_size] = NULL;
	return (fs);
}

void free_fs(filessystem_t *fs) {
	if (!fs) return;

	if (fs->etype == DIR__) {
		// TODO: Make an algorithm to free the strcuture.
		for (size_t i = 0; (i < fs->ifs_size) && (fs->children[i] != NULL); i++) {
			free_fs(fs->children[i]);
		}
		free(fs->children);
	}

	free(fs->value);
	free(fs->full_path);
	free(fs);
}

void print_fs(filessystem_t *fs, int depth)
{
	// TODO: Make a func that prints a tree of the directories struct.
	if (!fs) return;

	if (fs->parent == NULL)
		printf("ROOT: %s\n", fs->full_path);

	if (fs->etype == FILE__) {
		for (int j = 0; j < depth; ++j)
			printf(" ");

		printf("{ FILE } %s\n", (fs->value));
		return;
	}

	for (int j = 0; j < depth; ++j)
		printf(" ");
	// full_path -> /foo/bar
	// value     -> bar
	printf("{DIR} %s %zu\n", fs->value, fs->ifs_size);
	if (fs->ifs_size && fs->children) {
		for (size_t i = 0; (i < fs->ifs_size) && (fs->children[i] != NULL); i++) {
			print_fs(fs->children[i], depth + 2);
		}
	}
}

void test_fs(char *path) {
	filessystem_t *fs = slurp_fs(path);
	// > testing would be making an ls functions.
	// > a rm fucntion that deletes a dir tree from the tail to the head.
	if (fs != NULL) {
		print_fs(fs, 1);
		free_fs(fs);
	} else {
		printf("There was a problem loading the file system at: %s\n", path);
	}
}

