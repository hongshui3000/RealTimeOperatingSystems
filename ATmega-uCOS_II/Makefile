# Hey Emacs, this is a -*- makefile -*-
# By 	  : Gary Lee
# <E-mail : dreamer2006@163.com>
#
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF.
#
# make extcoff = Convert ELF to AVR Extended COFF.
#
# make program = Download the hex file to the device, using avrdude.
#                Please customize the avrdude settings below first!
#
# make debug = Start either simulavr or avarice as specified for debugging, 
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------

# MCU name
MCU = atmega128

# Processor frequency.
F_CPU = 8000000

export MCU F_CPU
# Output format. (can be srec, ihex, binary)
FORMAT = ihex

# Optimization level, can be [0, 1, 2, 3, s]. 
#     0 = turn off optimization. s = optimize for size.
OPT = s

# Target file name (without extension).
TARGET = atmega128_ucos2

# Object files directory
#     To put object files in current directory, use a dot (.), do NOT make
#     this an empty or blank macro!
OUTPUT  = objs
INCDIR  = inc
SUBDIRS = src
TOPDIR  = .

export OUTPUT INCDIR TOPDIR
# List C source files here. (C dependencies are automatically generated.)
SRC = src/os/os_core.c 		\
	  src/os/os_dbg_r.c 	\
	  src/os/os_flag.c 		\
	  src/os/os_mbox.c 		\
	  src/os/os_mem.c 		\
	  src/os/os_mutex.c 	\
	  src/os/os_q.c 		\
	  src/os/os_sem.c 		\
	  src/os/os_task.c 		\
	  src/os/os_time.c 		\
	  src/os/os_tmr.c 		\
	  src/avr/os_cpu_c.c 	\
	  app/main.c 			\
	  app/board.c			\
	  app/cmd_queue.c		\
	  app/data_queue.c		\
	  app/uart.c			\
	  app/res_control.c		\
	  app/timer.c			\
	  app/adc.c				\
	  app/measure.c			\
	  app/extint.c			\
	  app/i2c.c				\
	  app/pcf8563.c			\
	  app/key.c


# List Assembler source files here.
ASRC = src/avr/os_cpu_a.S

# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
DEBUG = dwarf-2

# List any extra directories to look for include files here.
# 用于添加额外include头文件
EXTRAINCDIRS = 

# Compiler flag to set the C Standard level.
#     c89   = "ANSI" C
#     gnu89 = c89 plus GCC extensions
#     c99   = ISO C99 standard (not yet fully implemented)
#     gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here for C sources
CDEFS = -DF_CPU=$(F_CPU)UL

# Place -D or -U options here for ASM sources
ADEFS = -DF_CPU=$(F_CPU)

#---------------- Compiler Options C ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(<:%.c=$(OUTPUT)/%.lst)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)

export CFLAGS
#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -adhlns:   create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
#  -listing-cont-lines: Sets the maximum number of continuation lines of hex 
#       dump that will be displayed for a given single line of source input.
ASFLAGS = $(ADEFS) -Wa,-adhlns=$(<:%.S=$(OUTPUT)/%.lst),-gstabs,--listing-cont-lines=100

#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB = 
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)

# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB = 
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)

MATH_LIB = -lm

# List any extra directories to look for libraries here.
# 添加额外库文件
EXTRALIBDIRS = 

#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(patsubst %,-L%,$(EXTRALIBDIRS))
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
#LDFLAGS += -T linker_script.x

# Define programs and commands.
SHELL 		= sh
CC 			= avr-gcc
OBJCOPY 	= avr-objcopy
OBJDUMP 	= avr-objdump
SIZE 		= avr-size
AR 			= avr-ar rcs
NM 			= avr-nm
AVRDUDE 	= avrdude
REMOVE 		= rm -f
REMOVEDIR 	= rm -rf
COPY 		= cp
WINSHELL 	= cmd

export CC SHELL OBJCOPY OBJDUMP SIZE AR NM REMOVE REMOVEDIR COPY WINSHELL

