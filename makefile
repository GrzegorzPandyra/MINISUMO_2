#
# Makefile created based on auto-generated makefile from arduino IDE
#

#folders
INC_DIR=include
OUT_DIR=out
LIB_DIR=lib
SRC_DIR=src

#compiler
CC=avr-gcc
MMCU=atmega8
CFLAGS=-I $(INC_DIR) -Wall -Os

# [target]:[prerequisites]
#	[recipe]
# -c 		Generate object file (compile file without linking)
# -o <dest>	Put compilation output to <dest>
# $@		Target file
# $< 		First prerequisite - here %.c

#dependency list
DEPS = config.h

#source files list
MCU1_SRC_LIST = $(SRC_DIR)/mcu1.c \
		   		$(SRC_DIR)/serial_interface.c \

MCU2_SRC_LIST = $(SRC_DIR)/mcu2.c \
		   		$(SRC_DIR)/serial_interface.c \

#make all rule
all: minisumo2_mcu1.elf minisumo2_mcu1.hex minisumo2_mcu2.elf minisumo2_mcu2.hex
	@echo '******                             ******'
	@echo '****  Finished building all targets  ****'
	@echo '******                             ******'

#compile project mcu1 into .elf file
minisumo2_mcu1.elf: 
	@echo 'Building target: $@.. '
	@echo ' '
	@echo ' ********************************************************************************************************* '
	$(CC) $(MCU1_SRC_LIST) $(CFLAGS) -mmcu=$(MMCU) -o $(OUT_DIR)/$@
	@echo ' ********************************************************************************************************* '
	@echo ' '
	@echo 'Finished building target: $@'
	@echo ' '

#compile project mcu2 into .elf file
minisumo2_mcu2.elf: 
	@echo 'Building target: $@.. '
	@echo ' '
	@echo ' ********************************************************************************************************* '
	$(CC) $(MCU2_SRC_LIST) $(CFLAGS) -mmcu=$(MMCU) -o $(OUT_DIR)/$@
	@echo ' ********************************************************************************************************* '
	@echo ' '
	@echo 'Finished building target: $@'
	@echo ' '

#convert .elf file into flashable .hex
minisumo2_mcu1.hex: minisumo2_mcu1.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex $(OUT_DIR)/$< $(OUT_DIR)/$@
	@echo 'Finished building: $@'
	@echo ' '

#convert .elf file into flashable .hex
minisumo2_mcu2.hex: minisumo2_mcu2.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex $(OUT_DIR)/$< $(OUT_DIR)/$@
	@echo 'Finished building: $@'
	@echo ' '

#make clean
clean:
	@rm -f $(OUT_DIR)/minisumo2_mcu1.elf
	@rm -f $(OUT_DIR)/minisumo2_mcu1.hex
	@rm -f $(OUT_DIR)/minisumo2_mcu2.elf
	@rm -f $(OUT_DIR)/minisumo2_mcu2.hex
	@echo '### Clean finished! ###'
	@echo ' '


