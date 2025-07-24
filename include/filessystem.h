#ifndef FILE_S_H
#define FILE_S_H
#include <stdlib.h>
#define FILE_SYSTEM_CAP 256

typedef enum BrowseEntryT {  
	FILE__ = 0x0,
	DIR__,
	SYM_LINK__,
	NOT_EXIST
} BrowseEntryT;

typedef enum FileType {
	PYTHON = 0,
	C,
	JS,
	GO, 
	MP3,
	UNSUP
} FileType;

typedef struct filessystem_t filessystem_t;

typedef struct filessystem_t {
	BrowseEntryT  etype;      // FILE__, DIR__, NOT_EXIST.
	FileType      ftype;      // C, PYTHON, JS...
	size_t        ifs_size;   // Size of the file or the directory...
	size_t        ifs_cap;    // Size of the file or the directory...
	char          *full_path; // /usr/foo 
	char          *value;     // foo
	filessystem_t *parent;    // In case we want to go up the tree.
	filessystem_t **children; // if this filesystem_t is a directory. then we need to store the files and directories it holds here.
} filessystem_t;

char *join_path(char *left, char *right);
filessystem_t *slurp_fs(char *path);
void free_fs(filessystem_t *fs);
void print_fs(filessystem_t *fs, int depth);
void test_fs(char *path); 

#endif // FILE_S_H

