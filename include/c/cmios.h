// $Id$
/*
 * Declaration of MIOS variables and functions
 *
 * ==========================================================================
 *
 *  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _CMIOS_H
#define _CMIOS_H

/*** hardware flags **********************************************************/
#include <hw_flags.h>

/*** general MIOS definitions ************************************************/
#define MIOS_MIDI_INTERFACE_COMMON	0x00
#define MIOS_MIDI_INTERFACE_TO_HOST	0x01

#define MIOS_MIDI_MERGER_DISABLED	0x00
#define MIOS_MIDI_MERGER_ENABLED	0x01
#define MIOS_MIDI_MERGER_MBLINK_EP	0x02
#define MIOS_MIDI_MERGER_MBLINK_FP	0x03

#ifdef MIOS_OLD_ENCODER_MODES
#define MIOS_ENC_MODE_NON_DETENTED	0x00
#define MIOS_ENC_MODE_DETENTED		0x80
#define MIOS_ENC_MODE_DETENTED1		0x80
#define MIOS_ENC_MODE_DETENTED2		0x81
#define MIOS_ENC_MODE_DETENTED3		0x82
#else
#define MIOS_ENC_MODE_NON_DETENTED	0xff
#define MIOS_ENC_MODE_DETENTED		0xaa
#define MIOS_ENC_MODE_DETENTED1		0xaa
#define MIOS_ENC_MODE_DETENTED2		0x22
#define MIOS_ENC_MODE_DETENTED3		0x88
#define MIOS_ENC_MODE_DETENTED4		0xa5
#define MIOS_ENC_MODE_DETENTED5		0x5a
#endif

#define MIOS_ENC_SPEED_SLOW		0x00
#define MIOS_ENC_SPEED_NORMAL		0x01
#define MIOS_ENC_SPEED_FAST		0x02

#define MIOS_LCD_TYPE_CLCD		0x00
#define MIOS_LCD_TYPE_GLCD0		0x01
#define MIOS_LCD_TYPE_GLCD1		0x02
#define MIOS_LCD_TYPE_GLCD2		0x03
#define MIOS_LCD_TYPE_GLCD3		0x04
#define MIOS_LCD_TYPE_GLCD4		0x05
#define MIOS_LCD_TYPE_MLCD		0x06
#define MIOS_LCD_TYPE_GLCD_CUSTOM	0x07

#define MIOS_GLCD_FONT			0x7cfc

/*** macro for creating encoder table ****************************************/
#define MIOS_ENC_TABLE \
        const unsigned int __at(0x3280) mios_enc_pin_table[64] =

#define MIOS_ENC_ENTRY(sr, din, mode)   ((mode) << 8) | ((din) + 8*((sr)-1))
#define MIOS_ENC_EOT                    0xffff

// export table for external access to array
extern const unsigned int __at(0x3280) mios_enc_pin_table[64];

/*** macro for creating midi processing table *********************************/
#define MIOS_MT_TABLE \
        const unsigned int __at(0x3080) mios_mproc_event_table[256] =

#define MIOS_MT_ENTRY(evnt0, evnt1)     ((evnt1) << 8) | (evnt0)
#define MIOS_MT_EOT                     0xffff

// export table for external access to array
extern const unsigned int __at(0x3080) mios_mproc_event_table[256];

/*** structures of the CFG and STAT bytes ************************************/
typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned LCD_TYPE:3;  // selects LCD type
    unsigned USE_GLCD:1;  // if 1, graphical LCD is connected
    unsigned MERGER:1;    // if 1, MIDI merger is enabled
    unsigned MBLINK:1;    // if 1, MIDIbox Link is enabled
    unsigned TO_HOST:1;   // if 1, MIDI interface will run with 38400 baud instead of 31250
    unsigned FRQ_20MHz:1; // if 1, it is assumed that the bux is running with 20 MHz, else with 40 MHz
  };
} mios_box_cfg0_t;

typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned BS_A:3;            // A2..A0 of BankStick
    unsigned BS_DIS_VERIFY:1;   // if 1, writes to the BankStick will not be verified
    unsigned IIC_STRETCH_CLK:1; // if 1, IIC clock stretching enabled
    unsigned :1;
    unsigned :1;
    unsigned :1;
  };
} mios_box_cfg1_t;

typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned BS_AVAILABLE:1;        // if 1, BankStick is available
    unsigned MBLINK_TUNNEL_PASS:1;  // if 1, a MBLinked event will be tunnled
    unsigned SUSPEND_ALL:1;         // if 1, all system and user tasks are suspended
    unsigned SUSPEND_USER:1;        // if 1, all user tasks are suspended
    unsigned AUTOREPEAT:1;          // if 1, start the auto-repeat handler
    unsigned MLCD_TRANSFER:1;       // if 1, a transfer to the MLCD has been started
    unsigned :1;
    unsigned :1;
  };
} mios_box_stat_t;


/*** MIOS tables *************************************************************/
extern const unsigned char MIOS_MPROC_EVENT_TABLE[2*128];
extern const unsigned char MIOS_ENC_PIN_TABLE[2*64];


/*** Addresses of global MIOS variables **************************************/
extern volatile mios_box_cfg0_t __at 0x000 MIOS_BOX_CFG0;
extern volatile mios_box_cfg1_t __at 0x001 MIOS_BOX_CFG1;
extern volatile mios_box_stat_t __at 0x002 MIOS_BOX_STAT;
extern volatile unsigned char __at 0x003 MIOS_PARAMETER1;
extern volatile unsigned char __at 0x004 MIOS_PARAMETER2;
extern volatile unsigned char __at 0x005 MIOS_PARAMETER3;


/*** Global MIOS functions ***************************************************/
extern void MIOS_MIDI_BeginStream(void) __wparam;
extern void MIOS_MIDI_DeviceIDAutoSet(void) __wparam;
extern unsigned char MIOS_MIDI_DeviceIDGet(void) __wparam;
extern void MIOS_MIDI_DeviceIDSet(unsigned char device_id) __wparam;
extern void MIOS_MIDI_EndStream(void) __wparam;
extern void MIOS_MIDI_Init(void) __wparam;
extern void MIOS_MIDI_InterfaceAutoSet(void) __wparam;
extern unsigned char MIOS_MIDI_InterfaceGet(void) __wparam;
extern void MIOS_MIDI_InterfaceSet(unsigned char interface) __wparam;
extern unsigned char MIOS_MIDI_MergerGet(void) __wparam;
extern void MIOS_MIDI_MergerSet(unsigned char mode) __wparam;
extern unsigned char MIOS_MIDI_RxBufferFree(void) __wparam;
extern unsigned char MIOS_MIDI_RxBufferGet(void) __wparam;
extern void MIOS_MIDI_RxBufferPut(unsigned char b) __wparam;
extern unsigned char MIOS_MIDI_RxBufferUsed(void) __wparam;
extern void MIOS_MIDI_TxBufferFlush(void) __wparam;
extern unsigned char MIOS_MIDI_TxBufferFree(void) __wparam;
extern unsigned char MIOS_MIDI_TxBufferGet(void) __wparam;
extern void MIOS_MIDI_TxBufferPut(unsigned char b) __wparam;
extern unsigned char MIOS_MIDI_TxBufferUsed(void) __wparam;

extern void MIOS_MPROC_MergerDisable(void) __wparam;
extern void MIOS_MPROC_MergerEnable(void) __wparam;
extern void MIOS_MPROC_MergerGet(void) __wparam;

extern unsigned char MIOS_AIN_DeadbandGet(void) __wparam;
extern void MIOS_AIN_DeadbandSet(unsigned char) __wparam;
extern void MIOS_AIN_Muxed(void) __wparam;
extern unsigned char MIOS_AIN_NumberGet(void) __wparam;
extern void MIOS_AIN_NumberSet(unsigned char pots) __wparam;
extern unsigned char MIOS_AIN_Pin7bitGet(unsigned char pot) __wparam;
extern unsigned int MIOS_AIN_PinGet(unsigned char pot) __wparam;
extern void MIOS_AIN_PinLSBGet(unsigned char pot) __wparam;
extern void MIOS_AIN_PinMSBGet(unsigned char pot) __wparam;
extern void MIOS_AIN_UnMuxed(void) __wparam;

