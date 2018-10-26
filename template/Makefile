# HustleEngine game makefile template
#
# The engine has a set of makefiles for the various target platforms,
# so the compilation is all taken care of on the engine side.
# 
# All that is needed is to list the game's 
# header directories and object files, the rest will be taken care of
# in the engine's makefiles.
#

GAME_NAME		:= EXAMPLE
GAME_INCLUDE	:= src/game
GAME_OBJS		:= src/game/foo.o src/main.o
ENGINE_DIR		:= HustleEngine
BUILD_DIR       := build

# *** include the engine's makefile *** #
include $(ENGINE_DIR)/hustle-build.mak  #
# ************************************* #

prebuild:
	# things that should happen before the compilation
	echo "** Prebuild **"

postbuild:　game
	# things that should happen after the compilation
	echo "** Postbuild **"

cleanhook:
	# add to the `make clean` target
	echo "** Clean **"
