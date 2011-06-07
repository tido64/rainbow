include build/unix/Makefile.inc

TARGET     = rainbow
SRC_DIR    = src
MODULES    = ConFuoco ConFuoco/Codecs Input Lua

EXEC      := $(BIN_DIR)/$(TARGET)
BUILD_DIR := $(addprefix $(OBJ_DIR)/,$(MODULES))
MODULES   := $(SRC_DIR) $(addprefix $(SRC_DIR)/,$(MODULES))
OBJ       := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(foreach mod,$(MODULES),$(wildcard $(mod)/*.cpp)))

default: $(BUILD_DIR) $(EXEC)

$(EXEC): $(OBJ) $(STATIC)
	@echo "> Linking $@"
	@$(CPP) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo Compiling $<
	@$(CPP) -c $< $(CFLAGS) -o $@

%box2d.a:
	@$(MAKE) -f $(OBJ_DIR)/Makefile.Box2D
	@$(MAKE) -f $(OBJ_DIR)/Makefile.Box2D clean

%lua.a:
	@$(MAKE) -C $(LIBLUA) linux
	@mv $(LIBLUA)/liblua.a $(OBJ_DIR)
	@$(MAKE) -C $(LIBLUA) clean

%png.a:
	@SRC_DIR=$(LIBPNG) TARGET=libpng $(MAKE) -f $(OBJ_DIR)/Makefile.lib
	@TARGET=libpng $(MAKE) -f $(OBJ_DIR)/Makefile.lib clean

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@for dir in $(BUILD_DIR); do \
		rm -fr $$dir; \
	done
	@rm -fr $(OBJ)

distclean: clean
	@rm -f $(STATIC)

flags:
	@echo CFLAGS = $(CFLAGS)
	@echo LDFLAGS = $(LDFLAGS)

test: $(OBJ) $(STATIC)
	@echo "> Building tests"
	@$(CPP) -c $(SRC_DIR)/RainbowSDL.cpp -DRAINBOW_TEST $(CFLAGS) -o $(OBJ_DIR)/RainbowSDL.o
	@$(CPP) $(LDFLAGS) -lpthread -o $(BIN_DIR)/run_tests $(OBJ) $(STATIC) $(wildcard $(LIB_DIR)/gtest/*.a)
