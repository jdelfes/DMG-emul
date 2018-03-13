CC := gcc
LD := gcc

TARGET := DMG-emul
BUILDDIR_TARGET := build

CFLAGS  := -O2 $(shell sdl2-config --cflags)
LDFLAGS := $(shell sdl2-config --libs)

MODULES   := headers core cpu mbc snd
SRC_DIR   := $(addprefix src/,$(MODULES))
BUILD_DIR := $(addprefix $(BUILDDIR_TARGET)/,$(MODULES))

SRC      := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJ      := $(patsubst src/%.c,$(BUILDDIR_TARGET)/%.o,$(SRC))
INCLUDES := $(addprefix -I,$(SRC_DIR))

vpath %.c $(SRC_DIR)

define make-goal
$1/%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs $(BUILDDIR_TARGET)/$(TARGET)

$(BUILDDIR_TARGET)/$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) $^ -o $@

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -f $(BUILDDIR_TARGET)/$(TARGET)
	@rm -rf $(BUILD_DIR)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))

