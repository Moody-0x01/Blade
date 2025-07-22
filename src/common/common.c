// #include "logger.h"
#include <blade.h>
#include <ncurses.h>
#include <string.h>

size_t fsize(FILE *stream) {
  long size = 0;
  fseek(stream, 0, SEEK_END);
  size = ftell(stream);
  rewind(stream);
  return (size_t)size;
}

bool file_exists(const char *fpath) {
  struct stat sb;
  return (stat(fpath, &sb) == 0) && (S_ISREG(sb.st_mode));
}

bool dir_exists(const char *folder) {
  struct stat sb;
  return (stat(folder, &sb) == 0) && (S_ISDIR(sb.st_mode));
}

int min(int a, int b) { return (a > b) ? b : a; }

int max(int a, int b) { return (a < b) ? b : a; }

bool isprintable(char c) { return isalnum(c) || ispunct(c) || isspace(c); }
char *string_dup(char *str) {
  char *s = calloc(1, strlen(str) + 1);
  size_t i = 0;

  while (str[i]) {
    s[i] = str[i];
    i++;
  }

  s[i] = 0;
  return s;
}

int decode_utf8(uint32_t utf8_bytes, char *str) {
  printf("> DECODE 0x%x\n", utf8_bytes);
  if (utf8_bytes > 1114112 || (utf8_bytes >= 0xd800 && utf8_bytes <= 0xdfff)) {
    // encodes U+fffd; replacement character
    str[0] = (char)0xef;
    str[1] = (char)0xbf;
    str[2] = (char)0xbd;
    str[3] = 0;
    return 3;
  }

  // just like 7-bit ascii
  if (utf8_bytes < 128) {
    str[0] = utf8_bytes;
    str[1] = '\0';
    return 1;
  }

  uint32_t len = 0;
  if (utf8_bytes < 2048) {
    len = 2;
  } else {
    if (utf8_bytes < 65636) {
      len = 3;
    } else {
      len = 4;
    }
  }
  str[len] = '\0';

  // set the bits at the start to indicate number of bytes
  unsigned char m = TEC_MOST_SIG_BIT;
  uint32_t i = 0;
  str[0] = 0;
  while (i < len) {
    str[0] |= m;
    i += 1;
    m >>= 1;
  }

  // set the most significant bits in the other bytes
  i = 1;
  while (i < len) {
    str[i] = (char)TEC_MOST_SIG_BIT;
    i += 1;
  }

  // fill in the codepoint
  int j = len - 1;
  while (j >= 0) {
    m = 1;
    i = 0;
    while (utf8_bytes && i < 6) {
      if (utf8_bytes % 2) {
        str[j] |= m;
      }
      utf8_bytes >>= 1;
      m <<= 1;
      i += 1;
    }
    j -= 1;
  }

  return (3);
}

void string_clean(char *s) {
  size_t i = strlen(s);

  if (s == NULL)
    return;
  if (!*s)
    return;
  // remove spaces from the end.
  while (i > 1 && isspace(s[i - 1])) {
    s[i - 1] = 0x00;
    i--;
  }

  // remove spaces from the start.
  while (isspace(*s)) {
    s++;
  }
}

char *resolve_path(char *src, char *dest) {
  // TODO: Implement - command arg. so we can return to our prev dir.
  char *delim = "/";

  if (!src || !dest) {
    return NULL;
  }

  char *buffer = calloc(1, strlen(src) + strlen(dest) + 2);
  if (!strcmp(dest, "..")) {
    // TODO: remove one entry back. /include/x/y => /include/x
    // NOTE: if we are already at the root, we stop the movement! and we always
    // return the root.
    if (strcmp(src, "/") != 0) {
      size_t last = strlen(src) - 1;

      do {
        last--;
      } while (last > 0 && src[last] != *delim);
      if (last > 0) {
        if (src[last] == *delim)
          last--;
        buffer = strncpy(buffer, src, last + 1);
        return buffer;
      }
      // NOTE: here we know last == 0 so length is 1 and the first char is / so
      // we are the root. NOTE: this is why we fall thro to the case where we
      // return the root.
    }
    buffer = strcpy(buffer, src);
    return buffer;
  }

  if (!strcmp(dest, ".")) {
    // TODO: do nothing
    buffer = strcpy(buffer, src);
    return buffer;
  }

  buffer = strcpy(buffer, src);
  buffer = strcat(buffer, delim);
  buffer = strcat(buffer, dest);
  return (buffer);
}

int make_dir(char *path) {
  int status = 0;

  if (!path)
    return status;
  // NOTE: There are directories that are not makeable.
  if (strcmp(path, ".") == 0 || strcmp(path, "..") == 0 ||
      strcmp(path, "/") == 0)
    return status;
  if (dir_exists(path))
    return status;
  status = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  return status;
}

// int remove_dir(char *full_path)
// {
//     (void) full_path;
//     // TODO: traverse a directory and remove all its files and
//     subdirectoreis.
// }

Result *make_prompt_buffer(int x, int y, size_t w) {
  // (NOTE/BUG): This bug is kinda tricky to track..
  Result *result = malloc(sizeof(Result));
  bool deleted = false;
  result->data = malloc(LINE_SZ);
  memset(result->data, 0, LINE_SZ);

  result->type = SUCCESS;
  result->etype = NONE;

  int buffer_idx = 0;
  int size = 0, byte = 0;

  int ret = mvchgat(y, 0, w, A_NORMAL, COMMAND_PROMPT_PAIR, NULL);
  if (ret == ERR) {
    y -= 1;
    mvchgat(y, 0, w, A_NORMAL, COMMAND_PROMPT_PAIR, NULL);
  }

  move(y, x + buffer_idx);
  if (result == NULL || result->data == NULL)
    return NULL;
  while ((byte = getch())) {
    switch (byte) {
    case ESC: {
      result->type = ERROR;
      result->etype = EXIT_SIG;
      result->data = strcpy(result->data, "<killed>");
      return result;
    } break;
    case NL: {
      if (size == 0) {
        result->type = ERROR;
        result->etype = EMPTY_BUFF;
        result->data = strcpy(result->data, "<Empty>");
        return result;
      }
      result->type = OK;
      return result;
    } break;
    case KEY_BACKSPACE: {
      
      if (size && buffer_idx) {
        memmove(result->data + buffer_idx - 1, result->data + buffer_idx,
                size - buffer_idx);
        // memset(result->data + buffer_idx, 0x00, size - );
        
        result->data[size - 1] = 0x00;
        size--;
        buffer_idx--;
        if (!size && !buffer_idx) {
          mvdelch(y, x);
        } else {
          deleted = true;
        }
      }
    } break;
    case KEY_RIGHT: {
      if (buffer_idx < size)
        buffer_idx++;
    } break;
    case KEY_LEFT: {
      if (buffer_idx > 0)
        buffer_idx--;
    } break;
    case KEY_HOME: {
      buffer_idx = 0;
    } break;
    case KEY_END: {
      buffer_idx = size;
    } break;
    default: {
      if (isprintable(byte)) {
        memmove(result->data + buffer_idx + 1, result->data + buffer_idx,
                size - buffer_idx);

        result->data[buffer_idx++] = byte;
        size++;
      }
    } break;
    }
    // log_into_f(">> CURRENT: %s\n", result->data);
    
    if (size && *result->data) {
      for (int i = 0; i < size; ++i)
        mvaddch(y, x + i, result->data[i]);
    }

    if (deleted) {
      delch();
      deleted = false;
    }

    mvchgat(y, 0, w, A_NORMAL, COMMAND_PROMPT_PAIR, NULL);
    move(y, x + buffer_idx);
    refresh();
  }

  if (result->type == OK) {
    // NOTE: This call cleans up the buffer from any trilling or leading white
    // spaces,
    string_clean(result->data);
  }

  return result;
}
char *slurp_file_content(const char *path) {
  FILE *stream = NULL;
  if (!path)
    return NULL;
  stream = fopen(path, "r");

  if (!stream) {
    return NULL;
  }

  size_t size = fsize(stream);
  char *buffer = calloc(1, size);
  size_t n = fread(buffer, 1, size, stream);
  assert(size == n && "The read size is not equal to the expected size.");

  fclose(stream);
  return (buffer);
}

void set_cursor_style(int style)
{
    switch(style) {
        case BLOCK_BLINK: // Block blinking
            printf("\033[1 q");
            break;
        case BLOCK_STEADY: // Block steady
            printf("\033[2 q");
            break;
        case UNDR_BLINK: // Underline blinking
            printf("\033[3 q");
            break;
        case UNDR_STEADY: // Underline steady
            printf("\033[4 q");
            break;
        case BAR_BLINK: // Bar blinking (thin line)
            printf("\033[5 q");
            break;
        case BAR_STEADY: // Bar steady (thin line)
            printf("\033[6 q");
            break;
    }
    fflush(stdout);
}

void refresh_cursor(editorMode mode)
{
	curs_set(1);
	switch (mode)
	{
		case VISUAL: 
		case FILEBROWSER:
		case MPLAYER:
		case NORMAL: {
			set_cursor_style(BLOCK_STEADY);
		} break;
		case COMMAND:
		case INSERT: {
			set_cursor_style(BAR_STEADY);
		} break;
		default: {
			set_cursor_style(BLOCK_STEADY);
		};
	}
}
