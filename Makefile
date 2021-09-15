CC=clang++
CFLAGS=-c -std=c++17 -Wall -MD -I/usr/local/include/SDL2 -Isrc -ferror-limit=0
LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf
SRC=src
BIN=bin
OBJ=$(BIN)/obj
TARGET=treeline

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
SRCS=$(call rwildcard,$(SRC),*.cpp)
OBJS=$(SRCS:$(SRC)/%.cpp=$(OBJ)/%.o)
BINS=$(TARGET:%=$(BIN)/%)

build: $(BINS)
	@echo "Build complete!"

debug: DEBUG=-DDEBUG_BUILD -g
debug: build

$(BINS): $(OBJS)
	@echo "Linking..."
	@mkdir -p $(BIN)
	@$(CC) $(LDFLAGS) -o $@ $^

$(OBJS): $(OBJ)/%.o: $(SRC)/%.cpp
	@echo "Assembling" $<
	@mkdir -p $(@D)
	@$(CC) $(DEBUG) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BIN)

-include $(OBJS:.o=.d)

