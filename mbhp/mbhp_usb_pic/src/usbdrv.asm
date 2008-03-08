; $Id$
;
;  USB Driver - inspirated from Microchip CDC example
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	list	p=18f4550
	radix	dec
#include <p18f4550.inc>
#include "macros.inc"
#include "usb_defs.inc"

; ==========================================================================

	;; export lables
	global	USBDRV_Init
	global	USBDRV_Tick
	global	EP0_SETUP
	global	USB_DEVICE_STATE
	global	USBDRV_DTSHandshake

	;; import lables
	extern	_USBDSC_DEVICE
	extern	_USBDSC_CFG_LIST
	extern	_USBDSC_STR_LIST

	extern	USBCLS_CheckUSBMIDIRequest
	extern	USBCLS_USBMIDIInitEP

	extern	MIDI_TxSendDirect

; ==========================================================================

#define USB_ALT_INTF_NUM 2
#define EP0_BUFFER_SIZE	0x40

accessram		udata_acs
USB_DEVICE_STATE	res	1
USB_STAT		res	1
USB_ACTIVE_CFG		res	1
CTRL_TRF_STATE		res	1
CTRL_TRF_SESSION_OWNER	res	1
CTRL_TRF_COUNT_L	res	1
CTRL_TRF_COUNT_H	res	1
CTRL_TRF_SRC_PTR_L	res	1
CTRL_TRF_SRC_PTR_H	res	1
CTRL_TRF_DST_PTR_L	res	1
CTRL_TRF_DST_PTR_H	res	1

USB_ALT_INTF		res	USB_ALT_INTF_NUM

usb5_ep0		udata	0x500
EP0_SETUP		res	EP0_BUFFER_SIZE
EP0_DATA		res	EP0_BUFFER_SIZE

; ==========================================================================
USBDRV	code
; ==========================================================================

USBDRV_Init
	movlw	(1 << UPUEN) | (1 << FSEN)
	movwf	UCFG, ACCESS

	movlw	DETACHED_STATE
	movwf	USB_DEVICE_STATE
	clrf	USB_STAT
	clrf	USB_ACTIVE_CFG

	return

; ==========================================================================
USBDRV_DisableEP1to15
	lfsr	FSR2, UEP1	; clear all EP
USBDRV_DisableEP1to15_Loop
	clrf	POSTINC2
	movlw	LOW(UEP15)
	cpfsgt	FSR2L, ACCESS
	rgoto	USBDRV_DisableEP1to15_Loop
	return

; ==========================================================================
USBDRV_Tick
	call	USBDRV_CheckBusStatus
	btfss	UCFG, UTEYE
	rcall	USBDRV_Service
	return

; --------------------------------------------------------------------------
USBDRV_CheckBusStatus
	;; if module is off, enable it
	btfss	UCON, USBEN, ACCESS
	call	USBDRV_Enable

	;; After enabling the USB module, it takes some time for the voltage
	;; on the D+ or D- line to rise high enough to get out of the SE0 condition.
	;; The USB Reset interrupt should not be unmasked until the SE0 condition is
	;; cleared. This helps preventing the firmware from misinterpreting this
	;; unique event as a USB bus reset from the USB host.
	movlw	ATTACHED_STATE
	cpfseq	USB_DEVICE_STATE, ACCESS
	rgoto	USBDRV_CheckBusStatus_NoAS
USBDRV_CheckBusStatus_AS
	btfsc	UCON, SE0, ACCESS
	rgoto	USBDRV_CheckBusStatus_SE0

	clrf	UIR, ACCESS	; clear all USB interrupts
	clrf	UIE, ACCESS	; mask all USB interrupts
	bsf	UIE, URSTIE, ACCESS; unmask RESET interrupt
	bsf	UIE, IDLEIE, ACCESS; unmask IDLE interrupt
	
	movlw	POWERED_STATE	; transition to next state
	movwf	USB_DEVICE_STATE

USBDRV_CheckBusStatus_SE0
USBDRV_CheckBusStatus_NoAS

	return

; --------------------------------------------------------------------------
USBDRV_Enable
	clrf	UCON, ACCESS
	clrf	UIE, ACCESS	; mask all USB interrupts
	bsf	UCON, USBEN, ACCESS; enable module and attach to bus
	movlw	ATTACHED_STATE	; transition to attached state
	movwf	USB_DEVICE_STATE

	return

; --------------------------------------------------------------------------
USBDRV_Disable
	clrf	UCON, ACCESS	; disable module and detach from bus
	clrf	UIE, ACCESS	; mask all USB interrupts
	movlw	DETACHED_STATE	; transition to detached state
	movwf	USB_DEVICE_STATE

	return

; --------------------------------------------------------------------------
USBDRV_SoftDetach
	goto	USBDRV_Disable