extern unsigned char MIOS_MF_PWM_DutyCycleDownGet(void) __wparam;
extern void MIOS_MF_PWM_DutyCycleDownSet(unsigned char cali_down) __wparam;
extern unsigned char MIOS_MF_PWM_DutyCycleUpGet(void) __wparam;
extern void MIOS_MF_PWM_DutyCycleUpSet(unsigned char cali_up) __wparam;
extern unsigned char MIOS_MF_DeadbandGet(void) __wparam;
extern void MIOS_MF_DeadbandSet(unsigned char deadband) __wparam;
extern void MIOS_MF_Disable(void) __wparam;
extern void MIOS_MF_Enable(void) __wparam;
extern void MIOS_MF_FaderMove(unsigned char fader, unsigned int pos) __wparam;
extern void MIOS_MF_SuspendDisable(unsigned char fader) __wparam;
extern void MIOS_MF_SuspendEnable(unsigned char fader) __wparam;
extern unsigned char MIOS_MF_SuspendGet(unsigned char fader) __wparam;
extern unsigned char MIOS_MF_PWM_PeriodGet(void) __wparam;
extern void MIOS_MF_PWM_PeriodSet(unsigned char speed) __wparam;
extern void MIOS_MF_TouchDetectionReset(unsigned char fader) __wparam;

extern unsigned char MIOS_DIN_PinGet(unsigned char pin) __wparam;
extern unsigned char MIOS_DIN_SRGet(unsigned char sr) __wparam;

extern unsigned char MIOS_DOUT_PinGet(unsigned char pin) __wparam;
extern void MIOS_DOUT_PinSet(unsigned char pin, unsigned char pin_value) __wparam;
extern void MIOS_DOUT_PinSet0(unsigned char pin) __wparam;
extern void MIOS_DOUT_PinSet1(unsigned char pin) __wparam;
extern unsigned char MIOS_DOUT_SRGet(unsigned char sr) __wparam;
extern void MIOS_DOUT_SRSet(unsigned char sr, unsigned char sr_value) __wparam;

extern void MIOS_ENC_Abs7bitAdd(unsigned char enc, char add) __wparam;
extern unsigned char MIOS_ENC_Abs7bitGet(unsigned char enc) __wparam;
extern void MIOS_ENC_Abs7bitSet(unsigned char enc, unsigned char value) __wparam;
extern unsigned char MIOS_ENC_NumberGet(void) __wparam;
extern unsigned char MIOS_ENC_SpeedGet(unsigned char enc) __wparam;
extern void MIOS_ENC_SpeedSet(unsigned char enc, unsigned char mode, unsigned char parameter) __wparam;

extern unsigned char MIOS_SRIO_NumberGet(void) __wparam;
extern void MIOS_SRIO_NumberSet(unsigned char number_sr) __wparam;
extern unsigned char MIOS_SRIO_TS_SensitivityGet(void) __wparam;
extern void MIOS_SRIO_TS_SensitivitySet(unsigned char update_frq) __wparam;
extern unsigned char MIOS_SRIO_UpdateFrqGet(void) __wparam;
extern void MIOS_SRIO_UpdateFrqSet(unsigned char update_frq) __wparam;
extern unsigned char MIOS_SRIO_DebounceGet(void) __wparam;
extern void MIOS_SRIO_DebounceSet(unsigned char debounce_value) __wparam;

extern void MIOS_LCD_Clear(void) __wparam;
extern void MIOS_LCD_Cmd(unsigned char cmd) __wparam;
extern unsigned char MIOS_LCD_CursorGet(void) __wparam;
extern void MIOS_LCD_CursorSet(unsigned char pos) __wparam;
extern void MIOS_LCD_Data(unsigned char data) __wparam;
extern void MIOS_LCD_Init(void) __wparam;
extern void MIOS_LCD_PrintBCD1(unsigned char v) __wparam;
extern void MIOS_LCD_PrintBCD2(unsigned char v) __wparam;
extern void MIOS_LCD_PrintBCD3(unsigned char v) __wparam;
extern void MIOS_LCD_PrintBCD4(unsigned int v) __wparam;
extern void MIOS_LCD_PrintBCD5(unsigned int v) __wparam;
extern void MIOS_LCD_PrintChar(unsigned char c) __wparam;
extern void MIOS_LCD_PrintHex1(unsigned char h) __wparam;
extern void MIOS_LCD_PrintHex2(unsigned char h) __wparam;
extern void MIOS_LCD_PrintMessage(code char *str) __wparam;
extern void MIOS_LCD_PrintPreconfString(unsigned char len, code char *str) __wparam;
extern void MIOS_LCD_PrintString(code char *str) __wparam;
extern void MIOS_LCD_TypeAutoSet(void) __wparam;
extern unsigned char MIOS_LCD_TypeGet(void) __wparam;
extern void MIOS_LCD_TypeSet(unsigned char type, unsigned char par1, unsigned char par2) __wparam;
extern void MIOS_LCD_YAddressSet(unsigned char line0, unsigned char line1, unsigned char line2, unsigned char line3) __wparam;
extern unsigned char MIOS_LCD_YAddressGet(void) __wparam;
extern void MIOS_LCD_MessageStart(unsigned char delay) __wparam;
extern void MIOS_LCD_MessageStop(void) __wparam;

