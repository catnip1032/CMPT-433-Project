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
# Runs a script to determine the existing C compiler in the machine. This is
# necessary because a group member (guess who?) uses an alternative compiler
# out of unnecessary rebelliousness (:
CC := $(shell \
			if command -v arm-none-linux-gnueabihf-gcc &> /dev/null;\
			then\
					echo "arm-none-linux-gnueabihf-gcc";\
			else\
					echo "arm-linux-gnueabihf-gcc";\
			fi)

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
	$(CC) $(CFLAGS) -c $< -o $@

## Phony targets
## ----------------------------------------------------------------------------
# Phony targets are sus targets. No, jk. These are recipes that don't actually
# build anything. They are used to run some script, like cleaning stuff.
.PHONY: clean

clean:
	rm -f $(TARGET) $(TEST_BIN) $(OBJS)
