.PHONY: tools_clean
.SECONDARY:

TOOLS_SRCDIRS		= asset lib

TOOLS_SRC			= $(foreach dir, $(TOOLS_SRCDIRS), $(wildcard $(TOOLS_DIR)/src/$(dir)/*.c) $(wildcard $(TOOLS_DIR)/src/$(dir)/*.cpp))
TOOLS_OBJ			= $(filter %.o, $(patsubst %.cpp, %.o, $(TOOLS_SRC)) $(patsubst %.c, %.o, $(TOOLS_SRC)))
TOOLS_INCLUDE		= $(TOOLS_DIR)/src

TOOLS_CC			= gcc
TOOLS_CXX			= g++
TOOLS_CFLAGS		= -I$(TOOLS_INCLUDE) -O2 -Wpedantic -Wall -Werror -Isrc
TOOLS_CXXFLAGS		= $(TOOLS_CFLAGS) -std=c++11
TOOLS_LDFLAGS		= -lm

TOOLS_OBJ_DIR		= $(TOOLS_DIR)/bin/obj
TOOLS_CONFIG		= $(TOOLS_OBJ_DIR)/asset_config.o

tools_clean:
	@[[ -z "$(TOOLS_BIN)" ]] || rm -Rf $(TOOLS_BIN)
	@rm -f `find $(TOOLS_DIR) -name "*.o"`
	@echo "Cleaned tools"

%.o: %.c
	@mkdir -p `dirname $@`
	@$(TOOLS_CC) $(TOOLS_CFLAGS) -std=c99 -c $^ -o $@

%.o: %.cpp
	@mkdir -p `dirname $@`
	@$(TOOLS_CXX) $(TOOLS_CXXFLAGS) -c $^ -o $@

$(TOOLS_CONFIG): $(ASSET_CONFIG_FILE)
	@mkdir -p `dirname $@`
	@cat $(TOOLS_DIR)/src/asset_stub.inc $^ > $(TOOLS_OBJ_DIR)/asset_config_gen.c
	@$(TOOLS_CC) $(TOOLS_CFLAGS) -std=c99 -c $(TOOLS_OBJ_DIR)/asset_config_gen.c -o $@

$(TOOLS_BIN)/%: $(TOOLS_DIR)/src/main/%.o $(TOOLS_OBJ) $(TOOLS_CONFIG)
	@mkdir -p `dirname $@`
	@$(TOOLS_CXX) $(TOOLS_CFLAGS) $(TOOLS_LDFLAGS) $< $(TOOLS_OBJ) $(TOOLS_CONFIG) -o $@
	@echo "Built target: $@"

# specialize this specific tool for faster builds,
# (since it does not depend on anything but itself)
$(TOOLS_BIN)/mkbuildconf: $(TOOLS_DIR)/src/main/mkbuildconf.cpp
	@mkdir -p `dirname $@`
	@$(TOOLS_CXX) $(TOOLS_CXXFLAGS) $^ -o $@
	@echo "Built target: $@"