extern void MIOS_LCD_PrintCString(char *str) __wparam; // not supported by MIOS itself, but by the wrapper

extern void MIOS_CLCD_SpecialCharInit(unsigned char num, code char *c_table) __wparam;
extern void MIOS_CLCD_SpecialCharsInit(code char *c_table) __wparam;

extern void MIOS_GLCD_FontInit(code char *font) __wparam;
extern unsigned char MIOS_GLCD_GCursorGet(void) __wparam;
extern void MIOS_GLCD_GCursorSet(unsigned char x, unsigned char y) __wparam;

extern unsigned char MIOS_EEPROM_Read(unsigned char addr) __wparam;
extern unsigned char MIOS_EEPROM_Write(unsigned char addr, unsigned char value) __wparam;
extern unsigned char MIOS_EEPROM_ReadPage(unsigned char addr, char *buffer) __wparam;
extern unsigned char MIOS_EEPROM_WritePage(unsigned char addr, char *buffer) __wparam;

extern unsigned char MIOS_FLASH_Read(code char *addr, char *buffer) __wparam;
extern unsigned char MIOS_FLASH_Write(code char *addr, char *buffer) __wparam;

extern unsigned char MIOS_BANKSTICK_Read(unsigned int addr) __wparam;
extern unsigned char MIOS_BANKSTICK_Write(unsigned int addr, unsigned char value) __wparam;
extern void MIOS_BANKSTICK_CtrlSet(unsigned char ctrl) __wparam;
extern unsigned char MIOS_BANKSTICK_CtrlGet(void) __wparam;
extern unsigned char MIOS_BANKSTICK_WritePage(unsigned int addr, char *buffer) __wparam;
extern unsigned char MIOS_BANKSTICK_ReadPage(unsigned int addr, char *buffer) __wparam;

extern void MIOS_TIMER_Init(unsigned char mode, unsigned int period) __wparam;
extern void MIOS_TIMER_Start(void) __wparam;
extern void MIOS_TIMER_Stop(void) __wparam;
extern void MIOS_TIMER_ReInit(unsigned char mode, unsigned int period) __wparam;

extern unsigned char MIOS_HLP_GetBitANDMask(unsigned char value) __wparam;
extern unsigned char MIOS_HLP_GetBitORMask(unsigned char value) __wparam;
extern unsigned char MIOS_HLP_16bitAddSaturate(unsigned char add_value, unsigned int *ptr, unsigned int max_value) __wparam;
extern unsigned char MIOS_HLP_Dec2BCD(unsigned int value) __wparam;

extern void MIOS_Delay(unsigned char delay) __wparam;
extern unsigned char MIOS_GPCounterGet(void) __wparam;
extern void MIOS_Reset(void) __wparam;
extern void MIOS_SystemResume(void) __wparam;
extern void MIOS_SystemSuspend(void) __wparam;
extern void MIOS_UserResume(void) __wparam;
extern void MIOS_UserSuspend(void) __wparam;

extern void MIOS_DIN_PinAutoRepeatEnable(unsigned char pin) __wparam;
extern void MIOS_DIN_PinAutoRepeatDisable(unsigned char pin) __wparam;
extern unsigned char MIOS_DIN_PinAutoRepeatGet(unsigned char pin) __wparam;

extern void MIOS_AIN_DynamicPrioSet(unsigned char enable) __wparam;
extern unsigned char MIOS_AIN_DynamicPrioGet(void) __wparam;
extern unsigned char MIOS_AIN_LastPinsGet(void) __wparam;

extern void MIOS_IIC_Start(void) __wparam;
extern void MIOS_IIC_Stop(void) __wparam;
extern unsigned char MIOS_IIC_ByteSend(unsigned char b) __wparam;
extern unsigned char MIOS_IIC_ByteReceive(void) __wparam;
extern void MIOS_IIC_AckSend(void) __wparam;
extern void MIOS_IIC_NakSend(void) __wparam;
extern void MIOS_IIC_CtrlSet(unsigned char ctrl) __wparam;
extern unsigned char MIOS_IIC_CtrlGet(void) __wparam;

#endif /* _CMIOS_H */
