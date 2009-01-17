/*
 * Module Skeleton Driver
 *
 * ==========================================================================
 *
 *  Copyright 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _FRAM_H
#define _FRAM_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

#define FRAM_Mode_Read 0x01
#define FRAM_Mode_Write 0x00

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern unsigned char FRAM_Begin(unsigned char device_addr, unsigned int memory_addr, unsigned char mode) __wparam;
extern void FRAM_ReadBuf(unsigned char buffer_size, unsigned char * buffer) __wparam;
extern unsigned char FRAM_WriteBuf(unsigned char buffer_size, unsigned char * buffer) __wparam;
extern unsigned char FRAM_ReadByte(void) __wparam;
extern unsigned char FRAM_WriteByte(unsigned char byte) __wparam;
extern void FRAM_End(void) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

//FRAM_REG is set by FRAM_Begin:
//<7:7>first byte read flag, zero before first byte is read
//<6:6>Session Flag, set to 1 in FRAM_Begin -> FRAM_REG != 0 during a session.
//<5:4>multiplex addr, 5 MSB, 4 LSB
//<3:1>chip select (for FM24C512 LSB selects one of two memory blocks)
//<0:0>mode, 1 for read, 0 for write
//FRAM_End sets FRAM_REG to zero. You can use it to check if there's a 
//pending session, in this case you have to init FRAM_REG by yourself.
//Never change it during a session (FRAM_Begin -> FRAM_End)!!
extern unsigned char FRAM_REG;


#endif /* _FRAM_H */
