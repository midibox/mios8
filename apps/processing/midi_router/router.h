// $Id$
/*
 * Header file for router
 *
 * ==========================================================================
 *
 *  Copyright 2006 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _ROUTER_H
#define _ROUTER_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

// assembly/C compatible defines
#include "router_ports.h"

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// status of application
typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned IIC2_FWD_CHN8_16:1;
    unsigned IIC3_FWD_FORCE_CHN:1;
    unsigned IIC3_FWD_MBSID:1;
    unsigned IIC3_FWD_MBFM:1;
    unsigned IIC3_FWD_MBSEQ:1;
  };
} router_flags_t;


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////
extern void ROUTER_Init(void) __wparam;

extern void ROUTER_Tx_INT0(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;
extern void ROUTER_Tx_IIC0(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;
extern void ROUTER_Tx_IIC1(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;
extern void ROUTER_Tx_IIC2(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;
extern void ROUTER_Tx_IIC3(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;

extern void ROUTER_Rx_INT0(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;
extern void ROUTER_Rx_IIC0(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;
extern void ROUTER_Rx_IIC1(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;
extern void ROUTER_Rx_IIC2(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;
extern void ROUTER_Rx_IIC3(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern unsigned char sysex_owner;

// customized options
extern router_flags_t router_flags;
extern unsigned char router_iic3_fwd_force_channel;
extern unsigned char router_iic3_fwd_transpose;

#endif /* _ROUTER_H */
