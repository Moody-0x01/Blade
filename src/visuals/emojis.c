#include <blade.h>
#include <stdint.h>

// Emojis.
static uint32_t emojis_[] = {
	[E_FOLDER] = (uint32_t)(L''),
	[E_FILE] = (uint32_t)(L'󰈚'),
	[E_C_FILE] = (uint32_t)(L''),
	[E_PYFILE] = (uint32_t)(L''),
	[E_CHECK] = (uint32_t)(L''),
	[E_MUSIC] = (uint32_t)L'󰽰'
};

Emoji *emoji_pool(EmojiPoolAction a, EmojiKind kind) {
	static Emoji emojis[EMOJI_CAP] = {0};

	switch (a) {
		case INIT: {
			(emojis + E_FOLDER)->size =
				decode_utf8(emojis_[E_FOLDER], (emojis + E_FOLDER)->decoded);
			(emojis + E_FILE)->size =
				decode_utf8(emojis_[E_FILE], (emojis + E_FILE)->decoded);
			(emojis + E_C_FILE)->size =
				decode_utf8(emojis_[E_C_FILE], (emojis + E_C_FILE)->decoded);
			(emojis + E_PYFILE)->size =
				decode_utf8(emojis_[E_PYFILE], (emojis + E_PYFILE)->decoded);
			(emojis + E_CHECK)->size =
				decode_utf8(emojis_[E_CHECK], (emojis + E_CHECK)->decoded);
			(emojis + E_MUSIC)->size =
				decode_utf8(emojis_[E_MUSIC], (emojis + E_MUSIC)->decoded);
		} break;
		case GET: {
			if (kind < E_COUNT) {
				return emojis + kind;
			} else {
				return NULL;
			}
		} break;
		default: {
		} break;
	}

	return NULL;
}
