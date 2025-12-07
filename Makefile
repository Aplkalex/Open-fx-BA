# Open fx-BA project file for fxSDK

ProjectName := Open-fx-BA
InternalName := OPENFXBA

Type := g3a
Icon := icon.png
IconUnselected :=
AppVersion := 1.0.0

Compiler := sh-elf-gcc
Linker := sh-elf-gcc

# C flags
CFLAGS := -std=c11 -Os -Wall -Wextra
CFLAGS += -mb -ffreestanding -nostdlib
CFLAGS += -fstrict-volatile-bitfields

# For development without SDK (testing on Mac/Linux)
# Run: make test
# For actual build: fxsdk build-fx

# Source files
SOURCES := \
    src/main.c \
    src/ui.c \
    src/input.c \
    src/tvm.c \
    src/cashflow.c

# Headers
HEADERS := \
    src/types.h \
    src/config.h \
    src/ui.h \
    src/input.h \
    src/tvm.h \
    src/cashflow.h

# Object files
OBJECTS := $(SOURCES:.c=.o)

# Default target (for fxSDK)
all:
	fxsdk build-fx

# Development test (compile for local machine)
test: CFLAGS := -std=c11 -Wall -Wextra -g -DTEST_BUILD
test: CC := gcc
test: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -lm -o fx-ba-test
	./fx-ba-test

# Clean
clean:
	rm -f $(OBJECTS) fx-ba-test
	rm -rf build-fx

.PHONY: all test clean
