FRAM Module
============

This module enables you to drive the Ramtron Ferroelectrical RAM modules 
(FM24C512,FM24C256, FM24C64). One big advantages of the ferroelectrical 
technology is the write speed: there's no need to wait for the device to
finish a write cycle like with EEProms or Flash devices, the write operation
is finished with the next clock cycle, writes perform at the same speed as reads.
The other big plus is the endurance of 10 billion (10^10) read or write cycles.
In 7x24 heavy usage this would equal ca. 30years endurance. Data retention is
45 years.

The devices use the IIC protocol for communication and allow clock speeds up
to 1MHz. There are two or three device address pins (two for FM24C512):
The FM24C512 is treated like two FM24C256, it's memory space is splitted into 
two sectors. Therefore the 512 has just two device address pins, while 
FM24C256/FM24C64 have three.

So the chip select address is 3 bits for all device types, the address range
is 0x0000 - 0x7FFF for FM24C512 and FM24C256, 0x0000 - 0x1FFF for the FM24C64.
The smaller types are not compatible to this driver.

There is a multiplexing option (disabled by default), which uses two additional
pins (default J10 RD5/RD6) to multiplex up to 32 devices (e.g. 16 x F24C512).
In this mode, bits 3 and 4 of the device select address are used for the
multiplexer pins.

The default port used to drive the devices is J10, which allows the driver
to use its own dedicated IIC protocol with 1MHZ clock speed. If you choose to
use the standard MIOS IIC port J4 instead, the driver will use the mios_iic
functions which drive the port @400KHz to ensure compatibility to other IIC 
devices. By manipulating the pin assignments, you even can drive the FRAM over
J4 in fast mode, but this is not recommended and could lead to problems with
other IIC devices sharing the same port. 

At the end of this file you find some information about read/write speed.

The IIC slave-address(0xb1010) is the same as for the banksticks (24LC256/512), so they
also share the device select address range! You can drive both FRAM and Banksticks
over the MIOS IIC port, but then you have to share the device address space
between them. Note that the banksticks can not be driven by the fram-moudule
functions, you have to use the MIOS_Bankstick_XXX functions.
However, it's recomended to drive the FRAM's over the standard port J10, which
allows the FRAM to be driven at maximum speed of 1MHz.

Files
~~~~~
fram.inc: can be included into "asm-only" programs (non-relocatable code)
fram.asm: relocatable wrapper for C based applications
fram.h:	C headers
fram.mk:  include this into Makefile to integrate the driver
fram_iic.inc: 1MHZ IIC - driver. will be included by fram.inc automatically if needed

Functions
------------
These function descriptions are primarly dedicated to C programmers. ASM 
programmers please note that the device_addr parameter is different in the
C version (in ASM the r/w mode and device_addr are packed, r/w mode is bit 0),
please read the comments in fram.inc for further information how to pass the
paramters in ASM.


There are two types of functions:

High level functions 
----
High level functions wrap a whole Begin->R/W->End cycle. This is the 
safe option if you use shared pin's (e.g. RD5/RD6). Also this is the
easyest way to read or write data if you know the amount of data you
have to transfer in advance.
Take care that you never cross the upper address space limits. Neither these
functions will switch to the next device address automatically, nor the address
will wrap to zero automatically on the device!

//-- Reads one byte from address@device_addr and returns it.
//-- Error code in FRAM_ERROR. See "Error Codes" section.
unsigned char FRAM_ReadByte(unsigned char device_addr,unsigned int address);

//-- Writes one byte to address@device_addr. Returns 0x00 on error.
//-- Error code in FRAM_ERROR. See "Error Codes" section.
extern unsigned char FRAM_WriteByte(unsigned char device_addr,unsigned int address,unsigned char value);

//-- Reads byte_count bytes from address@device_addr to buffer. Returns 0x00 on error.
//-- Error code in FRAM_ERROR. See "Error Codes" section.
extern unsigned char FRAM_ReadBuf(unsigned char device_addr,unsigned int address,unsigned char byte_count, unsigned char * buffer);

//-- Writes byte_count bytes from buffer to address@device_addr. Returns 0x00 on error.
//-- Error code in FRAM_ERROR. See "Error codes" section.
extern unsigned char FRAM_WriteBuf(unsigned char device_addr,unsigned int address,unsigned char byte_count, unsigned char * buffer);



Low level functions
----
Before you can read or write bytes, you have to call FRAM_Begin to start a read
or write session. After that, you can call byte or buffer r/w functions before 
you end the session with FRAM_End (subsequent/continous r/w). 

Use this function set if you don't know the exact amount of data at session 
begin, e.g. the amount of data depends on what data you read (you have a file 
with size data at the beginning etc.), or if a ISR sets a flag during a session. 
You have to take care yourself, that you don't call any function that manipulates 
the FRAM pins during a session:

