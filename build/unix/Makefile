include build/unix/default.mk

TARGET    := rainbow
SRC_DIR   := src
MODULES   := ConFuoco ConFuoco/Codecs Graphics Input Lua

CXXFLAGS  += -I$(SRC_DIR)
EXEC      := $(BIN_DIR)/$(TARGET)
BUILD_DIR := $(addprefix $(OBJ_DIR)/,$(MODULES))
MODULES   := $(SRC_DIR) $(addprefix $(SRC_DIR)/,$(MODULES))
OBJ       := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(foreach mod,$(MODULES),$(wildcard $(mod)/*.cpp)))

default: $(BUILD_DIR) $(EXEC)

$(EXEC): $(OBJ) $(STATIC)
	$(call link)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(call compile)

%box2d.a:
	@$(MAKE) -f $(OBJ_DIR)/Makefile.Box2D -s
	@$(MAKE) -f $(OBJ_DIR)/Makefile.Box2D clean

%lua.a:
	@$(MAKE) -C $(LIBLUA) linux && mv $(LIBLUA)/liblua.a $(OBJ_DIR) && $(MAKE) -C $(LIBLUA) clean

%png.a:
	@TARGET=libpng SRC_DIR=$(LIBPNG) $(MAKE) -f $(OBJ_DIR)/Makefile.lib -s
	@TARGET=libpng $(MAKE) -f $(OBJ_DIR)/Makefile.lib clean

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@for dir in $(BUILD_DIR); do \
		rm -fr $$dir; \
	done
	@rm -fr $(OBJ)

distclean: clean
	@$(RM) $(STATIC)

flags:
	@echo CFLAGS = $(CFLAGS)
	@echo CXXFLAGS = $(CXXFLAGS)
	@echo LDFLAGS = $(LDFLAGS)

test: $(OBJ) $(STATIC)
	@echo "> Building tests"
	@$(CXX) -c $(SRC_DIR)/RainbowSDL.cpp -DRAINBOW_TEST $(CXXFLAGS) -o $(OBJ_DIR)/RainbowSDL.o
	@$(CXX) $(LDFLAGS) -lpthread -o $(BIN_DIR)/run_tests $(OBJ) $(STATIC) $(wildcard $(LIB_DIR)/gtest/*.a) && rm $(OBJ_DIR)/RainbowSDL.o
