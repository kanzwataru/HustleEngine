.PHONY: all clean

TARGETS := mkfontsheet mkpak
SRC     := ../src/extern/inih/ini.c
LIBS	:= -lSDL2 -lSDL2_ttf
CFLAGS  := -std=c99 -O2 $(LIBS) -I../src
CC		:= gcc

%: src/%.c $(OBJ)
	$(CC) $< $(SRC) $(CFLAGS) -o $@

all: mkfontsheet mkpak

clean:
	rm -f $(TARGETS)