# Define Messages
# English
MSG_ERRORS_NONE 	= Errors: none
MSG_BEGIN 			= -------- 编译开始 --------
MSG_END 			= -------- 编译结束  --------
MSG_SIZE_BEFORE 	= Size before: 
MSG_SIZE_AFTER 		= Size after:
MSG_COFF 			= Converting to AVR COFF:
MSG_EXTENDED_COFF 	= Converting to AVR Extended COFF:
MSG_FLASH 			= Creating load file for Flash:
MSG_EEPROM 			= Creating load file for EEPROM:
MSG_EXTENDED_LISTING= Creating Extended Listing:
MSG_SYMBOL_TABLE 	= Creating Symbol Table:
MSG_LINKING 		= Linking:
MSG_COMPILING 		= Compiling C:
MSG_COMPILING_CPP 	= Compiling C++:
MSG_ASSEMBLING 		= Assembling:
MSG_CLEANING 		= Cleaning project:
MSG_CREATING_LIBRARY= Creating library:

export MSG_COMPILING MSG_COMPILING_CPP MSG_ASSEMBLING

# 列出所有的目标文件 *.o
OBJ = $(SRC:%.c=$(OUTPUT)/%.o) $(ASRC:%.S=$(OUTPUT)/%.o) 

# 列出所有的列表文件 *.lst
LST = $(SRC:%.c=$(OUTPUT)/%.lst) $(ASRC:%.S=$(OUTPUT)/%.lst) 

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS 	 = -mmcu=$(MCU) -I./$(INCDIR) $(CFLAGS) $(GENDEPFLAGS)
ALL_CPPFLAGS = -mmcu=$(MCU) -I./$(INCDIR) -x c++ $(CPPFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS  = -mmcu=$(MCU) -I./$(INCDIR) -x assembler-with-cpp $(ASFLAGS)

export ALL_CFLAGS ALL_CPPFLAGS ALL_ASFLAGS

# Default target.
all: begin gccversion sizebefore build sizeafter end

# Eye candy.
begin:
	@echo
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)
	@echo		
	
# Display compiler version information.
gccversion : 
	@$(CC) --version

sizebefore:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); \
	2>/dev/null; echo; fi
	
sizeafter:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); $(ELFSIZE2);\
	$(AVRMEM) 2>/dev/null; echo; fi
	
# Change the build target to build a HEX file or a library.
build: elf hex eep lss sym
#build: lib

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym
LIBNAME=lib$(TARGET).a
lib: $(LIBNAME)

# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf
ELFSIZE2 = $(SIZE) --mcu=$(MCU) --format=avr $(TARGET).elf

# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT = $(OBJCOPY) --debugging
COFFCONVERT += --change-section-address .data-0x800000
COFFCONVERT += --change-section-address .bss-0x800000
COFFCONVERT += --change-section-address .noinit-0x800000
COFFCONVERT += --change-section-address .eeprom-0x810000

coff: $(TARGET).elf
	@echo
	@echo $(MSG_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-avr $< $(TARGET).cof

extcoff: $(TARGET).elf
	@echo
	@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof

# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock $< $@

%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O $(FORMAT) $< $@ || exit 0

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S -z $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@

# Create library from object files.
.SECONDARY : $(TARGET).a
.PRECIOUS : $(OBJ)
%.a: $(OBJ)
	@echo
	@echo $(MSG_CREATING_LIBRARY) $@
	$(AR) $@ $(OBJ)

# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)

# Target: clean project.
clean: begin clean_list end

clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).eep
	$(REMOVE) $(TARGET).cof
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(SRC:%.c=$(OUTPUT)/%.o)
	$(REMOVE) $(SRC:%.c=$(OUTPUT)/%.lst)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) $(SRC:.c=.i)
	$(REMOVEDIR) .dep
	$(REMOVEDIR) ./objs

# Create object files directory
$(shell mkdir ./$(OUTPUT) 			2>/dev/null)
$(shell mkdir ./$(OUTPUT)/src 		2>/dev/null)
$(shell mkdir ./$(OUTPUT)/src/os 	2>/dev/null)
$(shell mkdir ./$(OUTPUT)/src/avr 	2>/dev/null)
$(shell mkdir ./$(OUTPUT)/app 		2>/dev/null)

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion build \
	elf hex eep lss sym coff extcoff clean clean_list

include $(TOPDIR)/rules.mk
