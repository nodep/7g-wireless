TARGET   = 7G_dngl_nrf.hex
OBJPATH  = objs/
CFLAGS   = --model-small -I../common -DNRF24LU1
LFLAGS   = --code-loc 0x0000 --code-size 0x4000 --xram-loc 0x8000 --xram-size 0x800
ASFLAGS  = -plosgff
RELFILES = $(addprefix $(OBJPATH), main.rel usb_desc.rel nRFutils.rel text_message.rel rf_dngl.rel \
			usb.rel reports.rel rf_addr.rel nrfdbg.rel nRF24L.rel crtxinit.rel)

VPATH    = ../common

$(TARGET): $(RELFILES)
	sdcc $(CFLAGS) $(LFLAGS) $(RELFILES) -o $(TARGET)

$(OBJPATH)%.rel: %.c makefile
	sdcc $(CFLAGS) -c $< -o $(OBJPATH)

$(OBJPATH)%.rel: %.asm
	sdas8051 $(ASFLAGS) $@ $<

clean:
	rm --force *.hex *.mem *.lk *.map $(addprefix $(OBJPATH), *.lst *.rel *.rst *.sym *.asm)

all: clean $(TARGET)

flash: clean $(TARGET)
	nrfburn -f 16 -w $(TARGET)
