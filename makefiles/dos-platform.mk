.PHONY: game run debug
CC			:= wcl386
CXX			:= wcl386
CC_DOS_DIR	:= $(dir $(shell which $(CC)))../binw
EXTENDER	:= pmodew
TARGET 		:= $(BUILD_DIR)/$(GAME_NAME).exe
DOS_ROOT	:= $(abspath $(BUILD_DIR))

INCLUDE_DIR		+=
ENGINE_SRC  += engine/dos/engine/engine.c \
			   engine/dos/engine/timer.s  \
 			   engine/dos/render/render.c \
			   platform/dos/bootstrap.c
DEFINES		+= HE_PLATFORM_DOS

HEADERS		 = $(foreach dir, $(INCLUDE_DIR), $(shell find $(dir) -name "*.h"))
SRC			:= $(GAME_SRC) $(addprefix $(ENGINE_DIR)/src/,$(ENGINE_SRC))
OBJ     	:= $(filter %.o, $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC)) $(patsubst %.s,$(OBJ_DIR)/%.o,$(SRC)) $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC)))

CFLAGS_COMM := -bt=dos -4s -zq -w4 -e25 -l=$(EXTENDER) $(addprefix -i,$(INCLUDE_DIR)) $(addprefix -d,$(DEFINES))
CFLAGS		:= $(CFLAGS_COMM) -za99
CXXFLAGS	:= $(CFLAGS_COMM)
LDFLAGS		:= -bt=dos -za99 -l=$(EXTENDER) -fe=$(TARGET) -fm=$(BUILD_DIR)/$(GAME_NAME) -4s -mf -zq

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

$(OBJ_DIR)/%.o: $(ENGINE_DIR)/src/%.c $(HEADERS)
	@mkdir -p `dirname $@`
	@$(CC) -c $(CFLAGS) $< -fo=$@

$(OBJ_DIR)/%.o: %.c $(ASSETS) $(HEADERS)
	@mkdir -p `dirname $@`
	@$(CC) -c $(CFLAGS) $< -fo=$@


$(OBJ_DIR)/%.o: %.cpp $(ASSETS) $(HEADERS)
	@mkdir -p `dirname $@`
	@$(CXX) -c $(CXXFLAGS) $< -fo=$@

$(OBJ_DIR)/%.o: %.s
	@mkdir -p `dirname $@`
	@nasm -f obj $^ -o $@

$(TARGET): $(OBJ) $(BUILD_DIR)/$(EXTENDER).exe
	@mkdir -p `dirname $@`
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $(CORE_TARGET)
	@echo "DOS32 binary -> $(TARGET)"

game: $(TARGET)
	@# clean up unnecessary crud that OpenWatcom leaves behind
	@rm -f *.err
	@rm -f $(EXTENDER).exe

run: all
	@dosbox -c "Z:" -c "mount F $(DOS_ROOT)" -c "F:" -c "$(GAME_NAME).exe"

debug: all
	@echo "No debugging available for this platform"
