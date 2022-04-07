## Definitions
## ----------------------------------------------------------------------------
APPNAME=recycler

## Directories
CMPT433_DIR = $(HOME)/cmpt433
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
TEST_DIR=$(CMPT433_DIR)/public/tests
TARGET_DIR = $(CMPT433_DIR)/public/myApps

## C Compiler
# Runs a script to determine the installed C compiler on the machine.
# Some group members use arm-none-linux-gnueabihf-gcc as an alternative,
# so this is necessary. If arm-none-linux-gnueabihf-gcc is not found,
# arm-linux-gnueabihf-gcc will be used instead.
ifeq (, $(shell arm-linux-gnueabihf-gcc --version &> /dev/null))
	CC := arm-none-linux-gnueabihf-gcc
else
	CC := arm-linux-gnueabihf-gcc
endif

## Compilation Flags
LFLAGS = -lpthread
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror $(LFLAGS) 

## Files
SRCS = $(wildcard $(SRC_DIR)/*.c)
TEST_SRC = test/test.c
HEADERS = $(filter-out include/main.h,$(patsubst src/%.c, include/%.h, $(SRCS)))
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

## Binaries
TARGET = $(TARGET_DIR)/$(APPNAME)
TEST_BIN=$(TEST_DIR)/test_$(APPNAME)

## Recipes
## ----------------------------------------------------------------------------
all: $(TARGET)

test: $(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(OBJS)
	if [ ! -d "$(TEST_DIR)" ]; \
	then \
		mkdir -p $(TEST_DIR);\
	fi
	$(CC) $(CFLAGS) $(TEST_SRC) $(HEADERS) $(filter-out build/main.o,$(OBJS)) \
		-o $@

$(TARGET): $(OBJS)
	if [ ! -d "$(TARGET_DIR)" ]; \
	then \
		mkdir -p $(TARGET_DIR);\
	fi
	$(CC) $(CFLAGS) $(OBJS) -o $@

build/%.o: $(SRC_DIR)/%.c $(wildcard $(INCLUDE_DIR)/%.h)
	@echo $(CC)
	$(CC) $(CFLAGS) -c $< -o $@

## Phony targets
## ----------------------------------------------------------------------------
# Phony targets are sus targets. No, jk. These are recipes that don't actually
# build anything. They are used to run some script, like cleaning stuff.
.PHONY: clean

clean:
	rm -f $(TARGET) $(TEST_BIN) $(OBJS)
