# $Id$

# define the processor, linker file and project name
PROCESSOR = 18f452
LKR_FILE  = $(MIOS_PATH)/etc/lkr/p$(PROCESSOR).lkr
PROJECT   = libsdcc

# list of objects that should be created and linked
C_SRC   = $(wildcard */*.c)
S_SRC   = $(wildcard */*.S)
OBJS    = $(C_SRC:.c=.o) $(S_SRC:.S=.o)

# include pathes (more will be added by .mk files)
GPASM_INCLUDE = 
SDCC_INCLUDE  = 


# override default rule:
all: $(PROJECT).lib


# include the common.mk file
include $(MIOS_PATH)/include/makefile/common.mk


# local default rule to create a library
$(PROJECT).lib: cleanall mk_outdir mk_outsubdirs $(addprefix $(OUTDIR)/, $(OBJS))
	rm -f */*__mios-gpasm-tmp.S  #  DIRTY!!! Makefile fails if temporary .S files are still located in original source directories. If the assembly of one .S file fails, the appr. gpasm file has to be deleted manually :-(
	$(GPLIB) $(PROJECT).lib $(addprefix $(OUTDIR)/, $(OBJS))


# creating subdirectories inside _output (not automated yet)
mk_outsubdirs:
	mkdir -p $(OUTDIR)/char
	mkdir -p $(OUTDIR)/fixed16x16
	mkdir -p $(OUTDIR)/float
	mkdir -p $(OUTDIR)/gptr
	mkdir -p $(OUTDIR)/int
	mkdir -p $(OUTDIR)/long
	mkdir -p $(OUTDIR)/lregs
	mkdir -p $(OUTDIR)/stack

