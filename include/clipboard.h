#ifndef CLIPBOARD_H
#define CLIPBOARD_H

// clipboard api
typedef enum ClipBoardEvent {
	CFREE,
	CGET,
	CSET
} ClipBoardEvent;

void *clipboard(ClipBoardEvent e, char *data);
#define CLIPBOARD_SET(data) clipboard(CSET, data);
#define CLIPBOARD_GET() (Chunk *) clipboard(CGET, NULL);
#define CLIPBOARD_FREE() clipboard(CFREE, NULL);

#endif // CLIPBOARD_H
