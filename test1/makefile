
TARGET = test1
MCU = atmega88
CLK = 20000000
OBJ = test1.o aes.o

INCLUDE = -I.. -I../../simavr/simavr/sim/avr
CFLAGS  = -fdata-sections -ffunction-sections -Wl,--gc-sections -Wall -g0 -Os -Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000

all: $(TARGET).elf

clean:
	rm -f *.hex *.elf *.o

aes.o: ../aes.c
	avr-gcc -c -mmcu=$(MCU) -DF_CPU=$(CLK) $(CFLAGS) $(INCLUDE) -o $@ $<

.c.o:
	avr-gcc -c -mmcu=$(MCU) -DF_CPU=$(CLK) $(CFLAGS) $(INCLUDE) -o $@ $<


$(TARGET).elf: $(OBJ)
	avr-gcc -mmcu=$(MCU) -DF_CPU=$(CLK) $(CFLAGS) $(INCLUDE) -Wl,-Map,$(TARGET).map -o $@ $(OBJ)

