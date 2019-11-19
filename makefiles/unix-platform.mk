.PHONY: game run debug
CC			:= gcc

INCLUDE_DIR	+= $(ENGINE_DIR)/src/extern/glad/include

EXTERN_SRC  := extern/glad/src/glad.c

ENGINE_SRC  += engine/sdl/engine/engine.c \
 			   engine/sdl/render/render.c \
			   engine/sdl/render/gl.c \
			   engine/sdl/render/gl_shader.c \
			   $(EXTERN_SRC)

DEFINES		+= HE_PLATFORM_SDL2 HE_LIB_EXT=so HE_GAME_NAME=$(GAME_NAME) HE_MAKE_DIR=$(PWD)
CORE_SRC	:= platform/sdl/bootstrap.c \
			   $(EXTERN_SRC)

CORE_TARGET := $(BUILD_DIR)/$(GAME_NAME)
LIB_TARGET	:= $(BUILD_DIR)/game.so

HEADERS		 = $(foreach dir, $(INCLUDE_DIR), $(shell find $(dir) -name "*.h"))
SRC			:= $(GAME_SRC) $(addprefix $(ENGINE_DIR)/src/,$(ENGINE_SRC))
OBJ     	:= $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
CORE_OBJ	:= $(patsubst %.c,$(OBJ_DIR)/%.o,$(CORE_SRC))
CORE_SRC	:= $(addprefix $(ENGINE_DIR)/src/,$(CORE_SRC))

CFLAGS		:= -Wall -fPIC $(addprefix -I,$(INCLUDE_DIR)) $(addprefix -D,$(DEFINES))
LDFLAGS		:= -lSDL2 -ldl

######################################################
# build settings
ifeq ($(DEBUG_BUILD), true)
CFLAGS		+= -O0 -g3 -DDEBUG -Wpedantic
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

$(CORE_TARGET): $(CORE_OBJ)
	@mkdir -p `dirname $@`
	@$(CC) $(LDFLAGS) $^ -o $(CORE_TARGET)
	@echo "UNIX engine core -> $(CORE_TARGET)"

$(LIB_TARGET): $(OBJ)
	@touch $(dir $@)/lock
	@mkdir -p `dirname $@`
	@$(CC) $(LDFLAGS) -shared -fPIC $^ -o $(LIB_TARGET)
	@rm $(dir $@)/lock
	@echo "UNIX game library -> $(LIB_TARGET)"

game: $(CORE_TARGET) $(LIB_TARGET)

run: all
	@cd $(BUILD_DIR) && ./$(GAME_NAME)

debug: all
	@cd $(BUILD_DIR) && gdb ./$(GAME_NAME)
