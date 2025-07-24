#include <blade.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

char *xstrdup(char *s) {
	size_t size = strlen(s);
	char *dup = malloc(size + 1);
	size_t idx = 0;
	for (; idx < size; idx++) {
		dup[idx] = s[idx];
	}
	dup[idx] = 0;
	return (dup);
}

int xstr(const char *s1, const char *s2) {
	int i = 0;
	int j = 0;
	int flen = strlen(s1);

	for (;s2[i]; i++) {
		if (s2[i] == s1[0])
			break;
	}
	if (!*(s2 + i))
		return (1);

	for (;s2[i] && s1[j]; i++, j++) {
		if (s2[i] != s1[j])
			break;
	}
	if (j < flen)
		return (1);
	return (0);
}

char *memxcpy(char *src, int n)
{
	assert(n > 0 && "@memxcpy: N <= 0");
	char *dst = malloc(n + 1);
	int i = 0;
	while (src[i] && n) {
		dst[i] = src[i];
		n--;
		i++;
	}
	dst[i] = 0;
	return (dst);
}

// TODO: 
char *xescape(char *src) {
	if (src == NULL)
		return NULL;
	char *bp;
	char *dst = malloc(sizeof(src));
	if (dst == NULL)
		return dst;
	bp = dst;
	for (;*src != 0x0;src++, dst++) {

		if (*src == '\\') {
			if (*(src + 1)) {
				switch (*(1+src)) {
					case 't': {
						*dst = TAB;
						src++;
					} break;
				}
				continue;
			}
		}
		*dst = *src;
	}
	return bp;
}

