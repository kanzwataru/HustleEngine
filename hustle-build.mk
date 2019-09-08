.PHONY: all engine assets tools game clean cleanhook prebuild postbuild run
.DEFAULT_GOAL := all

INCLUDE		:= $(ENGINE_DIR)/src/engine
ENGINE_SRC	 =

all: postbuild

clean: cleanhook

run: all

game: assets

assets: engine tools

engine: prebuild

tools:
