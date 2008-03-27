# $Id: $

# default flags to GPASM here
GPASM_FLAGS   = 


# default rule that defines the .hex files which should be created
# note: always require a "cleanall", since dependencies (e.g. on .h and .inc files) are not properly declared
all: cleanall midimerger_pic18f_with_bootloader.hex midimerger_pic18f_without_bootloader.hex midimerger_pic16f87x.hex midimerger_pic16f88.hex

midimerger_pic18f_with_bootloader.hex: 18f/main.asm
	gpasm $< $(GPASM_FLAGS) -DMIOS_BSL_FLAG=1 -I ./18f -p 18f452 -o midimerger_pic18f_with_bootloader.hex

midimerger_pic18f_without_bootloader.hex: 18f/main.asm
	gpasm $< $(GPASM_FLAGS) -DMIOS_BSL_FLAG=0 -I ./18f -p 18f452 -o midimerger_pic18f_without_bootloader.hex

midimerger_pic16f87x.hex: 16f/main.asm
	gpasm $< $(GPASM_FLAGS) -I ./16f -p 16f874 -o midimerger_pic16f87x.hex

midimerger_pic16f88.hex: 16f/main.asm
	gpasm $< $(GPASM_FLAGS) -I ./16f -p 16f88 -o midimerger_pic16f88.hex


clean:
	rm -f *.cod *.lst *.err

cleanall: clean
	rm -f *.hex
