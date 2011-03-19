TARGET = rainbow
BINDIR = bin
LIBDIR = ../lib
OBJDIR = build/unix
SRCDIR = src

LIBFT  := /usr/include/freetype2
STATIC := $(OBJDIR)/libbox2d.a $(OBJDIR)/liblua.a $(OBJDIR)/libpng.a #$(OBJDIR)/libzip.a
LIBLUA := $(LIBDIR)/Lua
LIBPNG := $(LIBDIR)/libpng
LIBZIP := $(LIBDIR)/libzip

CPP     := g++
CFLAGS  := -g -O2 -Wall -pipe -I $(LIBFT) -I $(LIBDIR) -I $(LIBLUA) -I $(LIBPNG) -ftree-vectorize -ftree-vectorizer-verbose=6 -march=native
LDFLAGS := -lGL -lSDL -lfreetype -lz -lzip

EXEC := $(BINDIR)/$(TARGET)
OBJ  := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.cpp)) \
	$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/**/*.cpp))
DIRS := Hardware Input Lua ParticleSystem

default: check $(EXEC)

$(EXEC): $(OBJ) $(STATIC)
	$(CPP) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo Compiling $<
	@$(CPP) -c $< $(CFLAGS) -o $@

%box2d.a:
	@LIBDIR=$(LIBDIR) make -f $(OBJDIR)/Makefile.Box2D
	@make -f $(OBJDIR)/Makefile.Box2D clean

%lua.a:
	@cd $(LIBLUA) && make linux
	@mv $(LIBLUA)/liblua.a $(OBJDIR)
	@cd $(LIBLUA) && make clean

%png.a:
	@LIBSRC=$(LIBPNG) TARGET=libpng make -f $(OBJDIR)/Makefile.lib
	@TARGET=libpng make -f $(OBJDIR)/Makefile.lib clean

%zip.a:
	@LIBSRC=$(LIBZIP) TARGET=libzip make -f $(OBJDIR)/Makefile.lib
	@TARGET=libzip make -f $(OBJDIR)/Makefile.lib clean

check:
	@if [ ! -d $(BINDIR) ]; then mkdir -p $(BINDIR); fi
	@for dir in $(DIRS); do \
		if [ ! -d $(OBJDIR)/$$dir ]; then \
			mkdir -p $(OBJDIR)/$$dir; \
		fi; \
	done

clean:
	@for dir in $(DIRS); do \
		rm -fr $(OBJDIR)/$$dir; \
	done
	@rm -fr $(OBJ)

dist-clean: clean
	@rm -f $(STATIC)
