CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -O2 -Iinclude
SRC      := $(wildcard src/*.cpp)
OBJ      := $(patsubst src/%.cpp,build/%.o,$(SRC))
BIN      := build/switchback

.PHONY: all run clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(OBJ) -o $@

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build:
	mkdir -p build

# The program opens its level file from the current folder,
# so run it from inside levels/.
run: $(BIN)
	cd levels && ../$(BIN)

clean:
	rm -rf build

-include $(OBJ:.o=.d)
