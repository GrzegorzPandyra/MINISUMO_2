#
# Makefile created based on auto-generated makefile from arduino IDE
#

#folders
INC_DIR=include
OUT_DIR=out
OBJ_DIR=$(OUT_DIR)/obj
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
SRC_LIST = $(SRC_DIR)/main.c \
		   $(SRC_DIR)/serial_interface.c \

#make all rule
all: minisumo_2.elf minisumo_2.hex
	@echo '******                             ******'
	@echo '****  Finished building all targets  ****'
	@echo '******                             ******'

#compile project into .elf file
minisumo_2.elf: 
	@echo 'Building target: $@.. '
	@echo ' '
	@echo ' ********************************************************************************************************* '
	$(CC) $(SRC_LIST) $(CFLAGS) -mmcu=$(MMCU) -o $(OUT_DIR)/$@
	@echo ' ********************************************************************************************************* '
	@echo ' '
	@echo 'Finished building target: $@'
	@echo ' '

#convert .elf file into flashable .hex
minisumo_2.hex: minisumo_2.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex $(OUT_DIR)/$< $(OUT_DIR)/$@
	@echo 'Finished building: $@'
	@echo ' '

#make clean
clean:
	@rm -f $(OBJ_DIR)/*.o minisumo_2
	@rm -f $(OUT_DIR)/minisumo_2.elf
	@rm -f $(OUT_DIR)/minisumo_2.hex
	@echo '### Clean finished! ###'
	@echo ' '


