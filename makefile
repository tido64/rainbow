# Libraries
LIBDIR = ../lib
LIBLUA = $(LIBDIR)/Lua
LIBFREETYPE = /usr/include/freetype2

# Build environment
TARGET = onwire
BINDIR = bin
OBJDIR = build/unix
SRCDIR = src

# Flags
CPP = g++
CFLAGS = -g -O2 -Wall -pipe -I $(LIBDIR) -I $(LIBLUA) -I libs -I $(LIBFREETYPE) -ftree-vectorize -ftree-vectorizer-verbose=0 -march=native
LDFLAGS = -lGL -lSDL -lfreetype -lpng -lzip
#STATIC_LIBS = /usr/lib/libfreetype.a /usr/lib/libpng.a /usr/lib/libzip.a /usr/lib/libz.a

EXEC = $(BINDIR)/$(TARGET)
OBJ = $(OBJDIR)/OnWireSDL.o \
	$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/**/*.cpp)) \
	$(OBJDIR)/librainbow.a \
	$(OBJDIR)/libbox2d.a \
	$(OBJDIR)/liblua.a

default: check $(EXEC)

$(EXEC): $(OBJ)
	$(CPP) $(LDFLAGS) -o $@ $(OBJ) $(STATIC_LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo Compiling $<
	@$(CPP) -c $< $(CFLAGS) -o $@

%box2d.a:
	@LIBDIR=$(LIBDIR) make -f $(OBJDIR)/Makefile.Box2D

%lua.a:
	@cd $(LIBLUA) && make linux
	@mv $(LIBLUA)/liblua.a $(OBJDIR)
	@cd $(LIBLUA) && make clean

%rainbow.a:
	@LIBDIR=$(LIBDIR) LIBLUA=$(LIBLUA) LIBFREETYPE=$(LIBFREETYPE) make -f $(OBJDIR)/Makefile.Rainbow

check:
	@if [ ! -d $(BINDIR) ]; then mkdir -p $(BINDIR); fi
	@if [ ! -d $(OBJDIR)/Elements ]; then mkdir -p $(OBJDIR)/Elements; fi
	@if [ ! -d $(OBJDIR)/Objects ]; then mkdir -p $(OBJDIR)/Objects; fi

clean:
	@make -f $(OBJDIR)/Makefile.Box2D clean
	@make -f $(OBJDIR)/Makefile.Rainbow clean
	rm -f $(OBJ)
	rm -fr $(OBJDIR)/Elements
	rm -fr $(OBJDIR)/Objects

clean-all: clean
	@rm -fr $(EXEC)

debug:
	@echo CFLAGS = $(CFLAGS)
