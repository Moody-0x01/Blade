#include <blade.h>

Chunk *chunk_new(void)
{
	Chunk *c = malloc(sizeof(Chunk));
	c->size  = 0;
	c->lines = 0;
	c->cap   = LINE_SZ;
	c->data  = malloc(LINE_SZ); 
	memset(c->data, 0, LINE_SZ);
	return (c);
}

void chunk_append_char(Chunk *c, char chr)
{
	if (c->size == c->cap) {
		c->cap *= 2;
		c->data = realloc(c->data, c->cap);
		memset(c->data + c->size, 0, (c->cap - c->size));
	}

	c->data[c->size++] = chr;
	if (chr == NL) c->lines++;
}

void chunk_append_s(Chunk *c, char *str)
{
	for (; *str; str++)
		chunk_append_char(c, *str);
}

void chunk_distroy(Chunk *c)
{
	if (c) {
		if (c->data)
			free(c->data);

		free(c);
	}
}



