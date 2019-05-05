.PHONY: all clean

TARGETS  := mkfontsheet mkpak mkasset

C_SRC    := $(shell find extern/ -type f -name '*.c')
C_SRC    += $(shell find src/lib/ -type f -name '*.c')
C_OBJ     = $(patsubst %.c, %.o, $(C_SRC))

CXX_SRC  := $(shell find src/lib/ -type f -name '*.cpp')
CXX_SRC  := $(shell find src/converters/ -type f -name '*.cpp')
CXX_OBJ   = $(patsubst %.cpp, %.o, $(CXX_SRC))

LIBS	 := -lSDL2 -lSDL2_ttf
INCDIR   := -I../src -Isrc -I.
CFLAGS   := -std=c99 -O2 $(LIBS) $(INCDIR)
CXXFLAGS := -std=c++11 -O2 $(LIBS) $(INCDIR)

CC	 	 := gcc
CXX	 	 := g++

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

%: src/%.o $(CXX_OBJ) $(C_OBJ)
	@$(CXX) $(CXXFLAGS) $^ -o $@

all: $(TARGETS)

clean:
	rm -f $(TARGETS)
	rm -f **/*.o
