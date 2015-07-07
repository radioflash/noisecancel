TRIPLE= arm-none-eabi
CROSS= ./$(TRIPLE)-gcc/bin/$(TRIPLE)-
LD= $(CROSS)gcc
CC= $(CROSS)gcc
CXX= $(CROSS)g++
OBJCOPY= $(CROSS)objcopy
OBJDUMP= $(CROSS)objdump
SIZE= $(CROSS)size
NM= $(CROSS)nm
OPENOCD= openocd

# output directory
OUT= build

APP_SRC_DIR= src
HAL_SRC_DIR= src/device/src

# recursive wildcard function
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

INC= -Isrc -Isrc/device/inc -Isrc/cmsis/inc -Isrc/dsp/inc

# -flto is needed for compiling and linking
ARCH= -march=armv7e-m -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -flto
OPT= -Os -g -ffunction-sections -fdata-sections
CWARN= -Wall -Wextra -Wdouble-promotion -Wmissing-field-initializers \
  -Werror=implicit-function-declaration
LDSCRIPT= stm32f4.ld

ASMFLAGS= $(ARCH)
CFLAGS= $(ARCH) $(OPT) $(CWARN) $(INC) -std=gnu99 
CXXFLAGS= $(ARCH) $(OPT) $(CWARN) $(INC) -fno-exceptions -std=gnu++11
LDFLAGS= $(ARCH) -g --specs=nano.specs -nostartfiles -static -Wl,--gc-sections -T $(LDSCRIPT) -L. -lPDMFilter_CM4F_GCC 

APP_ASM_SRC= $(wildcard $(APP_SRC_DIR)/*.s)
APP_C_SRC= $(wildcard $(APP_SRC_DIR)/*.c)
APP_CXX_SRC= $(wildcard $(APP_SRC_DIR)/*.cpp)

HAL_C_SRC= $(wildcard $(HAL_SRC_DIR)/*.c)

APP_ASM_OBJ= $(APP_ASM_SRC:%.s=$(OUT)/%.o) #$(APP_LLVM_ASM:%.s=$(OUT)/%.o)
APP_C_OBJ= $(APP_C_SRC:%.c=$(OUT)/%.o)
APP_CXX_OBJ= $(APP_CXX_SRC:%.cpp=$(OUT)/%.o)

HAL_C_OBJ= $(HAL_C_SRC:%.c=$(OUT)/%.o)

OBJ= $(APP_ASM_OBJ) $(APP_C_OBJ) $(APP_CXX_OBJ) $(HAL_C_OBJ)

ELF= $(OUT)/noisecancel.elf
HEX= $(OUT)/noisecancel.hex
BIN= $(OUT)/noisecancel.bin

# suppress echoing the command invocation
Q= @
# silent sub-make
S= -s
E= @echo

# bootloader is phony because it's managed by another makefile
.PHONY: all flash erase clean

# just compile, flashing is on a separate target. Keep generated code.
all: $(ELF) $(BIN) $(HEX) $(OUT)/symbolsizes.txt $(OUT)/disasm.txt $(OUT)/headers.txt
	$(Q) $(SIZE) $(ELF)

erase:
	$(Q) $(OPENOCD) -f openocd.cfg -c "init; reset init; stm32f4x mass_erase 0; shutdown;"

# the --reset-option of the st-flash tool always jumps into recently flashed
# code which does not work for us because we rely on the bootloader for
# setting things up. 
flash: $(BIN)
	$(E) "FLASHING" 
	$(Q) $(OPENOCD) -f openocd.cfg -c "program $(BIN) 0x08000000 verify reset"

clean:
	-rm -rf $(OUT)

$(ELF): $(OBJ) $(LDSCRIPT) Makefile
	$(Q) mkdir -p $(@D)
	$(E) "LD ELF    $@" $(OBJ)
	$(Q) $(LD) -o $@ $(OBJ) $(LDFLAGS)

$(BIN): $(ELF)
	$(Q) mkdir -p $(@D)
	$(E) "OBJCOPY   $@"
	$(Q) $(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(Q) mkdir -p $(@D)
	$(E) "OBJCOPY   $@"
	$(Q) $(OBJCOPY) -O ihex $< $@
	
$(OUT)/headers.txt: $(ELF)
	$(Q) mkdir -p $(@D)
	$(Q) $(OBJDUMP) -x $< > $@

$(OUT)/disasm.txt: $(ELF)
	$(E) "DISASM    $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(OBJDUMP) -d $< > $@

$(OUT)/symbols.txt: $(ELF)
	$(Q) mkdir -p $(@D)
	$(Q) $(OBJDUMP) -t $< > $@

$(OUT)/symbolsizes.txt: $(ELF)
	$(E) "SYMSIZES  $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(NM) --reverse-sort --print-size --size-sort $< > $@

# header dependencies
-include $(OBJ:%.o=%.d)

# application assembly compilation rule (startup code)
$(APP_ASM_OBJ): $(OUT)/%.o: %.s Makefile
	$(E) "ASM       $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(CC) -o $@ -c -MMD -MP $(ASMFLAGS) $<

# C compilation rule
$(APP_C_OBJ): $(OUT)/%.o: %.c Makefile
	$(E) "CC        $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(CC) -o $@ -c -MMD -MP $(CFLAGS) -frandom-seed=$< $<
	
$(HAL_C_OBJ): $(OUT)/%.o: %.c Makefile
	$(E) "CC        $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(CC) -o $@ -c -MMD -MP $(CFLAGS) -frandom-seed=$< $<

# Cpp compilation rule
$(APP_CXX_OBJ): $(OUT)/%.o: %.cpp Makefile
	$(E) "CPP       $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(CXX) -o $@ -c -MMD -MP $(CXXFLAGS) -frandom-seed=$< $<

# disables all builtin suffix-rules, which we don't need anyway. 
.SUFFIXES:
