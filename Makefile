#Q ?= @
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
BUILD_DIR = output
BUILD_DIR_BUILD = output/build
BUILD_DIR_TEST = output/sim
CC_TEST = x86_64-w64-mingw32-gcc
CXX_TEST = x86_64-w64-mingw32-g++
CFLAGS_TEST = -std=c99
CFLAGS_TEST += -Os -Wall
CFLAGS_TEST += -ggdb
LDFLAGS_TEST = -shared

define object_for_dir
$(addprefix $(1)/,$(addsuffix .o,$(basename $(2))))
endef
NWLINK = npx --yes -- nwlink@0.0.19
LINK_GC = 1
LTO = 0

define object_for
$(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(1))))
endef

src = $(addprefix src/,\
	libs/storage.c \
	keyboard.c \
	periodic.c \
	main.c \
)

CFLAGS = -std=c99
CFLAGS += $(shell $(NWLINK) eadk-cflags-device)
CFLAGS += -Os -Wall
CFLAGS += -ggdb
LDFLAGS = -Wl,--relocatable
LDFLAGS += -nostartfiles
LDFLAGS += --specs=nano.specs

ifeq ($(LINK_GC),1)
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,-e,main -Wl,-u,eadk_app_name -Wl,-u,eadk_app_icon -Wl,-u,eadk_api_level
LDFLAGS += -Wl,--gc-sections
endif

ifeq ($(LTO),1)
CFLAGS += -flto -fno-fat-lto-objects
CFLAGS += -fwhole-program
CFLAGS += -fvisibility=internal
LDFLAGS += -flinker-output=nolto-rel
endif

.PHONY: build
build: $(BUILD_DIR_BUILD)/app_stripped.nwa

.PHONY: check
check: $(BUILD_DIR_BUILD)/app_stripped.bin

.PHONY: run
run: $(BUILD_DIR_BUILD)/app_stripped.nwa sim/input.txt
	@echo "INSTALL $<"
	$(Q) $(NWLINK) install-nwa --external-data sim/input.txt $<

.PHONY: test
test: $(BUILD_DIR_TEST)/app.dll sim/input.txt
	@echo "TEST $@"
	$(Q) ./sim/epsilon.exe --nwb $(BUILD_DIR_TEST)/app.dll --nwb-external-data sim/input.txt

$(BUILD_DIR_BUILD)/%.bin: $(BUILD_DIR_BUILD)/%.nwa sim/input.txt
	@echo "BIN     $@"
	$(Q) $(NWLINK) nwa-bin --external-data sim/input.txt $< $@

$(BUILD_DIR_BUILD)/%.elf: $(BUILD_DIR_BUILD)/%.nwa sim/input.txt
	@echo "ELF     $@"
	$(Q) $(NWLINK) nwa-elf --external-data sim/input.txt $< $@

$(BUILD_DIR_BUILD)/app.nwa: $(call object_for_dir,$(BUILD_DIR_BUILD),$(src)) $(BUILD_DIR_BUILD)/icon.o
	@echo "LD      $@"
	$(Q) $(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR_BUILD)/app_stripped.nwa: $(BUILD_DIR_BUILD)/app.nwa | $(BUILD_DIR_BUILD)
	@echo "STRIP   $< -> $@"
	$(Q) arm-none-eabi-strip --strip-unneeded $< -o $@

$(BUILD_DIR_TEST)/app.dll: $(call object_for_dir,$(BUILD_DIR_TEST),$(src)) sim/libepsilon.a
	@echo "LDTEST  $@"
	$(Q) $(CC_TEST) $(CFLAGS_TEST) $(LDFLAGS_TEST) $^ sim/libepsilon.a -o $@

$(addprefix $(BUILD_DIR_BUILD)/,%.o): %.c | $(BUILD_DIR_BUILD)
	@echo "CC      $^"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC) $(CFLAGS) -c $^ -o $@

$(addprefix $(BUILD_DIR_BUILD)/,%.o): %.cpp | $(BUILD_DIR_BUILD)
	@echo "CXX     $^"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CXX) $(CFLAGS) -c $^ -o $@

$(addprefix $(BUILD_DIR_TEST)/,%.o): %.c | $(BUILD_DIR_TEST)
	@echo "CCTEST  $^"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC_TEST) $(CFLAGS_TEST) -c $^ -o $@

$(addprefix $(BUILD_DIR_TEST)/,%.o): %.cpp | $(BUILD_DIR_TEST)
	@echo "CXXTEST $^"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CXX_TEST) $(CFLAGS_TEST) -c $^ -o $@

$(BUILD_DIR_BUILD)/icon.o: assets/icon.png
	@echo "ICON    $<"
	$(Q) $(NWLINK) png-icon-o $< $@

.PRECIOUS: $(BUILD_DIR_BUILD) $(BUILD_DIR_TEST)
$(BUILD_DIR_BUILD):
	$(Q) mkdir -p $@/src

$(BUILD_DIR_TEST):
	$(Q) mkdir -p $@/src

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -rf $(BUILD_DIR_BUILD) $(BUILD_DIR_TEST)