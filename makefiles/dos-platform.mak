.PHONY: buildnrun run

# Cross-compile for DOS with OpenWatcom
ENGINE_SRC += platform/dos32/vga.c platform/dos32/kb.c platform/dos32/mem.c
HEADERS	   += $(GAME_INCLUDE)
LIBS		= mathc.lib
TARGET		= $(BUILD_DIR)/$(GAME_NAME).exe
DOS_ROOT    = $(shell pwd)/$(BUILD_DIR)

CFLAGS 		= -dDOS32 -4 -bt=dos -ml -w4 -zq
ifeq ($(DEBUG_BUILD), 1)
CFLAGS	   += -od -d2 -dDEBUG
else
CFLAGS	   += -otexan -d0
endif
CFLAGS	   += $(addprefix -i,$(HEADERS))
LDFLAGS		= -bt=dos -fe=$(TARGET) -fm=$(BUILD_DIR)/$(GAME_NAME)

CC			= wcl

$(OBJ_DIR)/%.o: %.c
	mkdir -p `dirname $@`
	$(CC) -c $(CFLAGS) $^ -fo=$@

$(TARGET): $(OBJS)
	mkdir -p `dirname $@`
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS)

	# clean up unnecessary crud that OpenWatcom leaves behind
	rm -f *.err

build: $(TARGET)

run: all
	dosbox -c "Z:" -c "mount F $(DOS_ROOT)" -c "F:" -c "cd $(DOS_BUILD)" -c "$(GAME_NAME).exe"

clean: preclean
	@rm -f *.err
