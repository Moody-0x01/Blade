CC=gcc
STATIC=./static
BIN=./bin
BLADELIB=$(STATIC)/libblade.a
MINIAUDIOLIB=$(STATIC)/libminiaudio.a
MINIAUDIOSRC=./include/miniaudio/miniaudio.c
NAME=$(BIN)/blade
SRC=./src/main.c
INCLUDE=-I./include
CFLAGS=-Wall -pedantic -Wextra -std=c11 -ggdb
LIBS=-lncursesw -lm -pthread
SRCS=./src/audio/audio.c ./src/audio/audioplayer.c ./src/audio/fft.c ./src/common/Array.c ./src/common/chunk.c\
	 ./src/common/clipboard.c ./src/common/common.c ./src/common/dir.c ./src/common/logger.c ./src/common/np_atoi_base.c\
	 ./src/common/path_t.c ./src/common/read_file.c ./src/common/signals_.c ./src/common/split.c ./src/common/xstring.c\
	 ./src/editor/binds.c ./src/editor/blade.c ./src/editor/classification.c ./src/editor/command.c ./src/editor/editor.c\
	 ./src/editor/editor_command.c ./src/editor/emain.c ./src/editor/line.c ./src/editor/mode_executors.c\
	 ./src/editor/movement.c ./src/editor/rendering_functions.c ./src/visuals/colors.c ./src/visuals/emojis.c\
	 ./src/tokenizer/scripting.c ./src/tokenizer/token.c ./src/filebrowser/file_browser.c\
	 ./src/filebrowser/filessystem.c ./src/cfg/cfg.c ./src/cfg/cfg_parser.c
OBJS=$(SRCS:%.c=%.o)

all: $(NAME)

pre:
	mkdir -p $(BIN)
	mkdir -p $(STATIC)

$(MINIAUDIOLIB): $(MINIAUDIOSRC)
	$(CC) $(CFLAGS) $(INCLUDE) -c $(MINIAUDIOSRC)
	ar rcs $@ miniaudio.o

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $^ -o $@

$(BLADELIB): $(OBJS)
	ar rcs $@ $?

$(NAME): pre $(MINIAUDIOLIB) $(BLADELIB) $(SRC)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $(SRC) $(BLADELIB) $(MINIAUDIOLIB) $(LIBS) -fPIC

clean:
	rm -rf miniaudio.o
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(STATIC)
	rm -rf $(BIN)

re: fclean all
