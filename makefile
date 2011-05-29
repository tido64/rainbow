TARGET = rainbow
BINDIR = bin
LIBDIR = ../lib
OBJDIR = build/unix
SRCDIR = src

# Dynamic libraries
INCFT  := `freetype-config --cflags`
INCSDL := `sdl-config --cflags`

# Static libraries
STATIC  := $(OBJDIR)/libbox2d.a $(OBJDIR)/libpng.a
LIBLUA  := /usr/include/luajit-2.0
LIBPNG  := $(LIBDIR)/libpng

CC      := "clang -x c"
CPP     := clang
CFLAGS  := -g -O2 -Wall -Werror -pipe $(INCSDL) $(INCFT) -I$(LIBDIR) -I$(LIBLUA) -I$(LIBPNG) -march=native -stdlib=libstdc++
LDFLAGS := -lstdc++ -lm -lGL -lSDL -lfreetype -lluajit-5.1 -lz -lopenal -lvorbisfile

EXEC := $(BINDIR)/$(TARGET)
OBJ  := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.cpp)) \
	$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/**/*.cpp)) \
	$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/**/**/*.cpp))
DIRS := Common ConFuoco ConFuoco/Codecs Hardware Input Lua ParticleSystem

default: check $(EXEC)

$(EXEC): $(OBJ) $(STATIC)
	$(CPP) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo Compiling $<
	@$(CPP) -c $< $(CFLAGS) -o $@

%box2d.a:
	@CPP=$(CPP) LIBDIR=$(LIBDIR) make -f $(OBJDIR)/Makefile.Box2D
	@make -f $(OBJDIR)/Makefile.Box2D clean

%lua.a:
	@cd $(LIBLUA) && make linux
	@mv $(LIBLUA)/liblua.a $(OBJDIR)
	@cd $(LIBLUA) && make clean

%png.a:
	@CC=$(CC) LIBSRC=$(LIBPNG) TARGET=libpng make -f $(OBJDIR)/Makefile.lib
	@TARGET=libpng make -f $(OBJDIR)/Makefile.lib clean

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

distclean: clean
	@rm -f $(STATIC)

flags:
	@echo $(CFLAGS)
	@echo $(LDFLAGS)
