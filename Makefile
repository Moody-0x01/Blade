CC=gcc
STATIC=./static
BIN=./bin
BLADELIB=$(STATIC)/libblade.a
MINIAUDIOLIB=$(STATIC)/libminiaudio.a
MINIAUDIOSRC=./include/miniaudio/miniaudio.h
NAME=$(BIN)/blade
SRC=./src/main.c
INCLUDE=-I./include
CFLAGS=-Wall -pedantic -Wextra -std=c11 -ggdb
LIBS=-lncursesw -lm -pthread
AUDIOAPISRC=./src/audio/*.c
COMMONAPISRC=./src/common/*.c
EDITORAPISRC=./src/editor/*.c
VISUALAPISRC=./src/visuals/*.c
TOKENIZERAPISRC=./src/tokenizer/*.c
FILEBROWSERAPISRC=./src/filebrowser/*.c
CFGSRC=./src/cfg/*.c

all: $(NAME)

pre:
	mkdir -p $(BIN)
	mkdir -p $(STATIC)

$(MINIAUDIOLIB): $(MINIAUDIOSRC)
	$(CC) -x c -c $(MINIAUDIOSRC)
	ar rcs $@ miniaudio.o
	rm miniaudio.o

$(BLADELIB): $(AUDIOAPISRC) $(COMMONAPISRC) $(EDITORAPISRC) $(VISUALAPISRC) $(TOKENIZERAPISRC) $(FILEBROWSERAPISRC) $(CFGSRC)
	$(CC) $(CFLAGS) $(INCLUDE) -c $(COMMONAPISRC) -fPIC
	$(CC) $(CFLAGS) $(INCLUDE) -c $(EDITORAPISRC) -fPIC
	$(CC) $(CFLAGS) $(INCLUDE) -c $(VISUALAPISRC) -fPIC
	$(CC) $(CFLAGS) $(INCLUDE) -c $(TOKENIZERAPISRC) -fPIC
	$(CC) $(CFLAGS) $(INCLUDE) -c $(FILEBROWSERAPISRC) -fPIC
	$(CC) $(CFLAGS) $(INCLUDE) -c $(CFGSRC) -fPIC
	$(CC) $(CFLAGS) $(INCLUDE) -c $(AUDIOAPISRC) -fPIC
	ar rcs $@ *.o
	rm *.o

$(NAME): pre $(MINIAUDIOLIB) $(BLADELIB) $(SRC)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $(SRC) $(BLADELIB) $(MINIAUDIOLIB) $(LIBS) -fPIC

clean:
	rm -rf *.o

fclean: clean
	rm -rf $(STATIC)
	rm -rf $(BIN)

re: fclean all
