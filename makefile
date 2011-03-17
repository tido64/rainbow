TARGET = rainbow
BINDIR = bin
LIBDIR = ../lib
OBJDIR = build/unix
SRCDIR = src
STATIC = $(OBJDIR)/libbox2d.a $(OBJDIR)/liblua.a $(OBJDIR)/libpng.a

LIBLUA = $(LIBDIR)/Lua
LIBFT = /usr/include/freetype2
LIBPNG = $(LIBDIR)/libpng

CPP = g++
CFLAGS = -g -O2 -Wall -pipe -I $(LIBDIR) -I $(LIBLUA) -I $(LIBFT) -I $(LIBPNG) -ftree-vectorize -ftree-vectorizer-verbose=6 -march=native
LDFLAGS = -lGL -lSDL -lfreetype -lzip

EXEC = $(BINDIR)/$(TARGET)
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.cpp)) \
	$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/**/*.cpp))

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

%libpng.a:
	@LIBPNG=$(LIBPNG) make -f $(OBJDIR)/Makefile.libpng
	@make -f $(OBJDIR)/Makefile.libpng clean

check:
	@if [ ! -d $(BINDIR) ]; then mkdir -p $(BINDIR); fi
	@if [ ! -d $(OBJDIR)/Hardware ]; then mkdir -p $(OBJDIR)/Hardware; fi
	@if [ ! -d $(OBJDIR)/Input ]; then mkdir -p $(OBJDIR)/Input; fi
	@if [ ! -d $(OBJDIR)/Lua ]; then mkdir -p $(OBJDIR)/Lua; fi
	@if [ ! -d $(OBJDIR)/ParticleSystem ]; then mkdir -p $(OBJDIR)/ParticleSystem; fi

clean:
	@rm -fr $(OBJDIR)/Hardware
	@rm -fr $(OBJDIR)/Input
	@rm -fr $(OBJDIR)/Lua
	@rm -fr $(OBJDIR)/ParticleSystem
	@rm -fr $(OBJ)
