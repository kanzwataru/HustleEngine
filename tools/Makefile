.phony: all clean

BIN_DIR 	:= build/bin
OBJ_DIR 	:= build/temp
TARGETS		:= mkasset mkpak mkmak

MKASSET_SRC	:= mkasset.cpp
MKPAK_SRC	:= mkpak.cpp
MKMAK_SRC	:= mkmak.cpp

INCLUDE		:= src/

CC		:= gcc
CXX		:= g++

LDFLAGS		:= 
CFLAGS		:= -O2 -Wall
CXXFLAGS	:= $(CFLAGS) -std=c++14

# internal #
TARGETS		:= $(patsubst %, $(BIN_DIR)/%, $(TARGETS))
MKASSET_OBJ	:= $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(MKASSET_SRC))
MKPAK_OBJ	:= $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(MKPAK_SRC))
MKMAK_OBJ	:= $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(MKMAK_SRC))
# ######## #

all: $(TARGETS)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p `dirname $@`
	@$(CXX) -c $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/mkasset: $(MKASSET_OBJ)
	@mkdir -p `dirname $@`
	@$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

$(BIN_DIR)/mkpak: $(MKPAK_OBJ)
	@mkdir -p `dirname $@`
	@$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

$(BIN_DIR)/mkmak: $(MKMAK_OBJ)
	@mkdir -p `dirname $@`
	@$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
	
clean:
	@rm -Rf build/*
