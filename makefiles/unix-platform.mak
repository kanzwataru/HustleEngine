.PHONY: platform_build platform_run platform_debug
CC			:= gcc

ENGINE_SRC  += engine/sdl/engine.c engine/sdl/render.c
DEFINES		+= HE_PLATFORM_SDL2 HE_LIB_EXT=so HE_GAME_NAME=$(GAME_NAME)
CORE_SRC	:= platform/sdl/bootstrap.c

CORE_TARGET := $(BUILD_DIR)/$(GAME_NAME)
LIB_TARGET	:= $(BUILD_DIR)/game.so

SRC			:= $(GAME_SRC) $(addprefix $(ENGINE_DIR)/src/,$(ENGINE_SRC))
OBJ     	:= $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
CORE_OBJ	:= $(patsubst %.c,$(OBJ_DIR)/%.o,$(CORE_SRC))
CORE_SRC	:= $(addprefix $(ENGINE_DIR)/src/,$(CORE_SRC))

CFLAGS		:= -Wall -fPIC $(addprefix -I,$(INCLUDE)) $(addprefix -D,$(DEFINES))
LDFLAGS		:= -lSDL2

######################################################
# build settings
ifeq ($(DEBUG_BUILD), 1)
CFLAGS		+= -O0 -g3 -DDEBUG -Wpedantic
else
CFLAGS		+= -O2 -Werror
endif
######################################################

$(OBJ_DIR)/%.o: $(ENGINE_DIR)/src/%.c
	@mkdir -p `dirname $@`
	@$(CC) -c $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p `dirname $@`
	@$(CC) -c $(CFLAGS) $^ -o $@

$(CORE_TARGET): $(CORE_OBJ)
	@mkdir -p `dirname $@`
	@$(CC) $(LDFLAGS) $^ -o $(CORE_TARGET)

$(LIB_TARGET): $(OBJ)
	@touch $(dir $@)/lock
	@mkdir -p `dirname $@`
	@$(CC) $(LDFLAGS) -shared -fPIC $^ -o $(LIB_TARGET)
	@rm $(dir $@)/lock

platform_build: $(CORE_TARGET) $(LIB_TARGET)

platform_run: all
	@cd $(BUILD_DIR) && $(CORE_TARGET)

platform_debug: all
	@cd $(BUILD_DIR) && gdb $(CORE_TARGET)
