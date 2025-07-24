#include <blade.h>

static void parse_ecommand_internal(eCommand *c, char *buffer)
{
	char *bcopy = NULL, *token = NULL, *delim = " \n\t";
	if (!buffer || !c) return;

	bcopy = string_dup(buffer);
	token = strtok(bcopy, delim);
	c->name = string_dup(token);
	token = strtok(NULL, delim);

	while (token != NULL)
	{
		if (c->size == c->cap - 1)
		{
			c->argv = realloc(c->argv, sizeof(char *) * c->cap * 2);
			memset((c->argv + c->cap - 1), 0, sizeof(char *) * (c->cap * 2));
			c->cap *= 2;
		}

		c->argv[c->size++] = string_dup(token);
		token             = strtok(NULL, delim);
	}

	c->argv[c->size] = NULL;
	free(bcopy);
}


eCommand *command_alloc(size_t cap) {
	eCommand *c = malloc(sizeof(*c));
	memset(c, 0, sizeof(*c));
	c->argv = malloc(sizeof(char *) * cap);
	memset(c->argv, 0, sizeof(*c->argv) * cap);
	c->cap  = cap;
	c->size = 0;
	return (c);
}

void command_distroy(eCommand *c) {
	if (c == NULL) return;

	for (size_t i = 0; i < c->size; ++i) {
		free(c->argv[i]);
	}

	free(c->argv);
	free(c->name);
	free(c);
}


eCommand *command_parse(char *command) {
	// assert(command != NULL);
	eCommand *c = NULL;
	if (!command || !*command) return c;
	c = command_alloc(INIT_COMMAND_CAP);
	// TODO: Implement this one! to parse commands, take a buff = "name argv1 argv2 argv3"
	// then return this struct { name -> "name", argv -> {"argv1", "argv2", "argv3"}, .size -> 3, .cap -> 25 }
	parse_ecommand_internal(c, command);
	return c;
}