; --------------------------------------------------------------------------
; * Note:            Device state transitions through the following stages:
; *                  DETACHED -> ATTACHED -> POWERED -> DEFAULT ->
; *                  ADDRESS_PENDING -> ADDRESSED -> CONFIGURED -> READY
USBDRV_Service
	;; don't continue if USB cable not attached
	movf	USB_DEVICE_STATE, W
	xorlw	DETACHED_STATE
	bz	USBDRV_Service_End

	;; check for USB activity interrupt
	btfss	UIR, ACTVIF, ACCESS
	rgoto	USBDRV_Service_NoACTVIF
	btfsc	UIE, ACTVIE, ACCESS
	rcall	USBDRV_WakeFromSuspend
USBDRV_Service_NoACTVIF

	;; don't continue if the device is in suspend mode
	btfsc	UCON, SUSPND, ACCESS
	rgoto	USBDRV_Service_End

	;; service USB bus reset interrupt
	btfss	UIR, URSTIF, ACCESS
	rgoto	USBDRV_Service_NoURSTIF
	btfsc	UIE, URSTIE, ACCESS
	rcall	USBDRV_ResetHandler
USBDRV_Service_NoURSTIF

	;; service other USB interrupts
	btfss	UIR, IDLEIF, ACCESS
	rgoto	USBDRV_Service_NoIDLEIF
	btfsc	UIE, IDLEIE, ACCESS
	rcall	USBDRV_Suspend
USBDRV_Service_NoIDLEIF

	btfss	UIR, SOFIF, ACCESS
	rgoto	USBDRV_Service_NoSOFIF
	btfsc	UIE, SOFIE, ACCESS
	rcall	USBDRV_SOFHandler
USBDRV_Service_NoSOFIF

	btfss	UIR, STALLIF, ACCESS
	rgoto	USBDRV_Service_NoSTALLIF
	btfsc	UIE, STALLIE, ACCESS
	rcall	USBDRV_StallHandler
USBDRV_Service_NoSTALLIF

	btfss	UIR, UERRIF, ACCESS
	rgoto	USBDRV_Service_NoUERRIF
	btfsc	UIE, UERRIE, ACCESS
	rcall	USBDRV_ErrorHandler
USBDRV_Service_NoUERRIF

	;; don't continue if host has not sent a bus reset
	movlw	DEFAULT_STATE - 1
	cpfsgt	USB_DEVICE_STATE, ACCESS
	rgoto	USBDRV_Service_End

	;; servicing USB transaction complete interrupt
	btfss	UIR, TRNIF, ACCESS
	rgoto	USBDRV_Service_NoTRNIF
	btfss	UIE, TRNIE, ACCESS
	rgoto	USBDRV_Service_NoTRNIF
	call	USBDRV_CtrlEPService
	bcf	UIR, TRNIF, ACCESS
USBDRV_Service_NoTRNIF
	
USBDRV_Service_End
	return

; --------------------------------------------------------------------------
USBDRV_Suspend
	bsf	UIE, ACTVIE, ACCESS; enable bus activity interrupt
	bcf	UIR, IDLEIF, ACCESS
	bsf	UCON, SUSPND, ACCESS; put USB module in power conserve mode, SIE clock inactive

	;; here we could go into sleep, idle, ...

	bcf	PIR2, USBIF
	bsf	PIE2, USBIE
 	sleep
	bcf	PIE2, USBIE
	return

; --------------------------------------------------------------------------
USBDRV_WakeFromSuspend
	bcf	UCON, SUSPND, ACCESS
	bcf	UIE, ACTVIE, ACCESS
USBDRV_WakeFromSuspendLoop
	bcf	UIR, ACTVIF, ACCESS
	btfsc	UIR, ACTVIF, ACCESS		; loop until flag really cleared (erratum in Microchip framework)
	bra	USBDRV_WakeFromSuspendLoop
	return

; --------------------------------------------------------------------------
USBDRV_ResetHandler
	clrf	UEIR, ACCESS	; clear all USB error flags
	clrf	UIR, ACCESS	; clear all USB interrupts
	movlw	b'10011111'	; unmask all USB error interrupts
	movwf	UEIE, ACCESS
	movlw	b'01111011'	; enable all interrupts except ACTVIE
	movwf	UIE, ACCESS

	clrf	UADDR, ACCESS	; reset to default address
	rcall	USBDRV_DisableEP1to15
	movlw	(1 << EPHSHK) | 0x06	; init EP0 as Ctrl EP
	movwf	UEP0, ACCESS

USBDRV_ResetHandler_FlushIFLoop		; flush any pending transactions
	btfss	UIR, TRNIF, ACCESS
	rgoto	USBDRV_ResetHandler_NoIF
	bcf	UIR, TRNIF, ACCESS
	rgoto	USBDRV_ResetHandler_FlushIFLoop
USBDRV_ResetHandler_NoIF

	bcf	UCON, PKTDIS, ACCESS	; make sure packet processing is enabled
	
	call	USBDRV_PrepareForNextSetupTrf	; see label name...

	clrf	USB_ACTIVE_CFG				; clear active configuration
	movlw	DEFAULT_STATE				; transition to default state
	movwf	USB_DEVICE_STATE
	return

; --------------------------------------------------------------------------
USBDRV_SOFHandler
	bcf	UIR, SOFIF, ACCESS
	return

