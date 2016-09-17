TARGET  = 7G_ctrl
DEVICE  = atmega169p
OBJPATH = objs/
#AVRDUDE = avrdude -c dragon_isp -P usb -p $(DEVICE)
AVRDUDE = avrdude -c stk500v2 -P avrdoper -p $(DEVICE)

# we calibrate the RC oscillator to this frequency with the 32KHz crystal
F_CPU   = 921600

# fuses for the ATmega169PV
FUSEH   = 0x99
FUSEL   = 0x62
FUSEX   = 0xff

CFLAGS	= -I. -I../common -Wall -Os -flto
#CFLAGS += -DDBGPRINT

LFLAGS  = -Wl,--relax -flto
#LFLAGS += -u vfprintf -lprintf_min

# search for source files in this directory
VPATH   = ../common

COMPILE = avr-gcc -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) $(CFLAGS)

OBJECTS = $(addprefix $(OBJPATH), 7g_ctrl.o matrix.o led.o rf_ctrl.o sleeping.o \
			ctrl_settings.o proc_menu.o calibrate_rc.o avrdbg.o rf_addr.o nRF24L.o)

hex: $(TARGET).hex

$(TARGET).hex: $(OBJECTS)
	$(COMPILE) -o $(TARGET).elf $(OBJECTS) $(LFLAGS)
	rm -f $(TARGET).hex $(TARGET).eep.hex
	avr-objcopy -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex
	avr-size -C --mcu=$(DEVICE) $(TARGET).elf

flash: all $(TARGET).hex
	$(AVRDUDE) -U flash:w:$(TARGET).hex:i

clean:
	rm -f $(TARGET).hex $(TARGET).elf $(OBJECTS)

fuse:
	$(AVRDUDE) -U hfuse:w:$(FUSEH):m -U lfuse:w:$(FUSEL):m -U efuse:w:$(FUSEX):m

read_fuse:
	$(AVRDUDE) -U hfuse:r:-:h -U lfuse:r:-:h -U efuse:r:-:h

# $(COMPILE) -S -fverbose-asm $< -o $@

$(OBJPATH)%.o: %.c makefile
	$(COMPILE) -c $< -o $@

all: clean hex
