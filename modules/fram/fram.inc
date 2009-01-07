; FRAM Module
;
; ==========================================================================
;
;  Copyright 2009 Matthias Mächler (maechler@mm-computing.ch / thismaechler@gmx.ch)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

; If set to 1, mios_iic functions will be used (@400kHZ)
#ifndef FRAM_MIOS_IIC
#define FRAM_MIOS_IIC 0
#endif

;includes 1MHZ IIC driver if needed. maps functions either from mios_iic or fram_iic
#if FRAM_MIOS_IIC==0
#include "fram_iic.inc"
#define IIC_Start FRAM_IIC_Start
#define IIC_Stop FRAM_IIC_Stop
#define IIC_AckSend FRAM_IIC_AckSend
#define IIC_NakSend FRAM_IIC_NakSend
#define IIC_ByteReceive FRAM_IIC_ByteReceive
#define IIC_ByteSend FRAM_IIC_ByteSend
#else
#define IIC_Start MIOS_IIC_Start
#define IIC_Stop MIOS_IIC_Stop
#define IIC_AckSend MIOS_IIC_AckSend
#define IIC_NakSend MIOS_IIC_NakSend
#define IIC_ByteReceive MIOS_IIC_ByteReceive
#define IIC_ByteSend MIOS_IIC_ByteSend
#endif


#define slave_id 0xA0 ;high nibble is the slave ID (b1010)

;; --------------------------------------------------------------------------
;;  FUNCTION: mod_skel 
;;  DESCRIPTION: mod_skel. does something stupid. Put your code in here instead.
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: - mod_skel_var
;; --------------------------------------------------------------------------

;_mod_skel_function ; (for C)
;mod_skel_function
;#if _mod_skel_def==1	; note this is using the define from the fix in the asm file!
;	incf _mod_skel_var, F, BANKED
;#endif
;	call _mod_skel_extfunc
;	return

