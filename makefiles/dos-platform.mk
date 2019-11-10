.PHONY: platform_build platform_run platform_debug
CC			:= wcl386
CC_DOS_DIR	:= $(dir $(shell which $(CC)))../binw
EXTENDER	:= pmodew
TARGET 		:= $(BUILD_DIR)/$(GAME_NAME).exe
DOS_ROOT	:= $(abspath $(BUILD_DIR))

HEADERS		+=
ENGINE_SRC  += engine/dos/engine/engine.c engine/dos/render/render.c platform/dos/bootstrap.c
DEFINES		+= HE_PLATFORM_DOS

SRC			:= $(GAME_SRC) $(addprefix $(ENGINE_DIR)/src/,$(ENGINE_SRC))
OBJ     	:= $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

CFLAGS		:= -bt=dos -4s -zq -w4 -e25 -l=$(EXTENDER) $(addprefix -i,$(HEADERS)) $(addprefix -d,$(DEFINES))
LDFLAGS		:= -bt=dos -l=$(EXTENDER) -fe=$(TARGET) -fm=$(BUILD_DIR)/$(GAME_NAME) -4s -mf -zq

######################################################
# build settings
ifeq ($(DEBUG_BUILD), true)
CFLAGS		+= -dDEBUG -od -d2
else
CFLAGS		+= -otexan -d0
endif
######################################################

$(BUILD_DIR)/$(EXTENDER).exe:
	@# (if using PMODE/W extender, it's not necessary to copy to build because it's embedded)
	@#mkdir -p `dirname $@`
	@#cp $(CC_DOS_DIR)/$(EXTENDER) $@

	@# copy it to the current dir also, temporarily for the linker to find
	@cp $(CC_DOS_DIR)/$(EXTENDER).exe ./$(EXTENDER).exe

$(OBJ_DIR)/%.o: $(ENGINE_DIR)/src/%.c
	@mkdir -p `dirname $@`
	@$(CC) -c $(CFLAGS) $^ -fo=$@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p `dirname $@`
	@$(CC) -c $(CFLAGS) $^ -fo=$@

$(TARGET): $(OBJ) $(BUILD_DIR)/$(EXTENDER).exe
	@mkdir -p `dirname $@`
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $(CORE_TARGET)
	@echo "DOS32 binary -> $(TARGET)"

platform_build: $(TARGET)
	@# clean up unnecessary crud that OpenWatcom leaves behind
	@rm -f *.err
	@rm $(EXTENDER).exe

platform_run: all
	@dosbox -c "Z:" -c "mount F $(DOS_ROOT)" -c "F:" -c "$(GAME_NAME).exe"	

platform_debug: all
	@echo "No debugging available for this platform"