* RD5 / RD6 are shared with MIOS_LCD_XXX functions (default multiplex pins).
* RD5 is also shared with MIOS_IIC (MIOS_Bankstick_XXX, MIOS_IIC_XXX)

See "PIN's / Defaults" section to get information about default PIN assignments.

Note that with FRAM_Begin, you select r/w mode. Don't try to call 
FRAM_ReadXXX_Cont in write mode and vice versa! Also never call any high-level 
functions during a session, this would end your session immediately.
Byte and buffer r/w functins can be mixed, they are about the same speed. The
device- and memory-address are transfered to the device with FRAM_Begin, after
that, each r/w call increases the address latch on the device automatically. Be
aware that if you cross the upper address space limit, neither the address will
wrap to zero, nor the next device will be selected! You have to track yourself,
at which address you are reading/writing currently.

//-- Begins a read or write session. Selects the device and sets start memory-address.
//-- Returns 0x00 on error, error code in FRAM_ERROR. See "Error Codes" section.
unsigned char FRAM_Begin(unsigned char device_addr,unsigned int address,unsigned char mode);

//-- Ends a read or write session and releases the IIC port. After this call,
//-- it's save again to use the shared pins by other function calls.
extern void FRAM_End(void);

//-- Reads one byte from the current address and returns it. Current address
//-- latch increments automatically on the device.
unsigned char FRAM_ReadByte_Cont(void);

//-- Writes one byte to the current address. Current address latch increments 
//-- automatically on the device.
//-- Returns 0x00 on error, error code in FRAM_ERROR. See "Error Codes" section.
unsigned char FRAM_WriteByte_Cont(unsigned char byte);

//-- Reads byte_count bytes from the current address to buffer. Current address
//-- latch increments automatically on the device.
void FRAM_ReadBuf_Cont(unsigned char byte_count, unsigned char * buffer);

//-- Writes byte_count bytes to the current address. Current address latch increments 
//-- automatically on the device.
//-- Returns 0x00 on error, error code in FRAM_ERROR. See "Error Codes" section.
unsigned char FRAM_WriteBuf_Cont(unsigned char byte_count, unsigned char * buffer);

Parameter description (both high-/low level functions):
----
device_addr:
	0x00 is the first device. If you don't use multiplexing, you can access
	maximal 8 devices. Note that a FM24C512 uses two device addresses. With
	multiplexing, 32 devices can be accessed.
	If you use the MIOS_IIC port, and you share it with banksticks, be aware
	that they use the same slave-address, so you have also to share the device
	addresses with them!
address:
	memory address from 0x0000 to 0x7FFF (FM24C512 / FM24C256) or 0x1FFF (FM24C64)
value:
	the byte value you want to write
byte_count:
	number of bytes to read from the device / write to the device
buffer:
	pointer to the first byte of the buffer to read from / write to
mode:
	0x00 for write mode, 0x01 for read mode. You can use the constants
	FRAM_Mode_Read / FRAM_Mode_Write instead (defined in fram.h)


Error-codes
----
After a function call, there will be an error-status available in FRAM_ERROR.
Note that all functions that can return with an error also return 0x00 on error,
except FRAM_ReadByte, whose return value is the byte. With FRAM_ReadByte, you
have to check FRAM_ERROR after the call to check for success.

0x00: no error
0x01: error on device address send (FRAM_Begin and high-level functions)
0x02: error on address MSB send (FRAM_Begin and high-level functions)
0x04: error on address LSB send (FRAM_Begin and high-level functions)
0x08: error on device address send (FRAM_Begin and high-level functions in read mode)
0x10: error on byte write (FRAM_WriteXXX[_XXX] functions)


PIN's / Defaults
-----------------

If you don't override any defines, the following defaults will take place:

* IIC SDA : RC5 (J10)
* IIC SCL : RC4 (J10)
* Multiplexing disabled (multiplex pin assignments ignored)
* MIOS-IIC disabled


If you enable multiplexing, all the pins needed will be assigned to J10:

* IIC SDA : RC5 (J10)
* IIC SCL : RC4 (J10)
* Multiplex LSB: RD6 (J10)
* Multiplex MSB: RD5 (J10)


If you enable MIOS-IIC, the IIC communication will use the mios-iic pins,
multiplex pins will switch to RC4/RC5. Note that in this case, overriding pin
assignment for SDA/SCL will have no effect:

* IIC SDA : RA4 (J4, default MIOS-IIC-Port)
* IIC SCL : RD5 (J4, default MIOS-IIC-Port)
* Multiplex LSB (only if multiplexing enabled): RC5 (J10)
* Multiplex MSB (only if multiplexing enabled): RC4 (J10)


