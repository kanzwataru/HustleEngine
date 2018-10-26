.PHONY: buildnrun run

# Cross-compile for DOS with OpenWatcom
ENGINE_OBJS = $(COMMON_ENGINE_OBJS) platform/dos/vga.o platform/dos/kb.o
OBJS		= $(GAME_OBJS) $(addprefix $(ENGINE_DIR)/src/,$(ENGINE_OBJS))
SRCS		= $(OBJS:.o=.c)
INCLUDE		= $(COMMON_INCLUDE) $(GAME_INCLUDE)
LIBS		= mathc.lib
TARGET		= $(GAME_NAME).exe

CFLAGS 		= -3 -bt=dos -ml -w4 -zq
ifeq ($(DEBUG_BUILD), 1)
CFLAGS	   += -od -d2 -dDEBUG
else
CFLAGS	   += -otexan -d0
endif
CFLAGS	   += $(addprefix -i,$(INCLUDE))
LDFLAGS		= -fe=$(TARGET)

CC			= wcl

%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -fo=$@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS)
	
	# clean up unnecessary crud that OpenWatcom leaves behind
	rm -f *.err

enginelib:
	echo "Building HustleEngine"

buildnrun: postbuild
	dosbox -c "cd C:\DEV\BALLOON" -c "balloon.exe"

run:
	dosbox -c "cd C:\DEV\BALLOON" -c "balloon.exe"

clean: preclean
	rm -f $(OBJS) $(TARGET)
	rm -f *.obj *.err