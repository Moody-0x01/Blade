#include "Array.h"
#include <blade.h>
#include <string.h>

array_t *alloc_array(size_t cap, size_t element_size)
{
	array_t *self = NULL;
	if (!cap || !element_size)
		return (self);
	self = malloc(sizeof(*self));
	if (!self)
		return (NULL);
	self->data = malloc(element_size * cap);
	if (!self->data) {
		fprintf(stderr, "Hey, buy ram man. lol");
		free(self);
		return (NULL);
	}
	memset(self->data, 0x0, element_size * cap);
	self->cap  = cap;
	self->size = 0;
	self->_each = element_size;
	return (self);
}

void distroy_array(array_t *self)
{
	if (self) {
		if (self->data)
			free(self->data);
		free(self);
	}
}

void grow_array(array_t *self) {
	if (self->size >= self->cap) {
		self->cap  *= 2;
		self->data = realloc(self->data, self->cap * self->_each);
	}
}
void memclean_array(array_t *self)
{
	memset_array(self, 0x0);
	self->size = 0;
}

void memset_array(array_t *self, int byte)
{
	if (self) {
		memset(self->data, byte, self->cap * self->_each);
	}
}

void append_array(array_t *self, int data)
{
	// TODO: We can only assume u r using integfers, for now.
	grow_array(self);
	int *a = self->data;
	a[self->size] = data;
	self->size++;
}