; --------------------------------------------------------------------------
USBDRV_StallHandler
	btfss	UEP0, EPSTALL, ACCESS
	rgoto	USBDRV_StallHandler_Ignore
	call	USBDRV_PrepareForNextSetupTrf	; "Firmware Workaround"
	bcf	UEP0, EPSTALL, ACCESS
USBDRV_StallHandler_Ignore
	bcf	UIR, STALLIF, ACCESS
	return

; --------------------------------------------------------------------------
USBDRV_ErrorHandler
#if 1
	extern	MIDI_TxBufferPut
	movlw	0xb0
	call	MIDI_TxBufferPut
	movlw	0x00
	call	MIDI_TxBufferPut
	movf	UEIR, W
	call	MIDI_TxBufferPut
#endif
	clrf	UEIR			; Clear all USB error flags, automatically resets UERRIF 
	return

; --------------------------------------------------------------------------
USBDRV_PrepareForNextSetupTrf
	movlw	WAIT_SETUP
	movwf	CTRL_TRF_STATE

	SET_BSR	EP0BO
	movlw	EP0_BUFFER_SIZE
	movwf	EP0BO + BDn_CNT, BANKED

	movlw	LOW(EP0_SETUP)
	movwf	EP0BO + BDn_ADDRL, BANKED
	movlw	HIGH(EP0_SETUP)
	movwf	EP0BO + BDn_ADDRH, BANKED
	movlw	(1 << BDn_STAT_UOWN) | (1 << BDn_STAT_DTSEN)
	movwf	EP0BO + BDn_STAT, BANKED

	movlw	0x00
	movwf	EP0BI + BDn_STAT, BANKED
	return

; --------------------------------------------------------------------------
USBDRV_CtrlEPService
	movlw	EP00_OUT
	cpfseq	USTAT, ACCESS
	rgoto	USBDRV_CtrlEPService_NoOut
