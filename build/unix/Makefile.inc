BIN_DIR = bin
LIB_DIR = ../lib
OBJ_DIR = build/unix

# Dynamic libraries
INCFT  := `freetype-config --cflags`
INCSDL := `sdl-config --cflags`

# Static libraries
STATIC  := $(OBJ_DIR)/libbox2d.a $(OBJ_DIR)/libpng.a
LIBLUA  := /usr/include/luajit-2.0
LIBPNG  := $(LIB_DIR)/libpng

CC      := clang -x c
CPP     := clang
CFLAGS  += -g -O0 -Wall -Werror -pipe $(INCSDL) $(INCFT) -I$(LIB_DIR) -I$(LIBLUA) -I$(LIBPNG) -ftrapv -fcatch-undefined-behavior -march=native -stdlib=libstdc++
LDFLAGS += -lstdc++ -lm -lz -lGL -lopenal -lSDL -lfreetype -lluajit-5.1 -lvorbisfile

.PHONY: default clean distclean flags test
