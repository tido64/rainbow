# http://wiki.debian.org/Hardening

BIN_DIR  := bin
LIB_DIR  := ../lib
OBJ_DIR  := build/unix

# Dynamic libraries
INCLUDE  := $(shell pkg-config --cflags freetype2 luajit sdl)

# Static libraries
STATIC   := $(OBJ_DIR)/libbox2d.a $(OBJ_DIR)/libpng.a
LIBPNG   := $(LIB_DIR)/libpng

CC       := clang -x c
CXX      := clang
CFLAGS   := -pipe -g0 -O2 -w -I$(LIB_DIR) -fno-rtti -fpie -march=native
CXXFLAGS := -pipe -std=c++0x -g -O0 -Wall -Wextra -pedantic-errors $(INCLUDE) -I$(LIBPNG) -I$(LIB_DIR) -fno-rtti -ftrapv -fpie -fcatch-undefined-behavior -march=native -stdlib=libstdc++
LIBS     := -lluajit-5.1 -lfreetype -lSDL -lvorbisfile -lopenal -lGL -lz -lm -lstdc++
LDFLAGS  := -Wl,-z,now -Wl,-z,relro -pie

# Pretty functions
archive   = @echo "> Archiving $@"; $(AR) rcDs $@ $^
compile   = @echo "Compiling $<..."; $(CXX) -c $< $(CXXFLAGS) -frandom-seed="$@" -o $@
compilec  = @echo "Compiling $<..."; $(CC) -c $< $(CFLAGS) -frandom-seed="$@" -o $@
link      = @echo "> Linking $@"; $(CXX) $(LDFLAGS) $(LIBS) -o $@ $^
makelib   = @TARGET=$(1) SRC_DIR=$(2) $(MAKE) -f $(OBJ_DIR)/Makefile.lib -s $(3)

.PHONY: clean distclean flags test
