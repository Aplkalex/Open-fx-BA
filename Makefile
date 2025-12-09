# Open fx-BA project file for fxSDK

ProjectName := Open-fx-BA
InternalName := OPENFXBA

SDK ?= fxsdk

Type := g3a
Icon := icon.png
IconUnselected :=
AppVersion := 1.0.0

CC ?= sh-elf-gcc
LINKER ?= sh-elf-gcc

# C flags
CFLAGS := -std=c11 -Os -Wall -Wextra
CFLAGS += -mb -ffreestanding -nostdlib
CFLAGS += -fstrict-volatile-bitfields

# For development without SDK (testing on Mac/Linux)
# Run: make test
# For actual build: fxsdk build-fx

# Common source files
BASE_SOURCES := \
    src/main.c \
    src/ui.c \
    src/input.c \
    src/tvm.c \
    src/cashflow.c \
    src/memory.c \
    src/keyboard.c \
    src/screens.c \
    src/worksheets.c \
    src/depreciation.c \
    src/bond.c \
    src/statistics.c \
    src/date.c \
    src/features.c \
    src/profit.c \
    src/fonts.c \
    src/display.c \
    src/tests.c

ifeq ($(SDK),casio)
HAL_SOURCES := \
    src/hal/casio/hal_display_casio.c \
    src/hal/casio/hal_keyboard_casio.c \
    src/hal/casio/hal_system_casio.c
PLATFORM_FLAGS := -DUSE_CASIO_SDK
else
HAL_SOURCES := \
    src/hal/fxsdk/hal_display_fxsdk.c \
    src/hal/fxsdk/hal_keyboard_fxsdk.c \
    src/hal/fxsdk/hal_system_fxsdk.c
PLATFORM_FLAGS := -DUSE_FXSDK
endif

SOURCES := $(BASE_SOURCES) $(HAL_SOURCES)
CFLAGS += $(PLATFORM_FLAGS)

# Headers
HEADERS := \
    src/types.h \
    src/config.h \
    src/ui.h \
    src/input.h \
    src/tvm.h \
    src/cashflow.h \
    src/memory.h \
    src/keyboard.h \
    src/screens.h \
    src/worksheets.h \
    src/depreciation.h \
    src/bond.h \
    src/statistics.h \
    src/date.h \
    src/features.h \
    src/profit.h \
    src/tests.h

# Object files
OBJECTS := $(SOURCES:.c=.o)

ifeq ($(SDK),casio)
all: casio-sdk
else
all: fxsdk-build
endif

fxsdk-build:
	fxsdk build-fx

casio-sdk: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -lfx -lm -o $(ProjectName).elf

# Development test (compile for local machine)
test: CFLAGS := -std=c11 -Wall -Wextra -g -DTEST_BUILD
test: CC := gcc
test: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -lm -o fx-ba-test
	./fx-ba-test

# Run CFA validation tests
cfa-test: CFLAGS := -std=c11 -Wall -Wextra -g -DTEST_BUILD
cfa-test: CC := gcc
cfa-test: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -lm -o fx-ba-test
	./fx-ba-test --test

# Clean
clean:
	rm -f $(OBJECTS) fx-ba-test
	rm -rf build-fx
	rm -f $(ProjectName).elf

.PHONY: all fxsdk-build casio-sdk test cfa-test clean
