.PHONY: all clean
.PRECIOUS: bin/%.o

ASSET_CONFIG_FILE ?= dummy_asset_conf.c  # needs to be set externally

SRCDIRS 	= asset
TARGETS 	= $(patsubst src/main/%.cpp, %, $(wildcard src/main/*.cpp))

SRC			= $(foreach dir, $(SRCDIRS), $(wildcard src/$(dir)/*.c) $(wildcard src/$(dir)/*.cpp))
HDR			= $(foreach dir, $(SRCDIRS), $(wildcard src/$(dir)/*.h) $(wildcard src/$(dir)/*.hpp))
OBJ			= $(filter %.o, $(patsubst %.cpp, bin/%.o, $(SRC)) $(patsubst %.c, bin/%.o, $(SRC)))

CC			= clang
CXX			= clang++
CFLAGS		= -O2 -Wpedantic -Wall -Werror -Isrc
CXXFLAGS	= $(CFLAGS) -std=c++11
LDFLAGS 	= -lm

MAINC		= $(patsubst %,src/main/%.cpp,$(TARGETS))
OUT			= $(patsubst %,bin/%, $(TARGETS))

all: $(OUT)

clean:
	@rm -Rf bin
	@echo "Cleaned tools"

bin/%.o: src/%.c
	@mkdir -p `dirname $@`
	@$(CC) $(CFLAGS) -std=c99 -c $^ -o $@

bin/%.o: src/%.cpp
	@mkdir -p `dirname $@`
	@$(CXX) $(CXXFLAGS) -c $^ -o $@

bin/asset_config.o: $(ASSET_CONFIG_FILE)
	@mkdir -p `dirname $@`
	@cat src/asset_stub.inc $^ > bin/asset_config_gen.c
	@$(CC) $(CFLAGS) -std=c99 -c bin/asset_config_gen.c -o $@

bin/%: bin/main/%.o $(OBJ) $(HDR) bin/asset_config.o
	@mkdir -p `dirname $@`
	@$(CXX) $(CFLAGS) $(LDFLAGS) $< $(OBJ) bin/asset_config.o -o $@
	@echo "Built target: $@"
