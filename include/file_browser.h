#ifndef FILE_B_H
#define FILE_B_H
// TODO: Use a file system instead of a file browser.
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <filessystem.h>
#include <Array.h>

#define FB_MAX_ENT 127
#define FILE_BROWSER_YPADDING 1
#define FILE_BROWSER_XPADDING FILE_BROWSER_YPADDING

typedef enum fb_command_e {
	MOVE = 0,
	COPY,
	FB_COMMAND_COUNT
} fb_command_t;


typedef struct BrowseEntry {
	bool selected; 
	BrowseEntryT etype;       // FILE__, DIR__, NOT_EXIST.
	FileType     ftype;       // C, PYTHON, JS...
	size_t       size;
	char         *full_path; // /usr/foo 
	char         *value;     // foo
} BrowseEntry;

// typedef struct FileBrowser FileBrowser;

typedef struct FileBrowser {
	size_t cap, size, cur_row, start, end;
	BrowseEntry *entries;
	BrowseEntryT type;
	char *open_entry_path; // current working path. it can the currrent open dir, or current open file.
	char *rel_path; // Not the Real path but the ralative one 
	array_t found;
} FileBrowser;

void rem_entry_from_list(BrowseEntry *list, size_t size, size_t i);
char *execute_fbsys_command(fb_command_t cmd, BrowseEntry src,
							BrowseEntry dst);
BrowseEntryT get_entry_type(char *path);
char **read_entire_dir(const char *dir_path);
void get_entry_info(BrowseEntry *e);
FileBrowser *new_file_browser(const char *dir_path, size_t window_height);
char *entry_type_as_cstr(BrowseEntryT T);
void release_fb(FileBrowser *fb);
FileBrowser *realloc_fb(FileBrowser *fb, char *next, size_t window_height);
void fb_append(FileBrowser *self, char *name);
bool fb_exists(FileBrowser *self, char *item);
void reinit_fb_bounds(FileBrowser *fb, size_t window_height);
void remove_entry(FileBrowser *fb);
void remove_entry_by_index(FileBrowser *fb, size_t index);
void fix_layout_file_browser(FileBrowser *fb, size_t window_height);
void fb_handle_mv(int c, FileBrowser *fb, size_t win_h);
#endif //FILE_B_H


