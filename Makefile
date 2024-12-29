

.PHONY: build clean


# NOTE: The below code is not actually needed as make is always executed in
# the working directory. It remains as a reference for make features.
# NOTE: MAKEFILE_LIST is a special variable representing the Makefiles 
# which have been parsed. We use this to get the project root directory.
PROJECT_ROOT := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ifeq ($(PROJECT_ROOT), $(shell pwd))
	PROJECT_ROOT= .
endif

RAYLIB_PATH := $(PROJECT_ROOT)/../../raylib

# Our header file location
INCLUDE_DIRS := $(PROJECT_ROOT)/src
# External header file locations
INCLUDE_DIRS += $(shell find $(PROJECT_ROOT)/include -type d)

INCLUDE_OPTS = $(addprefix -I,$(shell find $(PROJECT_ROOT)/include -type d))
LIB_PATH := $(PROJECT_ROOT)/lib

# DEFAULT COMPILER
CC := gcc

# COMPILER FLAGS
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result

# LINKER FLAGS
LDLIBS = -lraylib -lGLX -lm -lpthread -ldl -lrt -lc
LDFLAGS = -L$(PROJECT_ROOT)/src -L$(LIB_PATH) -L$(RAYLIB_PATH)/src

debug: $(PROJECT_ROOT)/src/boids.c
	$(CC) -g -o ./bin/boids $< $(CFLAGS) $(INCLUDE_OPTS) $(LDFLAGS) $(LDLIBS)
