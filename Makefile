SRCS=$(wildcard src/*.cpp)
all: $(SRCS)
	g++ -std=c++17 $(SRCS) -I -L -Wall -Werror -lSDL2 -o treeline
