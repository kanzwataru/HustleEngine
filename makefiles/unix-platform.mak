ENGINE_OBJS  = $(COMMON_ENGINE_OBJS) platform/sdl/kb.o
INCLUDE      = $(COMMON_INCLUDE) $(GAME_INCLUDE)
DEFINES		 = -DUNIX
LIBS         = -lm
TARGET       = $(GAME_NAME)

ifeq ($(SDL_VER), 2)
DEFINES     += -DSDL2
ENGINE_OBJS += platform/sdl/sdl2/video.o
LIBS        += -lSDL2
else
ENGINE_OBJS += platform/sdl/sdl1.2/video.o
LIBS        += -lSDL
endif

OBJS         = $(GAME_OBJS) $(addprefix $(ENGINE_DIR)/src/,$(ENGINE_OBJS))

ifeq ($(DEBUG_BUILD), 1)
CFLAGS       = -g3 -DDEBUG -Wpedantic -Wall -Werror
else
CFLAGS       = -O2 -Wall
endif
CFLAGS      += $(addprefix -I,$(INCLUDE))
CXXFLAGS     = $(CFLAGS) -fno-exceptions -fno-rtti
LDFLAGS      = -o $(TARGET)

CC			 = gcc
CXX			 = g++

%.o: %.c
	$(CC) -c $(CFLAGS) $(DEFINES) $*.c -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS)

build: $(TARGET)

clean: preclean
	rm -f $(OBJS) $(TARGET)
