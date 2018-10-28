ENGINE_SRC   = $(COMMON_ENGINE_SRC) platform/sdl/kb.c
INCLUDE      = $(COMMON_INCLUDE) $(GAME_INCLUDE)
DEFINES		 = -DUNIX
LIBS         = -lm
TARGET       = $(BUILD_DIR)/$(GAME_NAME)

ifeq ($(SDL_VER), 2)
DEFINES     += -DSDL2
ENGINE_SRC  += platform/sdl/sdl2/video.c
LIBS        += -lSDL2
else
ENGINE_SRC  += platform/sdl/sdl1.2/video.c
LIBS        += -lSDL
endif

SRC          = $(GAME_SRC) $(addprefix $(ENGINE_DIR)/src/,$(ENGINE_SRC))
OBJS		 = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

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

$(OBJ_DIR)/%.o: %.c
	mkdir -p `dirname $@`
	$(CC) -c $(CFLAGS) $(DEFINES) $*.c -o $@

$(TARGET): $(OBJS)
	mkdir -p `dirname $@`
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS)

build: $(TARGET)

clean: preclean
	rm -f $(OBJS) $(TARGET)
