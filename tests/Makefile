# HustleEngine game makefile template
#
# The engine has a set of makefiles for the various target platforms,
# so the compilation is all taken care of on the engine side.
#
# All that is needed is to list the game's
# header directories and object files, the rest will be taken care of
# in the engine's makefiles.
#

GAME_NAME		:= htest
GAME_INCLUDE	:= .
GAME_SRC		:= main.c simplet.c test.c tmem.c trle.c tasset.c
ENGINE_DIR		:= ..
BUILD_DIR       := build

# *** include the engine's makefile *** #
include $(ENGINE_DIR)/hustle-build.mak  #
# ************************************* #

prebuild:
	@# things that should happen before the compilation
	@echo "** Prebuild **"

postbuild: mainbuild
	@# things that should happen after the compilation
	@echo "** Postbuild **"
	rsync -av RES/ $(BUILD_DIR)/RES/

cleanhook:
	@# add to the `make clean` target
	@echo "** Clean **"
