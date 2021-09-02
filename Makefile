CC=g++
CFLAGS=-c -std=c++17 -Wall -Werror
LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
SRC=src
BIN=bin
OBJ=$(BIN)/obj
TARGET=treeline

SRCS=$(wildcard $(SRC)/*.cpp)
OBJS=$(SRCS:$(SRC)/%.cpp=$(OBJ)/%.o)
BINS=$(TARGET:%=$(BIN)/%)

build: $(BINS)
	@echo "Build complete!"

$(BINS): $(OBJS)
	@echo "Assembling..."
	@mkdir -p $(BIN)
	@$(CC) $(LDFLAGS) -o $@ $^

$(OBJS): $(OBJ)/%.o: $(SRC)/%.cpp
	@echo "Assembling" $<
	mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BIN)

