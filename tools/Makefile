.PHONY: all clean

LIBS	:= -lSDL2 -lSDL2_ttf
CFLAGS  := -std=c99 -O2 $(LIBS)
CC		:= gcc

%: src/%.c
	$(CC) -c $(CFLAGS) $< -o tmp.o
	$(CC) tmp.o $(CFLAGS) -o $@
	rm -f tmp.o

all: mkfontsheet

clean:
	rm -f mkfontsheet
