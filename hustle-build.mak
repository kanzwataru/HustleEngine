.PHONY: prebuild postbuild cleanhook preclean engine game
.DEFAULT_GOAL := all

ifeq ($(ENGINE_DIR),)
$(error ENGINE_DIR not set)
endif

COMMON_INCLUDE     = $(ENGINE_DIR)/src
COMMON_ENGINE_OBJS = engine/core.o engine/event.o engine/render.o platform/filesys.o
#COMMON_OBJS 	   = $(addprefix $(ENGINE_DIR)/src,$(COMMON_ENGINE_OBJS))

############################
# Include platform makefile #
ifeq ($(TARGET_PLATFORM), unix)
include $(ENGINE_DIR)/makefiles/unix-platform.mak
else ifeq ($(TARGET_PLATFORM), dos)
include $(ENGINE_DIR)/makefiles/dos-platform.mak
else
$(error Unsupported platform or TARGET_PLATFORM not specified)
endif
# ************************** #
############################

preclean: cleanhook
	rm -f $(COMMON_OBJS)
	find . -type f -name '._*' -delete

mainbuild: prebuild
	@$(MAKE) --no-print-directory build

all: postbuild
