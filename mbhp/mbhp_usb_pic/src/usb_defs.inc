; $Id$
;
;  Definitions for USB Driver
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


	;; USB stat struct
USB_STAT_RAM_PTR	EQU	0	; send data from RAM/ROM
USB_STAT_REMOTE_WAKEUP	EQU	1	;

	;; EP struct
BDn_SIZE	EQU	4	; size of one record
BDn_STAT	EQU	0	; offset to status register
BDn_CNT		EQU	1	; offset to counter register
BDn_ADDRL	EQU	2	; offset to address pointer (low byte)
BDn_ADDRH	EQU	3	; offset to address pointer (high byte)

	;; flags within STAT register
BDn_STAT_BC8		EQU	0
BDn_STAT_BC9		EQU	1
BDn_STAT_BSTALL		EQU	2
BDn_STAT_DTSEN		EQU	3
BDn_STAT_INCDIS		EQU	4
BDn_STAT_KEN		EQU	5
BDn_STAT_DTS		EQU	6
BDn_STAT_UOWN		EQU	7

	;; SFRs are located here:
EP0BO	EQU	0x0400
EP0BI	EQU	0x0404
EP1BO	EQU	0x0408
EP1BI	EQU	0x040c
EP2BO	EQU	0x0410
EP2BI	EQU	0x0414
	;; ...
EP15BO	EQU	0x0478
EP15BI	EQU	0x047c

	;; structure of setup buffer
EP0_SETUP_REQUEST_TYPE	EQU	0
EP0_SETUP_REQUEST	EQU	1
EP0_SETUP_VALUE_L	EQU	2
EP0_SETUP_VALUE_H	EQU	3
EP0_SETUP_INDEX_L	EQU	4
EP0_SETUP_INDEX_H	EQU	5
EP0_SETUP_LENGTH_L	EQU	6
EP0_SETUP_LENGTH_H	EQU	7

EP0_SETUP_REQUEST_TYPE_RECIPIENT EQU	0	; 5 bits
EP0_SETUP_REQUEST_TYPE_FIELD	 EQU	5	; 2 bits
EP0_SETUP_REQUEST_TYPE_DATADIR	 EQU	7	; 1 bit

EP0_SETUP_FEATURE	EQU	2

EP0_SETUP_DSC_INDEX	EQU	2
EP0_SETUP_DSC_TYPE	EQU	3
EP0_SETUP_LANGID_L	EQU	4
EP0_SETUP_LANGID_H	EQU	5

EP0_SETUP_DEVADR_L	EQU	2
EP0_SETUP_DEVADR_H	EQU	3

EP0_SETUP_CFGVALUE	EQU	2
EP0_SETUP_CFGRSD	EQU	3

EP0_SETUP_ALTID_L	EQU	2
EP0_SETUP_ALTID_H	EQU	3
EP0_SETUP_INTFID_L	EQU	4
EP0_SETUP_INTFID_H	EQU	5

EP0_SETUP_EPID_L	EQU	4
EP0_SETUP_EPID_H	EQU	5

EP0_SETUP_EPNUM		EQU	4
EP0_SETUP_EPNUM_FIELD	EQU	0	; 4 bits
EP0_SETUP_EPNUM_EPDIR	EQU	7	; 1 bit

; ==========================================================================


; ==========================================================================

DETACHED_STATE          EQU	0
ATTACHED_STATE          EQU	1
POWERED_STATE           EQU	2
DEFAULT_STATE           EQU	3
ADR_PENDING_STATE       EQU	4
ADDRESS_STATE           EQU	5
CONFIGURED_STATE        EQU	6

WAIT_SETUP          EQU	0
CTRL_TRF_TX         EQU	1
CTRL_TRF_RX         EQU	2

SETUP_TOKEN         EQU	b'00001101'
OUT_TOKEN           EQU	b'00000001'
IN_TOKEN            EQU	b'00001001'

HOST_TO_DEV         EQU	0
DEV_TO_HOST         EQU	1

STANDARD            EQU	0x00
CLASS               EQU	0x01
VENDOR              EQU	0x02

RCPT_DEV            EQU	0
RCPT_INTF           EQU	1
RCPT_EP             EQU	2
RCPT_OTH            EQU	3

OUT         EQU	0
IN          EQU	1

PIC_EP_NUM_MASK EQU	0x78
PIC_EP_DIR_MASK EQU	0x04

EP00_OUT    EQU (0x00<<3)|(OUT<<2)
EP00_IN     EQU (0x00<<3)|(IN<<2)
EP01_OUT    EQU (0x01<<3)|(OUT<<2)
EP01_IN     EQU (0x01<<3)|(IN<<2)
EP02_OUT    EQU (0x02<<3)|(OUT<<2)
EP02_IN     EQU (0x02<<3)|(IN<<2)
EP03_OUT    EQU (0x03<<3)|(OUT<<2)
EP03_IN     EQU (0x03<<3)|(IN<<2)
EP04_OUT    EQU (0x04<<3)|(OUT<<2)
EP04_IN     EQU (0x04<<3)|(IN<<2)
EP05_OUT    EQU (0x05<<3)|(OUT<<2)
EP05_IN     EQU (0x05<<3)|(IN<<2)
EP06_OUT    EQU (0x06<<3)|(OUT<<2)
EP06_IN     EQU (0x06<<3)|(IN<<2)
EP07_OUT    EQU (0x07<<3)|(OUT<<2)
EP07_IN     EQU (0x07<<3)|(IN<<2)
EP08_OUT    EQU (0x08<<3)|(OUT<<2)
EP08_IN     EQU (0x08<<3)|(IN<<2)
EP09_OUT    EQU (0x09<<3)|(OUT<<2)
EP09_IN     EQU (0x09<<3)|(IN<<2)
EP10_OUT    EQU (0x0A<<3)|(OUT<<2)
EP10_IN     EQU (0x0A<<3)|(IN<<2)
EP11_OUT    EQU (0x0B<<3)|(OUT<<2)
EP11_IN     EQU (0x0B<<3)|(IN<<2)
EP12_OUT    EQU (0x0C<<3)|(OUT<<2)
EP12_IN     EQU (0x0C<<3)|(IN<<2)
EP13_OUT    EQU (0x0D<<3)|(OUT<<2)
EP13_IN     EQU (0x0D<<3)|(IN<<2)
EP14_OUT    EQU (0x0E<<3)|(OUT<<2)
EP14_IN     EQU (0x0E<<3)|(IN<<2)
EP15_OUT    EQU (0x0F<<3)|(OUT<<2)
EP15_IN     EQU (0x0F<<3)|(IN<<2)

MUID_NULL               EQU 0
MUID_USB9               EQU 1
MUID_USBMIDI		EQU 16