USBDRV_CtrlEPService_Out
	SET_BSR	EP0BO
	rrf	EP0BO + BDn_STAT, W, BANKED	; get PID field (STAT[5:2]
	rrf	WREG, W
	andlw	0x0f
	xorlw	SETUP_TOKEN
	bnz	USBDRV_CtrlEPService_Out_NoST
USBDRV_CtrlEPService_Out_ST
	goto	USBDRV_EP0TrfSetupHandler
USBDRV_CtrlEPService_Out_NoST
	goto	USBDRV_EP0TrfOutHandler

USBDRV_CtrlEPService_NoOut
	movlw	EP00_IN
	cpfseq	USTAT, ACCESS
	rgoto	USBDRV_CtrlEPService_NoIn
USBDRV_CtrlEPService_In
	goto	USBDRV_EP0TrfInHandler
USBDRV_CtrlEPService_NoIn

USBDRV_CtrlEPService_End
	return

; --------------------------------------------------------------------------
USBDRV_EP0TrfSetupHandler
	movlw	WAIT_SETUP
	movwf	CTRL_TRF_STATE

	movlw	MUID_NULL
	movwf	CTRL_TRF_SESSION_OWNER

	clrf	CTRL_TRF_COUNT_L
	clrf	CTRL_TRF_COUNT_H

	call	USBDRV_CheckStdRequest
	movlw	MUID_NULL
	cpfseq	CTRL_TRF_SESSION_OWNER, ACCESS
	rgoto	USBDRV_EP0TrfSetup_GotOwner
	call	USBCLS_CheckUSBMIDIRequest
USBDRV_EP0TrfSetup_GotOwner

	;; service complete
	;; PKTDIS moved to the beginning - see PIC18F4550 Errata Sheet
	bcf	UCON, PKTDIS, ACCESS	; resume packet processing

	movlw	MUID_NULL
	cpfseq	CTRL_TRF_SESSION_OWNER, ACCESS
	rgoto	USBDRV_EP0TrfSetupHandler_NoM
USBDRV_EP0TrfSetupHandler_M
	SET_BSR	EP0BO
	movlw	EP0_BUFFER_SIZE
	movwf	EP0BO + BDn_CNT, BANKED
	movlw	LOW(EP0_SETUP)
	movwf	EP0BO + BDn_ADDRL, BANKED
	movlw	HIGH(EP0_SETUP)
	movwf	EP0BO + BDn_ADDRH, BANKED
	movlw	(1 << BDn_STAT_UOWN) | (1 << BDn_STAT_BSTALL)
	movwf	EP0BO + BDn_STAT, BANKED
	movwf	EP0BI + BDn_STAT, BANKED
	rgoto	USBDRV_EP0TrfSetupHandler_End
	
USBDRV_EP0TrfSetupHandler_NoM
	SET_BSR	EP0_SETUP
	btfss	EP0_SETUP + EP0_SETUP_REQUEST_TYPE, EP0_SETUP_REQUEST_TYPE_DATADIR, BANKED
	rgoto	USBDRV_EP0TrfSetupHandler_NoDH
USBDRV_EP0TrfSetupHandler_DH	; DEV_TO_HOST
	;; if(SetupPkt.wLength < wCount._word)
	movf	CTRL_TRF_COUNT_L, W
	subwf	EP0_SETUP + EP0_SETUP_LENGTH_L, W, BANKED
	movf	CTRL_TRF_COUNT_H, W
	subwfb	EP0_SETUP + EP0_SETUP_LENGTH_H, W, BANKED
	bc	USBDRV_EP0TrfSetupHandler_DH_Nlt
USBDRV_EP0TrfSetupHandler_DH_lt
	movff	EP0_SETUP + EP0_SETUP_LENGTH_L, CTRL_TRF_COUNT_L
	movff	EP0_SETUP + EP0_SETUP_LENGTH_H, CTRL_TRF_COUNT_H
USBDRV_EP0TrfSetupHandler_DH_Nlt

	call	USBDRV_TrfTxService
	movlw	CTRL_TRF_TX
	movwf	CTRL_TRF_STATE

	SET_BSR	EP0BO
	movlw	EP0_BUFFER_SIZE
	movwf	EP0BO + BDn_CNT, BANKED
	movlw	LOW(EP0_SETUP)
	movwf	EP0BO + BDn_ADDRL, BANKED
	movlw	HIGH(EP0_SETUP)
	movwf	EP0BO + BDn_ADDRH, BANKED
	movlw	(1 << BDn_STAT_UOWN)
	movwf	EP0BO + BDn_STAT, BANKED

	;; (count should have been initialized by responsible request owner)
	movlw	LOW(EP0_DATA)
	movwf	EP0BI + BDn_ADDRL, BANKED
	movlw	HIGH(EP0_DATA)
	movwf	EP0BI + BDn_ADDRH, BANKED
	movlw	(1 << BDn_STAT_UOWN) | (1 << BDn_STAT_DTS) | (1 << BDn_STAT_DTSEN)
	movwf	EP0BI + BDn_STAT, BANKED

	rgoto	USBDRV_EP0TrfSetupHandler_End

USBDRV_EP0TrfSetupHandler_NoDH	; HOST_TO_DEV
	movlw	CTRL_TRF_RX
	movwf	CTRL_TRF_STATE

	SET_BSR	EP0BI
	clrf	EP0BI + BDn_CNT, BANKED
	movlw	(1 << BDn_STAT_UOWN) | (1 << BDn_STAT_DTS) | (1 << BDn_STAT_DTSEN)
	movwf	EP0BI + BDn_STAT, BANKED

	movlw	EP0_BUFFER_SIZE
	movwf	EP0BO + BDn_CNT, BANKED

	movlw	LOW(EP0_DATA)
	movwf	EP0BO + BDn_ADDRL, BANKED
	movlw	HIGH(EP0_DATA)
	movwf	EP0BO + BDn_ADDRH, BANKED
	movlw	(1 << BDn_STAT_UOWN) | (1 << BDn_STAT_DTS) | (1 << BDn_STAT_DTSEN)
	movwf	EP0BO + BDn_STAT, BANKED

USBDRV_EP0TrfSetupHandler_End
	;; 	bcf	UCON, PKTDIS, ACCESS	; resume packet processing
	return

; --------------------------------------------------------------------------
USBDRV_EP0TrfOutHandler
	movlw	CTRL_TRF_RX
	cpfseq	CTRL_TRF_STATE, ACCESS
	rgoto	USBDRV_EP0TrfOutHandler_NoRx
USBDRV_EP0TrfOutHandler_Rx
	call	USBDRV_TrfRxService

	lfsr	FSR1, EP0BO + BDn_STAT
	call	USBDRV_DTSHandshake

	rgoto	USBDRV_EP0TrfOutHandler_End

USBDRV_EP0TrfOutHandler_NoRx
	call	USBDRV_PrepareForNextSetupTrf

USBDRV_EP0TrfOutHandler_End
	return

; --------------------------------------------------------------------------
USBDRV_EP0TrfInHandler
	movlw	ADR_PENDING_STATE
	cpfseq	USB_DEVICE_STATE, ACCESS
	rgoto	USBDRV_EP0TrfInHandler_NoAP
USBDRV_EP0TrfInHandler_AP
	SET_BSR	EP0_SETUP
	movf	EP0_SETUP + EP0_SETUP_DEVADR_L, W, BANKED
	movwf	UADDR, ACCESS
	movlw	ADDRESS_STATE
	skpnz
	movlw	DEFAULT_STATE
	movwf	USB_DEVICE_STATE
USBDRV_EP0TrfInHandler_NoAP

	movlw	CTRL_TRF_TX
	cpfseq	CTRL_TRF_STATE, ACCESS
	rgoto	USBDRV_EP0TrfInHandler_NoTx
USBDRV_EP0TrfInHandler_Tx
	call	USBDRV_TrfTxService

	lfsr	FSR1, EP0BI + BDn_STAT
	call	USBDRV_DTSHandshake

	rgoto	USBDRV_EP0TrfInHandler_End

USBDRV_EP0TrfInHandler_NoTx
	call	USBDRV_PrepareForNextSetupTrf

USBDRV_EP0TrfInHandler_End
	return

; --------------------------------------------------------------------------
USBDRV_TrfTxService
	;; if(wCount._word < EP0_BUFF_SIZE)
	movlw	EP0_BUFFER_SIZE
	subwf	CTRL_TRF_COUNT_L, W
	movlw	0
	subwfb	CTRL_TRF_COUNT_H, W
	bc	USBDRV_TrfTxService_Nlt
USBDRV_TrfTxService_lt
	movff	CTRL_TRF_COUNT_L, PRODL
	movff	CTRL_TRF_COUNT_H, PRODH
	rgoto	USBDRV_TrfTxService_lt_C
USBDRV_TrfTxService_Nlt
	movlw	EP0_BUFFER_SIZE
	movwf	PRODL
	clrf	PRODH
USBDRV_TrfTxService_lt_C

	SET_BSR	EP0BI
	movlw	0x03		; (mask out to be sure...)
	andwf	PRODH, F
	movf	EP0BI + BDn_STAT, W, BANKED
	andlw	0xfc
	iorwf	PRODH, W
	movwf	EP0BI + BDn_STAT, BANKED
	movff	PRODL, EP0BI + BDn_CNT

	;; CTRL_TRF_COUNT_[LH] -= PROD[LH]
	movf	PRODL, W
	subwf	CTRL_TRF_COUNT_L, F
	movf	PRODH, W
	subwfb	CTRL_TRF_COUNT_H, F

	lfsr	FSR2, EP0_DATA

	btfsc	USB_STAT, USB_STAT_RAM_PTR
	rgoto	USBDRV_TrfTxService_RdRAM
USBDRV_TrfTxService_RdROM
	movff	CTRL_TRF_SRC_PTR_L, TBLPTRL
	movff	CTRL_TRF_SRC_PTR_H, TBLPTRH
USBDRV_TrfTxService_ROMLoop
	movf	PRODL, W
	iorwf	PRODH, W
	bz	USBDRV_TrfTxService_ROMLoopEnd
	
	tblrd*+
	movff	TABLAT, POSTINC2

	decf	PRODL, F
	skpc
	decf	PRODH, F
	rgoto	USBDRV_TrfTxService_ROMLoop
USBDRV_TrfTxService_ROMLoopEnd
	movff	TBLPTRL, CTRL_TRF_SRC_PTR_L
	movff	TBLPTRH, CTRL_TRF_SRC_PTR_H
	rgoto	USBDRV_TrfTxService_End

USBDRV_TrfTxService_RdRAM
	movff	CTRL_TRF_SRC_PTR_L, FSR1L
	movff	CTRL_TRF_SRC_PTR_H, FSR1H
USBDRV_TrfTxService_RAMLoop
	movf	PRODL, W
	iorwf	PRODH, W
	bz	USBDRV_TrfTxService_RAMLoopEnd

	movff	POSTINC1, POSTINC2
	
	decf	PRODL, F
	skpc
	decf	PRODH, F
	rgoto	USBDRV_TrfTxService_RAMLoop
USBDRV_TrfTxService_RAMLoopEnd
	movff	FSR1L, CTRL_TRF_SRC_PTR_L
	movff	FSR1H, CTRL_TRF_SRC_PTR_H
	rgoto	USBDRV_TrfTxService_End

USBDRV_TrfTxService_End

	return

; --------------------------------------------------------------------------
USBDRV_TrfRxService
	SET_BSR	EP0BO
	movff	EP0BO + BDn_CNT, PRODL
	movf	EP0BO + BDn_STAT, W, BANKED
	andlw	0x03
	movwf	PRODH

	;; CTRL_TRF_COUNT_[LH] += PROD[LH]
	movf	PRODL, W
	addwf	CTRL_TRF_COUNT_L, F
	movf	PRODH, W
	addwfc	CTRL_TRF_COUNT_H, F

	lfsr	FSR2, EP0_DATA
	movff	CTRL_TRF_DST_PTR_L, FSR1L
	movff	CTRL_TRF_DST_PTR_H, FSR1H

USBDRV_TrfRxService_Loop
	movf	PRODL, W
	iorwf	PRODH, W
	bz	USBDRV_TrfRxService_LoopEnd

	movff	POSTINC2, POSTINC1

	decf	PRODL, F
	skpc
	decf	PRODH, F
	rgoto	USBDRV_TrfRxService_Loop
USBDRV_TrfRxService_LoopEnd
	movff	FSR1L, CTRL_TRF_DST_PTR_L
	movff	FSR1H, CTRL_TRF_DST_PTR_H
	
	return

; --------------------------------------------------------------------------
USBDRV_CheckStdRequest
	SET_BSR	EP0_SETUP
	swapf	EP0_SETUP + EP0_SETUP_REQUEST_TYPE, W, BANKED	; request type is bit [6:5]
	rrf	WREG, W
	andlw	0x03
	xorlw	STANDARD
	bnz	USBDRV_CheckStdRequest_End

	movf	EP0_SETUP + EP0_SETUP_REQUEST, W, BANKED
	bz	USBDRV_StdRequest_GetStatus	; 0
	addlw	-1
	bz	USBDRV_StdRequest_ClrFeature	; 1
	addlw	-2
	bz	USBDRV_StdRequest_SetFeature	; 3
	addlw	-2
	bz	USBDRV_StdRequest_SetAdr	; 5
	addlw	-1
	bz	USBDRV_StdRequest_GetDsc	; 6
	addlw	-1
	bz	USBDRV_StdRequest_SetDsc	; 7
	addlw	-1
	bz	USBDRV_StdRequest_GetCfg	; 8
	addlw	-1
	bz	USBDRV_StdRequest_SetCfg	; 9
	addlw	-1
	bz	USBDRV_StdRequest_GetIntf	; 10
	addlw	-1
	bz	USBDRV_StdRequest_SetIntf	; 11
	addlw	-1
	bz	USBDRV_StdRequest_SynchFrame	; 12
	rgoto	USBDRV_CheckStdRequest_End

; ------
USBDRV_StdRequest_GetStatus
	call	USBDRV_GetStatus_Handler
	rgoto	USBDRV_CheckStdRequest_End

USBDRV_StdRequest_ClrFeature
USBDRV_StdRequest_SetFeature
	;; (both handled by same routine)
	call	USBDRV_ReqFeatureHandler
	rgoto	USBDRV_CheckStdRequest_End

USBDRV_StdRequest_SetAdr
	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER
	movlw	ADR_PENDING_STATE
	movwf	USB_DEVICE_STATE
	rgoto	USBDRV_CheckStdRequest_End

USBDRV_StdRequest_GetDsc
	call	USBDRV_GetDscHandler
	rgoto	USBDRV_CheckStdRequest_End

USBDRV_StdRequest_SetDsc
	rgoto	USBDRV_CheckStdRequest_End

USBDRV_StdRequest_GetCfg
	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER

	bsf	USB_STAT, USB_STAT_RAM_PTR	; set memory type to RAM
	movlw	LOW(USB_ACTIVE_CFG)
	movwf	CTRL_TRF_SRC_PTR_L
	movlw	HIGH(USB_ACTIVE_CFG)
	movwf	CTRL_TRF_SRC_PTR_H
	movlw	1
	movwf	CTRL_TRF_COUNT_L
	clrf	CTRL_TRF_COUNT_H
	rgoto	USBDRV_CheckStdRequest_End	

; ------
USBDRV_StdRequest_SetCfg
	call	USBDRV_SetCfgHandler
	rgoto	USBDRV_CheckStdRequest_End

USBDRV_StdRequest_GetIntf
	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER

	bsf	USB_STAT, USB_STAT_RAM_PTR	; set memory type to RAM
	movlw	LOW(USB_ALT_INTF)
	addwf	EP0_SETUP + EP0_SETUP_INTFID_L, W, BANKED
	movwf	CTRL_TRF_SRC_PTR_L
	movlw	HIGH(USB_ALT_INTF)
	;; 	addwfc	EP0_SETUP + EP0_SETUP_INTFID_H, W, BANKED
	movwf	CTRL_TRF_SRC_PTR_H
	movlw	1
	movwf	CTRL_TRF_COUNT_L
	clrf	CTRL_TRF_COUNT_H
	rgoto	USBDRV_CheckStdRequest_End

USBDRV_StdRequest_SetIntf
	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER

	lfsr	FSR2, USB_ALT_INTF
	SET_BSR	EP0_SETUP
	movf	EP0_SETUP + EP0_SETUP_INTFID_L, W, BANKED
	addwf	FSR2L, F
	movff	EP0_SETUP + EP0_SETUP_ALTID_L, INDF2
	rgoto	USBDRV_CheckStdRequest_End
	
USBDRV_StdRequest_SynchFrame
	;; 	rgoto	USBDRV_CheckStdRequest_End
	
USBDRV_CheckStdRequest_End
	return

; --------------------------------------------------------------------------
USBDRV_GetStatus_Handler
	SET_BSR	EP0_DATA
	clrf	EP0_DATA + 0, BANKED	; initialize content
	clrf	EP0_DATA + 1, BANKED

#if 0	; (avoid transfer to WREG due to interrupt bug @ silicon)
	movff	EP0_SETUP + EP0_SETUP_REQUEST_TYPE, WREG
#else
	movff	EP0_SETUP + EP0_SETUP_REQUEST_TYPE, PRODL
	movf	PRODL, W
#endif
	andlw	0x1f		; 5 bitfield
	bz	USBDRV_GetStatus_RcptDev
	addlw	-1
	bz	USBDRV_GetStatus_RcptIntf
	addlw	-1
	bz	USBDRV_GetStatus_RcptEP
	;; 	addlw	-1
	;; 	bz	USBDRV_GetStatus_GetStatus_OTH
	rgoto	USBDRV_GetStatus_GetStatus_Cont

USBDRV_GetStatus_RcptDev
	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER
	;; bit0: Self-Powered Status [0] Bus-Powered [1] Self-Powered
	;; bit1: RemoteWakeup        [0] Disabled    [1] Enabled
	movlw	0x00
	movwf	EP0_DATA + 0, BANKED
	rgoto	USBDRV_GetStatus_GetStatus_Cont

USBDRV_GetStatus_RcptIntf
	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER
	rgoto	USBDRV_GetStatus_GetStatus_Cont

USBDRV_GetStatus_RcptEP
	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER

	rcall	USBDRV_GetEPPtr	; get FSR2 pointer to EPxB[OI] depending on EPNUM and direction field

	;; bit0: Halt Status [0] Not Halted [1] Halted
	SET_BSR	EP0_DATA
	movlw	0x00
	btfsc	INDF2, BDn_STAT_BSTALL
	movlw	0x01
	movwf	EP0_DATA + 0, BANKED
	rgoto	USBDRV_GetStatus_GetStatus_Cont

USBDRV_GetStatus_GetStatus_Cont

	movlw	MUID_USB9
	cpfseq	CTRL_TRF_SESSION_OWNER, ACCESS
	rgoto	USBDRV_GetStatus_End
	movlw	LOW(EP0_SETUP)			; set source pointer
	movwf	CTRL_TRF_SRC_PTR_L
	movlw	HIGH(EP0_SETUP)
	movwf	CTRL_TRF_SRC_PTR_H
	bsf	USB_STAT, USB_STAT_RAM_PTR	; set memory type to RAM
	movlw	2				; set data count
	movwf	CTRL_TRF_COUNT_L
	clrf	CTRL_TRF_COUNT_H

USBDRV_GetStatus_End
	return

; --------------------------------------------------------------------------
USBDRV_ReqFeatureHandler
	SET_BSR	EP0_SETUP
	movf	EP0_SETUP + EP0_SETUP_FEATURE, W, BANKED
	xorlw	0x01		; DEVICE_REMOTE_SETUP
	bnz	USBDRV_ReqFeature_NoDRS
USBDRV_ReqFeature_DRS
	movf	EP0_SETUP + EP0_SETUP_REQUEST_TYPE, W, BANKED
	andlw	0x1f		; bit 4:0
	xorlw	RCPT_DEV
	bnz	USBDRV_ReqFeature_NoDRS

	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER
	movf	EP0_SETUP + EP0_SETUP_REQUEST, W, BANKED
	xorlw	0x03		; SET_FEATURE
	bcf	USB_STAT, USB_STAT_REMOTE_WAKEUP
	skpz
	bsf	USB_STAT, USB_STAT_REMOTE_WAKEUP
USBDRV_ReqFeature_NoDRS

	movf	EP0_SETUP + EP0_SETUP_FEATURE, W, BANKED
	xorlw	0x00		; ENDPOINT_HALT
	bnz	USBDRV_ReqFeature_NoHEP
USBDRV_ReqFeature_HEP
	movf	EP0_SETUP + EP0_SETUP_REQUEST_TYPE, W, BANKED
	andlw	0x1f		; bit 4:0
	xorlw	RCPT_EP
	bnz	USBDRV_ReqFeature_NoHEP
	movf	EP0_SETUP + EP0_SETUP_EPNUM, W, BANKED
	andlw	0x0f		; (bitfield 3:0)
	bz	USBDRV_ReqFeature_NoHEP

	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER
	rcall	USBDRV_GetEPPtr	; get FSR2 pointer to EPxB[OI] depending on EPNUM and direction field
	movf	EP0_SETUP + EP0_SETUP_REQUEST, W, BANKED
	xorlw	0x03		; SET_FEATURE
	bnz	USBDRV_ReqFeatureHEP0
USBDRV_ReqFeatureHEP1
	movlw	(1 << BDn_STAT_UOWN) | (1 << BDn_STAT_BSTALL)
	movwf	INDF2
	rgoto	USBDRV_ReqFeature_NoHEP

USBDRV_ReqFeatureHEP0
	movlw	(1 << BDn_STAT_DTS)	; IN - CPU owns buffer, DTS must be set (framework erratum)
	btfss	EP0_SETUP + EP0_SETUP_EPNUM, EP0_SETUP_EPNUM_EPDIR, BANKED
	movlw	(1 << BDn_STAT_UOWN) | (1 << BDn_STAT_DTSEN)	; OUT - SIE owns buffer
	movwf	INDF2
USBDRV_ReqFeature_NoHEP
USBDRV_ReqFeature_End
	return

; --------------------------------------------------------------------------
USBDRV_GetDscHandler
	SET_BSR	EP0_SETUP
	movlw	0x80
	cpfseq	EP0_SETUP + EP0_SETUP_REQUEST_TYPE, BANKED
	rgoto	USBDRV_GetDsc_End

	decf	EP0_SETUP + EP0_SETUP_DSC_TYPE, W, BANKED
	bz	USBDRV_GetDsc_DEV	; 1
	addlw	-1
	bz	USBDRV_GetDsc_CFG	; 2
	addlw	-1
	bz	USBDRV_GetDsc_STR	; 3
	addlw	-1
	bz	USBDRV_GetDsc_INTF	; 4
	addlw	-1
	bz	USBDRV_GetDsc_EP	; 5
	rgoto	USBDRV_GetDsc_End

USBDRV_GetDsc_DEV
	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER

	bcf	USB_STAT, USB_STAT_RAM_PTR	; set memory type to ROM

	movlw	LOW(_USBDSC_DEVICE)		; copy source pointer + offset to SRC_PTR_[LH] and TBLPTR[LH]
	movwf	CTRL_TRF_SRC_PTR_L
	movwf	TBLPTRL

	movlw	HIGH(_USBDSC_DEVICE)
	movwf	CTRL_TRF_SRC_PTR_H
	movwf	TBLPTRH

	tblrd*+					; get descriptor size
	movff	TABLAT, CTRL_TRF_COUNT_L
	clrf	CTRL_TRF_COUNT_H
	rgoto	USBDRV_GetDsc_End

USBDRV_GetDsc_CFG
	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER

	bcf	USB_STAT, USB_STAT_RAM_PTR	; set memory type to ROM

	TABLE_ADDR _USBDSC_CFG_LIST
	clrc
	rlf	EP0_SETUP + EP0_SETUP_DSC_INDEX, W, BANKED
	TABLE_ADD_W
	tblrd*+
	movff	TABLAT, CTRL_TRF_SRC_PTR_L
	tblrd*+
	movff	TABLAT, CTRL_TRF_SRC_PTR_H

	movff	CTRL_TRF_SRC_PTR_L, TBLPTRL
	movff	CTRL_TRF_SRC_PTR_H, TBLPTRH

	tblrd*+
	tblrd*+
	tblrd*+					; get descriptor size
	movff	TABLAT, CTRL_TRF_COUNT_L
	tblrd*+
	movff	TABLAT, CTRL_TRF_COUNT_H

	rgoto	USBDRV_GetDsc_End

USBDRV_GetDsc_STR
	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER

	bcf	USB_STAT, USB_STAT_RAM_PTR	; set memory type to ROM

	TABLE_ADDR _USBDSC_STR_LIST
	clrc
	rlf	EP0_SETUP + EP0_SETUP_DSC_INDEX, W, BANKED
	TABLE_ADD_W
	tblrd*+
	movff	TABLAT, CTRL_TRF_SRC_PTR_L
	tblrd*+
	movff	TABLAT, CTRL_TRF_SRC_PTR_H

	movff	CTRL_TRF_SRC_PTR_L, TBLPTRL
	movff	CTRL_TRF_SRC_PTR_H, TBLPTRH

	tblrd*+					; get descriptor size
	movff	TABLAT, CTRL_TRF_COUNT_L
	clrf	CTRL_TRF_COUNT_H

	rgoto	USBDRV_GetDsc_End

USBDRV_GetDsc_INTF
USBDRV_GetDsc_EP
USBDRV_GetDsc_End
	return

; --------------------------------------------------------------------------
USBDRV_SetCfgHandler
	movlw	MUID_USB9
	movwf	CTRL_TRF_SESSION_OWNER
	call	USBDRV_DisableEP1to15

	lfsr	FSR2, USB_ALT_INTF
	movlw	USB_ALT_INTF_NUM
	movwf	PRODL
USBDRV_SetCfgHandler_ClrLoop
	clrf	POSTINC2
	decfsz	PRODL, F
	rgoto	USBDRV_SetCfgHandler_ClrLoop

	SET_BSR	EP0_SETUP
	movf	EP0_SETUP + EP0_SETUP_CFGVALUE, W, BANKED
	movwf	USB_ACTIVE_CFG
	bnz	USBDRV_SetCfg_CState
USBDRV_SetCfg_AState
	movlw	ADDRESS_STATE
	movwf	USB_DEVICE_STATE
	rgoto	USBDRV_SetCfg_End

USBDRV_SetCfg_CState
	movlw	CONFIGURED_STATE
	movwf	USB_DEVICE_STATE

	call	USBCLS_USBMIDIInitEP
USBDRV_SetCfg_End
	return

; --------------------------------------------------------------------------
USBDRV_GetEPPtr
	;; get FSR2 pointer to EPxB[OI] depending on EPNUM and direction field
	lfsr	FSR2, EP0BO
	SET_BSR	EP0_SETUP
	movf	EP0_SETUP + EP0_SETUP_EPNUM, W, BANKED
	andlw	0x0f		; EPNUM field is in 3..0
	mullw	8
	movf	PRODL, W
	addwf	FSR2L, F
	movf	PRODH, W
	addwfc	FSR2H, F
	movlw	0x00
	btfsc	EP0_SETUP + EP0_SETUP_EPNUM, EP0_SETUP_EPNUM_EPDIR, BANKED
	movlw	0x04
	addwf	FSR2L, F
	return

; --------------------------------------------------------------------------
; toggles DTS, expects pointer to BDn_STAT register in FSR1
USBDRV_DTSHandshake
	btfsc	INDF1, BDn_STAT_DTS
	rgoto	USBDRV_DTSHandshake_DTS1
USBDRV_DTSHandshake_DTS0
	movlw	(1 << BDn_STAT_UOWN) | (1 << BDn_STAT_DTS) | (1 << BDn_STAT_DTSEN)
	movwf	INDF1
	return
USBDRV_DTSHandshake_DTS1
	movlw	(1 << BDn_STAT_UOWN) | (1 << BDn_STAT_DTSEN)
	movwf	INDF1
	return

; ==========================================================================
	END
