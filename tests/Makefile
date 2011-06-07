include build/unix/Makefile.inc

#CPP     := g++
LDFLAGS += -lpthread
STATIC  += $(wildcard $(LIB_DIR)/gtest/*.a)
OUTPUT  := $(BIN_DIR)/run_tests

default: $(OBJ) $(STATIC)
	$(CPP) $(CFLAGS) $(LDFLAGS) -o $(OUTPUT) $^
	@$(OUTPUT)

clean:
	@rm -f $(OUTPUT)
