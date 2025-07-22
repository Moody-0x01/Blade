#include <common.h>
#include <blade.h>
// #include <ncurses.h>
// #include <stdio.h>

int editor(char **argv, const char *cfg_path) {
  setlocale(LC_CTYPE, "");  
  EditorConfig_t *cfg = editor_resolve_cfg(cfg_path);
  if (!cfg) {
    return 1;
  }
  emoji_init(); // Init the emojis cache.
  BladeEditor *E = init_editor(argv[1], cfg);
  cfg_interface(CFG_SET, cfg); 

  int c = 0;
  if (E == NULL) {
    goto EXIT_AND_RELEASE_RESOURCES;
  }
  while (true) {
	refresh_cursor(E->mode);
    // NOTE: Dont remove halfdelay because it is essencial for some signal
    // interrupts, sometimes u need to wait for something to happen before
    // updating.
    // halfdelay(0.100); // NOTE: wait ((1000ms/10ms) * n) => (1000ms/10ms) *
    // 1ms => 100ms.
    
    c = getch();
    if (c == KEY_RESIZE) {
      editor_load_layout(E);
      // re-init the file browser..
      if (E->fb->type == DIR__)
        reinit_fb_bounds(E->fb, E->renderer->win_h);
      sprintf(E->notification_buffer, "resize detected!");
      goto UPDATE_EDITOR;
    }

    editor_load_layout(E);
    if (is_move(c, E->mode))
    {
      editor_handle_move(c, E);
      goto UPDATE_EDITOR;
    }

    if (c == KEY_COMMAND_ || c == KEY_COMMAND_O) {
      if (E->mode == FILEBROWSER || E->mode == NORMAL) {
        // TODO: COMMAD MODE > <
        editor_command_(E);
        goto UPDATE_EDITOR;
      }
    }

    switch (E->fb->type) {
    case DIR__: {
      editor_file_browser(c, E);
    } break;
    case FILE__:
    case NOT_EXIST: {
      editor_update(c, E);
    } break;
    default: {
    } break;
    }

  UPDATE_EDITOR:
    // sprintf(E->notification_buffer, "$END: %d $CUR: %d", (int)E->fb->end, (int)E->fb->cur_row);
    editor_render(E);
    if (E->exit_pressed)
      break;
  }

EXIT_AND_RELEASE_RESOURCES:
  release_editor(E);
  return 0;
}
