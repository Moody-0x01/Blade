#include <blade.h>
Path *path_alloc(int cap) {
	Path *p = malloc(sizeof(*p));

	memset(p, 0, sizeof(*p));
	p->items = malloc(sizeof(char *) * cap);
	memset(p->items, 0, sizeof(p->items[0]) * cap);
	p->count = 0;
	p->capacity = cap;
	p->type = NOT_EXIST;

	return p;
}

void parse_path_internal(Path *p, char *buffer, char *delim) {
	char *bcopy = string_dup(buffer);
	char *token = strtok(bcopy, delim);

	while (token != NULL) {
		if (p->count == p->capacity - 1) {
			p->items = realloc(p->items, sizeof(char *) * p->capacity * 2);
			memset((p->items + p->capacity - 1), 0,
		  sizeof(char *) * (p->capacity * 2));
			p->capacity *= 2;
		}

		p->items[p->count++] = string_dup(token);
		token = strtok(NULL, delim);
	}

	p->items[p->count] = NULL;
	free(bcopy);
}

void parse_path(Path *p, char *path) {
	size_t sz = strlen(path);
	parse_path_internal(p, path, "/");
	p->type = FILE__;

	if ((path[sz - 1]) == '/') {
		p->type = DIR__;
	}
}

void editor_make_apply_path_tree(Path *p) {
	// ./x/y/z => . x y w, 4, file
	char pathBuff[PATH_MAX] = {0};
	size_t j = 1;

	strcpy(pathBuff, *p->items);

	if (!(strcmp(pathBuff, "..") == 0) && !(strcmp(pathBuff, ".") == 0)) {
		if (p->count > 1)
			make_dir(pathBuff);
	}

	for (; j < (size_t)p->count - 1; ++j) {
		strcat(pathBuff, "/");
		strcat(pathBuff, p->items[j]);
		make_dir(pathBuff);
	}

	if (p->count > 1) {
		strcat(pathBuff, "/");
		strcat(pathBuff, p->items[j]);
	}

	if (p->type == DIR__) {
		make_dir(pathBuff);
		return;
	}

	FILE *fp = fopen(pathBuff, "a");
	fclose(fp);
}

void pprint(Path *p) {
	printf("Count -> %i\n", p->count);
	printf("Capa  -> %i\n", p->capacity);
	printf("Type  -> %s\n", (p->type == FILE__) ? "FILE" : "DIR");

	for (int i = 0; i < p->count; ++i) {
		printf("[%i] %s\n", i, (p->items[i] == NULL) ? "(null)" : p->items[i]);
	}
}

void release_path(Path *p) {
	if (!p)
		return;
	for (int i = 0; i < p->count; ++i) {
		free(p->items[i]);
	}
	free(p->items);
	free(p);
}
