.PHONY: game run debug
CC			:= gcc

INCLUDE_DIR	+= $(ENGINE_DIR)/src/extern/glad/include

EXTERN_SRC  := extern/glad/src/glad.c

ENGINE_SRC  += engine/sdl/engine/engine.c \
 			   engine/sdl/render/render.c \
			   $(EXTERN_SRC)

SHADER_SRC	:= $(wildcard $(ENGINE_DIR)/src/platform/sdl/shaders/*.glsl)
SHADER_OUT	:= $(subst $(ENGINE_DIR)/src/platform/sdl/,$(BUILD_DIR)/,$(SHADER_SRC))

ifeq ($(OS), Windows_NT)
LIB_EXT		 = dll
else
LIB_EXT		 = so
endif

DEFINES		+= HE_PLATFORM_SDL2 HE_LIB_EXT=$(LIB_EXT) HE_GAME_NAME=$(GAME_NAME) HE_MAKE_DIR=$(PWD)

CORE_SRC	:= platform/sdl/bootstrap.c \
			   platform/sdl/render_display_opengl.c \
			   platform/sdl/gl/gl.c \
			   platform/sdl/gl/gl_shader.c \
			   $(EXTERN_SRC)

CORE_TARGET := $(BUILD_DIR)/$(GAME_NAME)
LIB_TARGET	:= $(BUILD_DIR)/game.$(LIB_EXT)

HEADERS		 = $(foreach dir, $(INCLUDE_DIR), $(shell find $(dir) -name "*.h"))
SRC			:= $(GAME_SRC) $(addprefix $(ENGINE_DIR)/src/,$(ENGINE_SRC))
OBJ     	:= $(filter %.o, $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC)) $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC)))
CORE_OBJ	:= $(patsubst %.c,$(OBJ_DIR)/%.o,$(CORE_SRC))
CORE_SRC	:= $(addprefix $(ENGINE_DIR)/src/,$(CORE_SRC))

CFLAGS		:= -Wall -fPIC $(addprefix -I,$(INCLUDE_DIR)) $(addprefix -D,$(DEFINES)) -Wno-unused
CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=c++98
LDFLAGS		:= -lSDL2 -ldl -lm

######################################################
# windows (mingw/msys) support
ifeq ($(OS), Windows_NT)
LDFLAGS		:= -lmingw32 -lSDL2main -lSDL2 -mwindows
DEFINES		+= -Dmain=SDL_main
endif
######################################################

######################################################
# build settings
ifeq ($(DEBUG_BUILD), true)
CFLAGS		+= -O0 -g3 -DDEBUG -Wextra
else
CFLAGS		+= -O2 -Werror
endif
######################################################

$(OBJ_DIR)/%.o: $(ENGINE_DIR)/src/%.c $(HEADERS)
	@mkdir -p `dirname $@`
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c $(ASSETS) $(HEADERS)
	@mkdir -p `dirname $@`
	@$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.cpp $(ASSETS) $(HEADERS)
	@mkdir -p `dirname $@`
	@$(CXX) -c $(CXXFLAGS) $< -o $@

$(CORE_TARGET): $(CORE_OBJ)
	@mkdir -p `dirname $@`
	@$(CC) $^ $(LDFLAGS) -o $(CORE_TARGET)
	@echo "UNIX engine core -> $(CORE_TARGET)"

$(LIB_TARGET): $(OBJ)
	@touch $(dir $@)/lock
	@mkdir -p `dirname $@`
	@$(CC) $^ $(LDFLAGS) -shared -fPIC -o $(LIB_TARGET)
	@rm $(dir $@)/lock
	@echo "UNIX game library -> $(LIB_TARGET)"

$(BUILD_DIR)/shaders/%.glsl: $(ENGINE_DIR)/src/platform/sdl/shaders/%.glsl
	@mkdir -p `dirname $@`
	@cp $^ $@
	@echo Copied shader `basename $@`

game: $(CORE_TARGET) $(LIB_TARGET) $(SHADER_OUT)

run: all
	@cd $(BUILD_DIR) && exec ./$(GAME_NAME)

debug: all
	@cd $(BUILD_DIR) && exec gdb ./$(GAME_NAME)
