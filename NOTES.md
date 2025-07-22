## NOW WORKING ON:
doing now => 
    <!-- [] Make an integrated shell. -->
    [ ] add word movement, move front with W and move back with B in normal and visual modes
    
## NOTEKEYS:
    [...] => Still under dev
    [*]   => Done
    [-]   => delayed

## Potential problems:
    - Byte encoding (utf8)
        0 - The editor can not read files that have non ascii bytes.
        1 - if it saves a file that contains these bytes. it gets corrupted.
            
## defs
    review: is an idea or a note  about a feature that was implemented but it is kind of wrong and should be fixed.
    feature: something that was not yet implemented and should be

## refs
    Colors: Learning bout colors: https://www.linuxjournal.com/content/about-ncurses-colors-0
    
## DONE
    [*] Make sure the editor's music player looks and seeems better.
    [*] add a music player haha.
    [*] Pick better coloring for the editor.
    [*] Modes (Normal, Visual, Insert).
    [*] Add copy/paste functionality
    [*] Organize features!
    [*] Handle Shift + key
    [*] Handle mouse pressing.
    [*] remove the old way of copy paste and introdude a struct.. ClipBoard to be used as a storage for any coppied or deled lines.
    [*] ability to delete files
    [*] add some keybindings
    [*]  add text highlighting for python as a test..
    [*]  Make the editor know which script it is edeting then apply this to the lexer so the syntax highlighting becomes better.
    [*]  add the cut capability. <- Working here.
    [*] Fix the cut, copy, paste and refactor this feature.
    [*] command Mode so we can add vim bindings.
    [*] ability to create/delete directories
    [*] ability to move/copy directories and files.
    [*] Add hjkl for movement.

## POSSIBLE NAMES:
    > Fox
    > Oxy
    > Linky
    > Vec
    > Ar
    > Curse
    > Line
    > astrix
    > linked

## POSSIBLE FEATURES:
    
    [...]  refactor -> Working Here.
    [...]  Work on colors and UI. -> Working Here.
    [...]  add the file browser.

    [] ability to create/delete directories.
    [] add types for c and python so u can add new types and colorize them differantly.    
    [] CD command to change the current dir..
    ```
        // PROMPT EXAMPLE:
            COMMAND > CMD_BUFFER
        then the buffer will be passed to a function to parse the command.
    ```
    [] Timer (Might be usefull if u need to stay less time implementing something.)
    [] add possibility to rename a file.

## POSSIBLE SYNTAX SUPPORT:
    [*] C.
    [*] js.
    [*] python.
    [] markdown
    [] Rust
    [] C++
 
### Personal reviews
    - review@1: [FIXED, most of it.]
        0 -> When I past coppied contant it makes a new line before inserting the coppied content. which is not good. [FIXED]
        1 -> I need to click f2 twice in order for the editor to quit, it should be only once. [FIXED]
        2 -> `c` copy key should kill a certain cut region, not just delete the content that the region holds. (...)
            1 -> cutting content from a line and pasting it works. however, it does not work if I want to cut a whole chunk of data (Multline).
        4 -> there is a bug in the cutting functionality that makes the x -> -4 for some reason.

    - review@2: [FIXED, most of it.]
        0 - The File browser does not display any informations about the current viewd dir. I need to implement some 
        to display info.
        1 - Fix buff constructor backspace.
        1 - The Text Editor has no proper name lol..
    
    - review@3 [FIXED]:
        0 - Fix the up/down that goes all the way to the end. instead it should go to somewhere more expected like the same col if (col <= size)
        1 - indent/unindemt a region.

    - review@4 [FIXED]:
        0 - Why the fuck does the editor paste on top of a line when I yank a certain line, Nromally it should make a new line then paste there..
        1 - make_promp_buff function has a certain bug where it does not clean the whole region where it writes to.. when the size of the buffer is 0..
        2 - The Create functionality is cool, but I need to delete files and directories also.. that is not enough
        3 - I also need markdown highlighting. 
        4 - This currrent version apparently can not lex multiline comments...
        ex: /* COMMENT */

## CRUCIAL BUGS:
    [FIXED] There is a bug that shows up when I try to cut from the start of the file. I need to fix he updating of lineren->start/lineren->origin apparently
    this problem accurs only when I try cutting a file.
    [FIXED (Partially.)] The Emoji's have differant size, I have no idea why..
    this problem accurs only when I try cutting a file.
    [...] The editor does not lazally load files, it just loads the entir file.. which is dangerous if u have a massive file (<more than 2GB>) 


## How To Lex Types in C:
    1 - anything that comes after a struct keyword and is an identifier.
    e: typedef struct T {...} T_t;
    2 - anythin that comes after the enum keyword and is an identifier.
    e: typedef enum T {...} T_e;

## ARCH PROBLEM THAT NEEDS A FIX:
    [] I dont like the way the editor tokenizes the tokens.. I need a more intuitive way like this:
        match file_type {
            C      => tokenize_as_c(&line);
            PYTHON => tokenize_as_python(&line);
            JS     => tokenize_as_js(&line);
        }
    This way I would have more strict and straight forward rules to tokenize the lines based of the type of the file I am editing
