.PHONY: preclean

COMMON_HEADERS     = src/HustleEngine src/WreckingBalloon src
COMMON_GAME_OBJS   = balloon.o cactoon.o resource.o wb.o
COMMON_ENGINE_OBJS = engine/core.o engine/event.o engine/render.o platform/filesys.o

COMMON_OBJS 	   = $(addprefix src/HustleEngine/,$(COMMON_ENGINE_OBJS))
COMMON_OBJS       += $(addprefix src/WreckingBalloon/,$(COMMON_GAME_OBJS))
COMMON_OBJS       += src/main.o src/test.o src/simplet.o

ifeq ($(TARGET_PLATFORM), unix)
include unix-makefile
else ifeq ($(TARGET_PLATFORM), dos)
include dos-makefile
else ifeq ($(TARGET_PLATFORM), doswcc)
include wcc-makefile
else
$(error Unsupported platform or TARGET_PLATFORM not specified)
endif

preclean:
	rm -f $(COMMON_OBJS)
	find . -type f -name '._*' -delete
