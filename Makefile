CROSS= ./arm-none-eabi-gcc/bin/arm-none-eabi-
LD= $(CROSS)gcc
CC= $(CROSS)gcc
CXX= $(CROSS)g++
OBJCOPY= $(CROSS)objcopy
OBJDUMP= $(CROSS)objdump
SIZE= $(CROSS)size
NM= $(CROSS)nm
OPENOCD= openocd

# recursive wildcard function
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

INC= -Isrc -Isrc/device/inc -Isrc/cmsis/inc -Isrc/dsp/inc

ARCH= -mno-thumb-interwork -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -fno-exceptions
OPT= -Os
CWARN= -Wall -Wmissing-field-initializers -Werror=implicit-function-declaration
LDSCRIPT= stm32f4.ld

ASMFLAGS= $(ARCH)
CFLAGS= $(ARCH) $(OPT) $(CWARN) $(INC) -std=gnu99 
CXXFLAGS= $(ARCH) $(OPT) $(CWARN) $(INC) -std=gnu++11
LDFLAGS= $(ARCH) --specs=nano.specs -static -Wl,--gc-sections -T $(LDSCRIPT)

# output directory
TMP= build

APP_SRC_DIR= src

APP_ASM_SRC= $(wildcard $(APP_SRC_DIR)/*.s)
APP_C_SRC= $(wildcard $(APP_SRC_DIR)/*.c)
APP_CXX_SRC= $(wildcard $(APP_SRC_DIR)/*.cpp)

APP_ASM_OBJ= $(APP_ASM_SRC:%.s=$(TMP)/%.o)
APP_C_OBJ= $(APP_C_SRC:%.c=$(TMP)/%.o)
APP_CXX_OBJ= $(APP_CXX_SRC:%.cpp=$(TMP)/%.o)

OBJ= $(APP_ASM_OBJ) $(APP_C_OBJ) $(APP_CXX_OBJ)

ELF= $(TMP)/noisecancel.elf
HEX= $(TMP)/noisecancel.hex
BIN= $(TMP)/noisecancel.bin

# suppress echoing the command invocation
Q= @
# silent sub-make
S= -s
E= @echo

# bootloader is phony because it's managed by another makefile
.PHONY: all flash erase clean

# just compile, flashing is on a separate target. Keep generated code.
all: $(ELF) $(BIN) $(HEX) $(TMP)/symbolsizes.txt $(TMP)/disasm.txt $(TMP)/headers.txt
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
	-rm -rf $(TMP)

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
	
$(TMP)/headers.txt: $(ELF)
	$(Q) mkdir -p $(@D)
	$(Q) $(OBJDUMP) -x $< > $@

$(TMP)/disasm.txt: $(ELF)
	$(E) "DISASM    $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(OBJDUMP) -d $< > $@

$(TMP)/symbols.txt: $(ELF)
	$(Q) mkdir -p $(@D)
	$(Q) $(OBJDUMP) -t $< > $@

$(TMP)/symbolsizes.txt: $(ELF)
	$(E) "SYMSIZES  $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(NM) --reverse-sort --print-size --size-sort $< > $@

# header dependencies
-include $(OBJ:%.o=%.d)

# application assembly compilation rule (startup code)
$(APP_ASM_OBJ): $(TMP)/%.o: %.s Makefile
	$(E) "ASM       $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(CC) -o $@ -c -MMD -MP $(ASMFLAGS) $<

# C compilation rule
$(APP_C_OBJ): $(TMP)/%.o: %.c Makefile
	$(E) "CC        $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(CC) -o $@ -c -MMD -MP $(CFLAGS) -frandom-seed=$< $<

# Cpp compilation rule
$(APP_CXX_OBJ): $(TMP)/%.o: %.cpp Makefile
	$(E) "CC        $@"
	$(Q) mkdir -p $(@D)
	$(Q) $(CXX) -o $@ -c -MMD -MP $(CXXFLAGS) -frandom-seed=$< $<

# disables all builtin suffix-rules, which we don't need anyway. 
.SUFFIXES:
