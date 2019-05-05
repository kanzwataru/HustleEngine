.PHONY: buildnrun run

# Cross-compile for DOS with OpenWatcom
ENGINE_SRC += platform/dos32/vga.c platform/dos32/kb.c
HEADERS	   += $(GAME_INCLUDE)
LIBS		= #mathc.lib
TARGET		= $(BUILD_DIR)/$(GAME_NAME).exe
DOS_ROOT    = $(shell pwd)/$(BUILD_DIR)

CFLAGS 		= -dDOS32 -dHUSTLE_RUNTIME -bt=dos -zq -w4 -e25
ifeq ($(DEBUG_BUILD), 1)
CFLAGS	   += -od -d2 -dDEBUG
else
CFLAGS	   += -otexan -d0
endif
CFLAGS	   += $(addprefix -i,$(HEADERS))
LDFLAGS		= -bt=dos -l=pmodew -fe=$(TARGET) -fm=$(BUILD_DIR)/$(GAME_NAME) -6r -mf -zq

CC			= wcl386
CC_DOS_DIR  = $(dir $(shell which $(CC)))../binw
EXTENDER    = pmodew.exe

$(BUILD_DIR)/$(EXTENDER):
	@# (if using PMODE/W extender, it's not necessary to copy to build because it's embedded)
	@#mkdir -p `dirname $@`
	@#cp $(CC_DOS_DIR)/$(EXTENDER) $@

	@# copy it to the current dir also, temporarily for the linker to find
	@cp $(CC_DOS_DIR)/$(EXTENDER) ./$(EXTENDER)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p `dirname $@`
	@$(CC) -c $(CFLAGS) $^ -fo=$@

$(TARGET): $(BUILD_DIR)/$(EXTENDER) $(OBJS)
	@mkdir -p `dirname $@`
	@$(CC) $(LDFLAGS) $(OBJS) $(LIBS)

	@# clean up unnecessary crud that OpenWatcom leaves behind
	@rm -f *.err

build: $(TARGET)
	@# clean up temporary extender
	@rm $(EXTENDER)

run: all
	dosbox -c "Z:" -c "mount F $(DOS_ROOT)" -c "F:" -c "cd $(DOS_BUILD)" -c "$(GAME_NAME).exe"

clean: preclean
	@rm -f *.err
