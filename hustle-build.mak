.PHONY: prebuild postbuild cleanhook preclean engine game

ifeq ($(ENGINE_DIR),)
$(error ENGINE_DIR not set)
endif

COMMON_INCLUDE     = $(ENGINE_DIR)/src
COMMON_ENGINE_OBJS = engine/core.o engine/event.o engine/render.o platform/filesys.o
#COMMON_OBJS 	   = $(addprefix $(ENGINE_DIR)/src,$(COMMON_ENGINE_OBJS))

############################
# Include platform makefile #
ifeq ($(TARGET_PLATFORM), unix)
include unix-makefile
else ifeq ($(TARGET_PLATFORM), dos)
include dos-makefile
else ifeq ($(TARGET_PLATFORM), doswcc)
include wcc-makefile
else
$(error Unsupported platform or TARGET_PLATFORM not specified)
endif
# ************************** #
############################

preclean: cleanhook
	rm -f $(COMMON_OBJS)
	find . -type f -name '._*' -delete

engine: prebuild enginelib
game: engine $(TARGET)
all: postbuild
