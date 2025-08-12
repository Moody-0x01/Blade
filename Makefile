CC=gcc
HMAP_DIR=./hmap
STATIC=./static
BIN=./bin
HMAP=./hmap/lib/libhmap.a
BLADELIB=$(STATIC)/libblade.a
MINIAUDIOLIB=$(STATIC)/libminiaudio.a
MINIAUDIOSRC=./include/miniaudio/miniaudio.c
NAME=$(BIN)/blade
SRC=./src/main.c
INCLUDE=-I./include -I$(HMAP_DIR)/include
CFLAGS=-Wall -pedantic -Wextra -std=c11 -ggdb -O3 -Wno-unused-result
LIBS=-L$(HMAP_DIR)/lib -lncursesw -lm -pthread -lhmap
SRCS=./src/audio/audio.c ./src/audio/audioplayer.c ./src/audio/fft.c ./src/common/Array.c ./src/common/chunk.c\
	 ./src/common/clipboard.c ./src/common/common.c ./src/common/dir.c ./src/common/logger.c ./src/common/np_atoi_base.c\
	 ./src/common/path_t.c ./src/common/read_file.c ./src/common/signals_.c ./src/common/split.c ./src/common/xstring.c\
	 ./src/editor/binds.c ./src/editor/blade.c ./src/editor/classification.c ./src/editor/command.c ./src/editor/editor.c\
	 ./src/editor/editor_command.c ./src/editor/emain.c ./src/editor/line.c ./src/editor/mode_executors.c\
	 ./src/editor/movement.c ./src/editor/rendering_functions.c ./src/visuals/colors.c ./src/visuals/emojis.c\
	 ./src/tokenizer/scripting.c ./src/tokenizer/construct_token.c ./src/tokenizer/token.c ./src/filebrowser/file_browser.c\
 	 ./src/filebrowser/filessystem.c ./src/cfg/cfg.c ./src/cfg/cfg_parser.c ./src/cfg/cfg_parse_config.c
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

$(NAME): pre $(MINIAUDIOLIB) $(BLADELIB) $(SRC) $(HMAP)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $(SRC) $(BLADELIB) $(MINIAUDIOLIB) $(LIBS) -fPIC -no-pie
$(HMAP):
	make -C $(HMAP_DIR)
clean:
	make -C $(HMAP_DIR) clean
	rm -rf miniaudio.o
	rm -rf $(OBJS)

fclean: clean
	make -C $(HMAP_DIR) fclean
	rm -rf $(STATIC)
	rm -rf $(BIN)

re: fclean all
