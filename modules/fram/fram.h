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

// high level functions

extern unsigned char FRAM_ReadByte(unsigned char device_addr,unsigned int address) __wparam;
extern unsigned char FRAM_WriteByte(unsigned char device_addr,unsigned int address,unsigned char value) __wparam;
extern unsigned char FRAM_ReadBuf(unsigned char device_addr,unsigned int address,unsigned char byte_count, unsigned char * buffer) __wparam;
extern unsigned char FRAM_WriteBuf(unsigned char device_addr,unsigned int address,unsigned char byte_count, unsigned char * buffer) __wparam;

// low level functions

extern unsigned char FRAM_Begin(unsigned char device_addr,unsigned int address,unsigned char mode) __wparam;
extern void FRAM_End(void) __wparam;

extern unsigned char FRAM_ReadByte_Cont(void) __wparam;
extern unsigned char FRAM_WriteByte_Cont(unsigned char byte) __wparam;
extern void FRAM_ReadBuf_Cont(unsigned char byte_count, unsigned char * buffer) __wparam;
extern unsigned char FRAM_WriteBuf_Cont(unsigned char byte_count, unsigned char * buffer) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern unsigned char FRAM_REG;
extern unsigned char FRAM_ERROR;


#endif /* _FRAM_H */
