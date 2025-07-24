#ifndef ARRAY
#define ARRAY
#include <stdint.h>
#include <stdlib.h>
typedef struct Array {
	size_t cap, size;
	size_t _each; // Private that indecates the size of each element
	void *data;
} array_t;
void append_array(array_t *self, int data);
void grow_array(array_t *self);
array_t *alloc_array(size_t cap, size_t element_size);
void distroy_array(array_t *self);
void memset_array(array_t *self, int byte);
void memclean_array(array_t *self);
#endif // ARRAY
