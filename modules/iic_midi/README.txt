$Id$

Driver for MBHP_IIC_MIDI module
-> http://www.ucapps.de/mbhp_iic_midi.html


C usage example -> $MIOS_PATH/apps/processing/midi_router
ASM usage example -> $MIOS_PATH/apps/sequencers/midibox_seq_v3


Files
~~~~~

  iic_midi.inc: can be included into "asm-only" programs (non-relocatable code)
  iic_midi.asm: relocatable wrapper for C based applications
  iic_midi.h:   C headers
  iic_midi.mk:  include this into Makefile to integrate the module


Defines
~~~~~~~

  used in iic_midi.inc
    C programs (relocatable code): can be overruled with -D<define>=<value> from command line 
    Assembly programs: has to be defined within program before "#include <iic_midi.inc>"
  
;; base address and max number of IIC slaves
#define IIC_MIDI_BASE_ADDR	0x10

;; maximum supported number of MBHP_IIC_MIDI modules (1..4)
#define IIC_MIDI_MAX_NUMBER	4

;; use Receive Interrupt lines for faster polling?
#define IIC_MIDI_USE_RI		1

;; don't use J5 of the core module as digital inputs or RI signals
#define IIC_MIDI_DONT_USE_J5_INPUTS 0

;; RI_N lines (note: ADC will be turned off in IIC_MIDI_Init if IIC_MIDI_DONT_USE_J5_INPUTS == 0)
#define IIC_MIDI_PORT_RI_N_SLAVE0	PORTA
#define IIC_MIDI_TRIS_RI_N_SLAVE0	TRISA
#define IIC_MIDI_PIN_RI_N_SLAVE0	0
#define IIC_MIDI_PORT_RI_N_SLAVE1	PORTA
#define IIC_MIDI_TRIS_RI_N_SLAVE1	TRISA
#define IIC_MIDI_PIN_RI_N_SLAVE1	1
#define IIC_MIDI_PORT_RI_N_SLAVE2	PORTA
#define IIC_MIDI_TRIS_RI_N_SLAVE2	TRISA
#define IIC_MIDI_PIN_RI_N_SLAVE2	2
#define IIC_MIDI_PORT_RI_N_SLAVE3	PORTA
#define IIC_MIDI_TRIS_RI_N_SLAVE3	TRISA
#define IIC_MIDI_PIN_RI_N_SLAVE3	3


Integration Hints
~~~~~~~~~~~~~~~~~

C based Applications
~~~~~~~~~~~~~~~~~~~~

  1) Makefile: add "iic_midi.o" to OBJS variable

  2) Makefile: include iic_midi.mk, and enhance IIC_MIDI_DEFINES if required:
---
# include MBHP_IIC_MIDI driver
IIC_MIDI_DEFINES += -DIIC_MIDI_USE_RI=0 -DIIC_MIDI_DONT_USE_J5_INPUTS=1
include $(MIOS_PATH)/modules/iic_midi/iic_midi.mk
---

  3) main.c: #include <iic_midi.h>

  4) main.c: add IIC_MIDI_Init() to Init() hook



"Assembler-Only" Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  1a) add application specific #defines to the assembly code (e.g. in
      your setup_*.asm file), e.g.:
      #define IIC_MIDI_USE_RI              0
      #define IIC_MIDI_DONT_USE_J5_INPUTS  1

  1b) main.asm (or main.inc): #include <iic_midi.inc>

  2) Makefile: include iic_midi.mk

  3) main.asm (or main.inc): call IIC_MIDI_Init from USER_Init hook

  4) app_defines.inc: add following variables, they must be located
     in access bank!
     Possible Range: 0x10..0x7f
---
;; for iic_midi module
_iic_midi_available_slaves EQU	0x070
_iic_midi_slave		EQU	0x071
IIC_MIDI_TMP		EQU	0x072
IIC_MIDI_TX_RETRY_CTR	EQU	0x073
IIC_MIDI_BYTES_TO_SEND	EQU	0x074
_iic_midi_rx_package	EQU	0x075; ..0x78
_iic_midi_tx_package	EQU	0x079; ..0x7c
---


TODO: allow banked RAM areas as well


