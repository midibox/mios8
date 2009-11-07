GPASMFLAGS = -p 18f452

all: cleanall main.hex

%.hex: %.asm
	gpasm $(GPASMFLAGS) $<

clean:
	rm -f *.cod *.lst *.err

cleanall: clean
	rm -f *.hex
