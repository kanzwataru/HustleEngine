OBJS        = $(COMMON_OBJS) src/HustleEngine/platform/sdl/kb.o
SRCS        = $(OBJS:.o=.c)
HEADERS     = $(COMMON_HEADERS)
DEFINES		= -DUNIX
LIBS        = -lm
TARGET      = balloon

ifeq ($(SDL_VER), 2)
DEFINES    += -DSDL2
OBJS       += src/HustleEngine/platform/sdl/sdl2/video.o
LIBS       += -lSDL2
else
OBJS	   += src/HustleEngine/platform/sdl/sdl1.2/video.o
LIBS       += -lSDL
endif

ifeq ($(DEBUG_BUILD), 1)
CFLAGS      = -g3 -DDEBUG -Wpedantic -Wall -Werror
else
CFLAGS      = -O2 -Wall
endif
CFLAGS     += $(addprefix -I,$(HEADERS))
CXXFLAGS    = $(CFLAGS) -fno-exceptions -fno-rtti
LDFLAGS     = -o $(TARGET)

CC			= gcc
CXX			= g++

%.o: %.c
	$(CC) -c $(CFLAGS) $(DEFINES) $*.c -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS)

clean: preclean
	rm -f $(OBJS) $(TARGET)
