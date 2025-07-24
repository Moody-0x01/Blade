#include <blade.h>

void do_resize(int resize_signal)
{
	(void) resize_signal;
	BladeEditor *E = editor_get();
	// Well, since functions that get the width and height r not that good and just return win->_maxy or win->_maxx
	// then I actually should reinitialize ncurses before redrawing the editor again lol.
	// NOTE: This feature is not completed yet so dont expect much for now.

	endwin();
	E->ewindow = init_ncurses_window(E->cfg);
	// recompute the layout, width and height!
	editor_load_layout(E);
	// re-init the file browser..    
	if (E->fb->type == DIR__) 
		reinit_fb_bounds(E->fb, E->renderer->win_h);

	sprintf(E->notification_buffer, "resize detected!");
	E->resized = 1;
	// re-render the editor.
	// refresh();
}

static void init_ctrl_signals(void) {
	struct termios term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ICANON | ECHO);
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;
	term.c_cc[VSTOP] = _POSIX_VDISABLE;
	term.c_cc[VSUSP] = _POSIX_VDISABLE;

	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
/*
void init_resize_sig() {
	struct sigaction sa, priorsa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = &do_resize;
	sigaction(SIGWINCH, &sa, &priorsa);
}
*/
void init_signals(void) 
{

	// Disable Ctrl-C.
	signal(SIGINT, SIG_IGN); 
	// enable Ctrl-Z. Ctrl-S
	init_ctrl_signals();
	// Configure a resize function
	// init_resize_sig();
}

