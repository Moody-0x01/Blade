#include <blade.h>

static int file_cmp(const void *ap, const void *bp)
{
	const char *a = *(const char**)ap;
	const char *b = *(const char**)bp;
	return strcmp(a, b);
}

char **read_entire_dir(const char *dir_path)
{
	char **files = (char **) malloc(sizeof(char *) * FB_MAX_ENT);
	int cap = FB_MAX_ENT;
	int it = 0;
	DIR *dir = NULL;
	int dir_name_size = 0;

	dir = opendir(dir_path);
	if (dir == NULL) {
		free(files);
		return NULL;
	}

	errno = 0;
	struct dirent *ent = readdir(dir);

	for (;ent != NULL; ++it) {

		if (it >= cap - 1) {
			cap *= 2;
			files = realloc(files, sizeof(char *) * cap);
		}

		dir_name_size =  strlen(ent->d_name);
		files[it] = malloc(dir_name_size + 1);
		files[it] = strcpy(files[it], ent->d_name);
		// da_append(files, temp_strdup());
		ent = readdir(dir);
	}

	files[it] = NULL;
	qsort(files, it, sizeof(*files), file_cmp);
	if (errno != 0) {
		free(files);
		return NULL;
	}

	if (dir) closedir(dir);
	return files;
}
