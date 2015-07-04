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

LLC= llc
CLANG= clang

# output directory
OUT= build

APP_SRC_DIR= src

# recursive wildcard function
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

INC= -Isrc -Isrc/device/inc -Isrc/cmsis/inc -Isrc/dsp/inc

# -flto is needed for compiling and linking
ARCH= -march=armv7e-m -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -flto
OPT= -Os -ffunction-sections -fdata-sections -fno-exceptions
CWARN= -Wall -Wextra -Wdouble-promotion -Wmissing-field-initializers \
  -Werror=implicit-function-declaration
LDSCRIPT= stm32f4.ld

ASMFLAGS= $(ARCH)
CFLAGS= $(ARCH) $(OPT) $(CWARN) $(INC) -std=gnu99 
CXXFLAGS= $(ARCH) $(OPT) $(CWARN) $(INC) -std=gnu++11
LDFLAGS= $(ARCH) --specs=nano.specs -nostartfiles -static -Wl,--gc-sections -T $(LDSCRIPT) 

# CLANG-flags
GCC_INC= ./$(TRIPLE)-gcc/$(TRIPLE)/include
LLVM_INC= -isystem $(GCC_INC) $(INC)
LLVM_ARCH= -fgnu-keywords -ffunction-sections -fdata-sections -m32
LLVM_OPT= -Os -g -emit-llvm
LLVM_CXX_SYS_INC= -cxx-isystem $(GCC_INC)/c++/4.9.3 -cxx-isystem $(GCC_INC)/c++/4.9.3/$(TRIPLE)/armv7e-m
LLVM_CFLAGS= $(LLVM_ARCH) $(LLVM_INC) $(LLVM_OPT) -std=c99
LLVM_CXXFLAGS= $(LLVM_ARCH) $(LLVM_CXX_SYS_INC) $(LLVM_INC) $(LLVM_OPT) -fno-exceptions -std=c++11
LLVM_LLCFLAGS= -mtriple=arm-none-eabi

APP_ASM_SRC= $(wildcard $(APP_SRC_DIR)/*.s)
APP_C_SRC= $(wildcard $(APP_SRC_DIR)/*.c)
APP_CXX_SRC= $(wildcard $(APP_SRC_DIR)/*.cpp)

#APP_LLVM_C_BC= $(APP_C_SRC:%.c=$(OUT)/%.bc)
#APP_LLVM_CXX_BC= $(APP_CXX_SRC:%.cpp=$(OUT)/%.bc)
#APP_LLVM_ASM= $(APP_LLVM_C_BC:%.bc=%.s) $(APP_LLVM_CXX_BC:%.bc=%.s)

APP_ASM_OBJ= $(APP_ASM_SRC:%.s=$(OUT)/%.o) #$(APP_LLVM_ASM:%.s=$(OUT)/%.o)
APP_C_OBJ= $(APP_C_SRC:%.c=$(OUT)/%.o)
APP_CXX_OBJ= $(APP_CXX_SRC:%.cpp=$(OUT)/%.o)

OBJ= $(APP_ASM_OBJ) $(APP_C_OBJ) $(APP_CXX_OBJ)

ELF= $(OUT)/noisecancel.elf
HEX= $(OUT)/noisecancel.hex
BIN= $(OUT)/noisecancel.bin

# suppress echoing the command invocation
#Q= @
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
	$(Q) $(LD) -o $@ $(LDFLAGS) $(OBJ)

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

# Cpp compilation rule
$(APP_CXX_OBJ): $(OUT)/%.o: %.cpp Makefile
	$(E) "CC        $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(CXX) -o $@ -c -MMD -MP $(CXXFLAGS) -frandom-seed=$< $<

$(APP_LLVM_C_BC): $(OUT)/%.bc: %.c Makefile
	$(E) "LLVM CC   $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(CLANG) $(LLVM_CFLAGS) -o $@ -c -MMD -MP $<

$(APP_LLVM_CXX_BC): $(OUT)/%.bc: %.cpp Makefile
	$(E) "LLVM CXX  $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(CLANG) $(LLVM_CXXFLAGS) -o $@ -c -MMD -MP $<

$(APP_LLVM_ASM): %.s: %.bc Makefile
	$(E) "OPT       $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(LLC) $(LLVM_LLCFLAGS) -o $@ $<

# disables all builtin suffix-rules, which we don't need anyway. 
.SUFFIXES:
