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
CFLAGS=-I $(INC_DIR) -Wall -Os -std=c99

# [target]:[prerequisites]
#	[recipe]
# -c 		Generate object file (compile file without linking)
# -o <dest>	Put compilation output to <dest>
# $@		Target file
# $< 		First prerequisite - here %.c

#source files list
MCU1_SRC_LIST = $(SRC_DIR)/mcu1.c \
		   		$(SRC_DIR)/serial_tx.c \
		   		$(SRC_DIR)/serial_rx.c \
		   		$(SRC_DIR)/serial_progmem.c \
		   		$(SRC_DIR)/ICCM.c \
		   		$(SRC_DIR)/distance_sensor.c \
		   		$(SRC_DIR)/line_sensor.c \
		   		$(SRC_DIR)/ADC.c \
		   		$(SRC_DIR)/AI.c \

MCU2_SRC_LIST = $(SRC_DIR)/mcu2.c \
		   		$(SRC_DIR)/serial_tx.c \
		   		$(SRC_DIR)/serial_rx.c \
		   		$(SRC_DIR)/serial_progmem.c \
		   		$(SRC_DIR)/ICCM.c \
		   		$(SRC_DIR)/drive_ctrl.c \

MCU1_DEFINES = 	-D MCU1 \
				-D AI_DEBUG \
				# -D SERIAL_RX_DEBUG \
				# -D SERIAL_TX_DEBUG \
				# -D ICCM_DEBUG \

MCU2_DEFINES = 	-D MCU2 \
				-D SERIAL_RX_DEBUG \
				-D SERIAL_TX_DEBUG \
				-D ICCM_DEBUG \

#make all rule
all: minisumo2_mcu1.elf minisumo2_mcu1.hex minisumo2_mcu2.elf minisumo2_mcu2.hex
	@echo '				******                             ******'
	@echo '				****  Finished building all targets  ****'
	@echo '				******                             ******'

#compile project mcu1 into .elf file
minisumo2_mcu1.elf: 
	@echo ' ********************************************************************************************************* '
	@echo 'Building target: $@.. '
	$(CC) $(MCU1_SRC_LIST) $(CFLAGS) $(MCU1_DEFINES) -mmcu=$(MMCU) -o $(OUT_DIR)/$@
	@echo 'Finished building target: $@'
	@echo ' ********************************************************************************************************* '
	@echo ' '

#compile project mcu2 into .elf file
minisumo2_mcu2.elf: 
	@echo ' ********************************************************************************************************* '
	@echo 'Building target: $@.. '
	$(CC) $(MCU2_SRC_LIST) $(CFLAGS) $(MCU2_DEFINES) -mmcu=$(MMCU) -o $(OUT_DIR)/$@
	@echo 'Finished building target: $@'
	@echo ' ********************************************************************************************************* '
	@echo ' '

#convert .elf file into flashable .hex
minisumo2_mcu1.hex: minisumo2_mcu1.elf
	@echo ' ********************************************************************************************************* '
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex $(OUT_DIR)/$< $(OUT_DIR)/$@
	@echo 'Finished building: $@'
	@echo ' ********************************************************************************************************* '
	@echo ' '

#convert .elf file into flashable .hex
minisumo2_mcu2.hex: minisumo2_mcu2.elf
	@echo ' ********************************************************************************************************* '
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex $(OUT_DIR)/$< $(OUT_DIR)/$@
	@echo 'Finished building: $@'
	@echo ' ********************************************************************************************************* '
	@echo ' '

#make clean
clean:
	@rm -f $(OUT_DIR)/minisumo2_mcu1.elf
	@rm -f $(OUT_DIR)/minisumo2_mcu1.hex
	@rm -f $(OUT_DIR)/minisumo2_mcu2.elf
	@rm -f $(OUT_DIR)/minisumo2_mcu2.hex
	@echo '### Clean finished! ###'
	@echo ' '

#memory analysis
mem: minisumo2_mcu1.elf minisumo2_mcu2.elf
	@echo ' ********************************************************************************************************* '
	avr-size --format=berkeley out/minisumo2_mcu1.elf
	@echo ' ********************************************************************************************************* '
	avr-size --format=berkeley out/minisumo2_mcu2.elf