Defines
--------
These defines can be overruled in the Makefile of your application. The values
shown here are the default values that will be used if you do not override them:

#Set this to 1 to enable multiplexing.
FRAM_DEFINES += -DFRAM_MULTIPLEX_ENABLE=0

#Set this to 1 to use the mios_iic functions (400KHz).
#If set to 0, the fram_iic functions @1MHz will be used,
#and port J10 will be the default for both IIC and multiplexing.
FRAM_DEFINES += -DFRAM_MIOS_IIC=0

#Increase this value if the communication gets unstable 
#(very long cable etc.). This will insert nop's at every
#time critical position in the fram_mios_iic protocoll driver.
#This option only has an effect if FRAM_MIOS_IIC = 0
FRAM_DEFINES += -DFRAM_IIC_SLOWDOWN=0

#SCL pin assignment
FRAM_DEFINES += -DFRAM_IIC_PORT_SCL=PORTC
FRAM_DEFINES += -DFRAM_IIC_TRIS_SCL=TRISC
FRAM_DEFINES += -DFRAM_IIC_PIN_SCL=4

#SDA pin assignment
FRAM_DEFINES += -DFRAM_IIC_PORT_SDA=PORTC
FRAM_DEFINES += -DFRAM_IIC_TRIS_SDA=TRISC
FRAM_DEFINES += -DFRAM_IIC_PIN_SDA=5

#Multiplexing MSB pin assignment
FRAM_DEFINES += -DFRAM_MULTIPLEX_PORT_MSB=PORTD
FRAM_DEFINES += -DFRAM_MULTIPLEX_TRIS_MSB=TRISD
FRAM_DEFINES += -DFRAM_MULTIPLEX_PIN_MSB=5

#Multiplexing LSB pin assignment
FRAM_DEFINES += -DFRAM_MULTIPLEX_PORT_LSB=PORTD
FRAM_DEFINES += -DFRAM_MULTIPLEX_TRIS_LSB=TRISD
FRAM_DEFINES += -DFRAM_MULTIPLEX_PIN_LSB=6


C based Applications
~~~~~~~~~~~~~~~~~~~~

1) Makefile: add "fram.o" to OBJS variable:
	
	OBJS = mios_wrapper.o app_lcd.o fram.o main.o

2) Makefile: include fram.mk 
	
	include $(MIOS_PATH)/modules/fram/fram.mk

3) Makefile: enhance FRAM_DEFINES if required (see section "Defines")

4) main.c: #include <fram.h>


"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 
1) main.asm (or main.inc) include the module source: 

	 #include <fram.inc>

2) Makefile: include fram.mk 
	
	include $(MIOS_PATH)/modules/fram/fram.mk

3) Makefile: enhance FRAM_DEFINES if required (see section "Defines")

4) app_defines.inc: add the variables you can find declared in fram.asm
   Possible Range: 0x10..0x7f
   Make sure the EQU address is unique in your app!

	;; for fram module
	FRAM_IIC_BUF	EQU	0x067 ; only needed if FRAM_MIOS_IIC = 0
	FRAM_REG			EQU 	0x068
	FRAM_ERROR		EQU	0x069



About Read / Write speed
----------------------------
In contrast to EEPROM / Flash based devices, there's no significant difference
between read and write speed. The following benchmark values are measured with
sdcc_fram_test (in apps/troubleshooting). For the speed check, subsequent reads
/ writes are used (FRAM_Begin -> R/W 8KBytes -> FRAM_End) @ 1MHz:

* Speed Test write (8KB subsequent): 11.4 mS/KB = 87.7 KB/S = 89.8 B/mS
* Speed Test read (8KB subsequent) : 10.8 mS/KB = 92.5 KB/S = 94.8 B/mS

Reading/Writing single bytes with a high-level function is ca. 6 times slower
than reading buffers of 256Bytes. 
When you use low-level functions, reading single bytes of buffers make no 
signigicant difference in speed.

You can use the following formula to aproximatly calculate the time needed
for a read or write session (one high-level function call or a Begin->R/W->End
Session cycle):

t(uS) = 50 + 11*byte_count

Simply multiply this value with 2.5 to get an aproximated value for the 
slower FRAM_MIOS_IIC=1 mode.



Instruction Cycles / Function (FRAM_MIOS_IIC = 0)
---------------------------------------------------
FRAM_Begin: 468
FRAM_End: 29
FRAM_ReadByteCont : 107 
FRAM_WriteByteCont : 110
FRAM_ReadBufCont : 10 + bytecount*105
FRAM_WriteBufCont : 5 + bytecount*110

