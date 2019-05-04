.PHONY: all clean

TARGETS := mkfontsheet mkpak
SRC     := $(shell find extern/ -type f -name '*.c')
SRC     += $(shell find src/lib/ -type f -name '*.c')
LIBS	:= -lSDL2 -lSDL2_ttf
CFLAGS  := -std=c99 -O2 $(LIBS) -I../src -I.
CC	:= gcc

%: src/%.c $(OBJ)
	$(CC) $< $(SRC) $(CFLAGS) -o $@

all: mkfontsheet mkpak

clean:
	rm -f $(TARGETS)
