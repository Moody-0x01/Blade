#ifndef EMOJIS_H
#define EMOJIS_H
#define EMOJI_CAP 25
#define EMOJI_BYTE_CAP 5

#include <stdint.h>
#include <stdlib.h>
typedef enum EmojiKind {
	E_FOLDER = 0,
	E_FILE,
	E_C_FILE,
	E_PYFILE,
	E_CHECK,
	E_MUSIC,
	E_COUNT
} EmojiKind;

typedef enum EmojiPoolAction { INIT, GET } EmojiPoolAction;

typedef struct Emoji {
	uint32_t raw;
	char decoded[EMOJI_BYTE_CAP];
	size_t size;
} Emoji;

Emoji *emoji_pool(EmojiPoolAction a, EmojiKind kind);

#define emoji_init() emoji_pool(INIT, 0)
#define emoji_get(e) emoji_pool(GET, e)

#endif // EMOJIS_H
