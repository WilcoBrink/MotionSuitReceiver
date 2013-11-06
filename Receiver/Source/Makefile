##############################################################################
# Makefile
# Use with ArmDevEnv by Wisse P.
##############################################################################
NAME		= Project

SRC_PATH		= ./../Source
INC_PATH		= ./../Header
BUILD_PATH		= ./../Build

INCLUDES        = -I $(INC_PATH)

# werken we vanuit ROM of RAM?
# Kies RUN_FROM_ROM (flash jtag of NXP serial tool )) of RUN_FROM_RAM (jtag)
MEM             = RUN_FROM_ROM

# Tool definities
AS              = $(ECHO)arm-none-eabi-as
CC              = $(ECHO)arm-none-eabi-gcc
CPP             = $(ECHO)arm-none-eabi-g++
OBJCOPY         = $(ECHO)arm-none-eabi-objcopy
READELF         = $(ECHO)arm-none-eabi-readelf

# Project files

BIN			= $(BUILD_PATH)/$(NAME).bin
ELF			= $(BUILD_PATH)/$(NAME).elf
MAP			= $(BUILD_PATH)/$(NAME).map
HEX			= $(BUILD_PATH)/$(NAME).hex
ifeq ($(MEM),RUN_FROM_RAM)
LNK             = $(BUILD_PATH)/Linkerscript_RAM
else
LNK             = $(BUILD_PATH)/Linkerscript_ROM
endif

# All .c & .s files in SRC_PATH will be build
MY_SRC		 = $(wildcard $(SRC_PATH)/*.c)
MY_SRC		+= $(wildcard $(SRC_PATH)/*.s)
ALL_SOURCES  =	$(patsubst %.cpp,%.o,\
				$(patsubst %.c,%.o,\
				$(patsubst %.s,%.o,\
				$(patsubst %.S,%.o,\
				$(MY_SRC)))))

# Compiler options
DEFINES         = -D__CPU_MODE__=0

# Welke compiler warnings moeten gegenereerd worden?
WARNINGSETTINGS = -Wall -Wshadow -Wpointer-arith -Wbad-function-cast \
				  -Wcast-align -Wsign-compare -Waggregate-return \
   				  -Wstrict-prototypes -Wmissing-prototypes \
				  -Wmissing-declarations -Wunused

# Hoe worden de c-files gecompileerd?
CFLAGS          = -g -pipe $(WARNINGSETTINGS) -mcpu=arm7tdmi -mtune=arm7tdmi \
				  -mstructure-size-boundary=32 -fpack-struct -fno-builtin \
				   $(INCLUDES) -D $(MEM) $(DEFINES)

# Waar moeten de object-files komen te staan?
LDFLAGS         = -Wl,-Map,$(MAP),-T,$(LNK) -nostartfiles

# Which files can be cleaned?
CLEAN		+= $(ALL_SOURCES) $(BIN) $(ELF) $(MAP) $(HEX)

###############################################################################
# Normal make all
###############################################################################
.PHONY: all
all: $(ELF) $(HEX)

# rule om .elf file te creëren (hier wordt het werkelijke compileerwerk gedaan)
# let op: crt0.o wordt ge-exclude, want die wordt apart gemaakt.
$(ELF):	$(ALL_SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(filter-out %crt0.o,$(ALL_SOURCES)) -o $(ELF)

# forceer ctr0.s met GCC te compileren, en niet met AS,
# hierdoor verschijnt crt0.s in insight
crt0.o: crt0.S
	$(CC) $(CFLAGS) -c crt0.S

# rule om de .hex te maken
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $(ELF) $(HEX)

# Remove output files
.PHONY: clean
clean:
	-$(RM) $(CLEAN)