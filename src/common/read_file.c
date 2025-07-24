#include <blade.h>

long lget_file_size(int Stream) 
{
	long size = lseek(Stream, 0L, SEEK_END);
	lseek(Stream, 0L, SEEK_SET);
	return size;
}

char *read_file(int fd) {
	long size = lget_file_size(fd);
	char *data = malloc(size + 1);
	read(fd, data, size);
	data[size] = 0;
	return (data);
}

char *read_next_line(int fd) {
	static char **lines = NULL;
	char *content = NULL;
	static int line_index = 0;
	if (line_index == 0 && content == NULL)
	{
		// TODO: implement the read file function.
		content = read_file(fd);
		lines = split(content, "\n", &(size_t){0});
		// NOTE: WE dont need it anymore.
		free(content);
		return (lines[line_index++]);
	}
	if (lines[line_index] == NULL)
	{
		// TODO: u r done. gotta free the mem.
		for (int i = 0; i < line_index; ++i)
			free(lines[i]);
		free(lines);
		return (NULL);
	}
	return (lines[line_index++]);
}
