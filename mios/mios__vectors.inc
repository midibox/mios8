; $Id$
;
; MIOS vectortable - contains user hooks to the OS side
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; never change the origin or the order of entries!
	org    0x2c00
_MIOS_MIDI_BeginStream                  goto	MIOS_MIDI_BeginStream
_MIOS_MIDI_DeviceIDAutoSet              goto	MIOS_MIDI_DeviceIDAutoSet
_MIOS_MIDI_DeviceIDGet                  goto	MIOS_MIDI_DeviceIDGet
_MIOS_MIDI_DeviceIDSet                  goto	MIOS_MIDI_DeviceIDSet
_MIOS_MIDI_EndStream                    goto	MIOS_MIDI_EndStream
_MIOS_MIDI_Init                         goto	MIOS_MIDI_Init
_MIOS_MIDI_InterfaceAutoSet             goto	MIOS_MIDI_InterfaceAutoSet
_MIOS_MIDI_InterfaceGet                 goto	MIOS_MIDI_InterfaceGet
_MIOS_MIDI_InterfaceSet                 goto	MIOS_MIDI_InterfaceSet
_MIOS_MIDI_MergerGet                    goto	MIOS_MIDI_MergerGet
_MIOS_MIDI_MergerSet                    goto	MIOS_MIDI_MergerSet
_MIOS_MIDI_RxBufferFree                 goto	MIOS_MIDI_RxBufferFree
_MIOS_MIDI_RxBufferGet                  goto	MIOS_MIDI_RxBufferGet
_MIOS_MIDI_RxBufferPut                  goto	MIOS_MIDI_RxBufferPut
_MIOS_MIDI_RxBufferUsed                 goto	MIOS_MIDI_RxBufferUsed
_MIOS_MIDI_TxBufferFlush                goto	MIOS_MIDI_TxBufferFlush
_MIOS_MIDI_TxBufferFree                 goto	MIOS_MIDI_TxBufferFree
_MIOS_MIDI_TxBufferGet                  goto	MIOS_MIDI_TxBufferGet
_MIOS_MIDI_TxBufferPut                  goto	MIOS_MIDI_TxBufferPut
_MIOS_MIDI_TxBufferUsed                 goto	MIOS_MIDI_TxBufferUsed
_MIOS_MPROC_MergerDisable               goto	MIOS_MPROC_MergerDisable
_MIOS_MPROC_MergerEnable                goto	MIOS_MPROC_MergerEnable
_MIOS_MPROC_MergerGet                   goto	MIOS_MPROC_MergerGet

#if MIOS_ENABLE_AIN
_MIOS_AIN_DeadbandGet                   goto	MIOS_AIN_DeadbandGet
_MIOS_AIN_DeadbandSet                   goto	MIOS_AIN_DeadbandSet
_MIOS_AIN_Muxed                         goto	MIOS_AIN_Muxed
_MIOS_AIN_NumberGet                     goto	MIOS_AIN_NumberGet
_MIOS_AIN_NumberSet                     goto	MIOS_AIN_NumberSet
_MIOS_AIN_Pin7bitGet                    goto	MIOS_AIN_Pin7bitGet
_MIOS_AIN_PinGet                        goto	MIOS_AIN_PinGet
_MIOS_AIN_PinLSBGet                     goto	MIOS_AIN_PinLSBGet
_MIOS_AIN_PinMSBGet                     goto	MIOS_AIN_PinMSBGet
_MIOS_AIN_UnMuxed                       goto	MIOS_AIN_UnMuxed
#else
_MIOS_AIN_DeadbandGet                   goto	MIOS_DISABLED_FUNCTION
_MIOS_AIN_DeadbandSet                   goto	MIOS_DISABLED_FUNCTION
_MIOS_AIN_Muxed                         goto	MIOS_DISABLED_FUNCTION
_MIOS_AIN_NumberGet                     goto	MIOS_DISABLED_FUNCTION
_MIOS_AIN_NumberSet                     goto	MIOS_DISABLED_FUNCTION
_MIOS_AIN_Pin7bitGet                    goto	MIOS_DISABLED_FUNCTION
_MIOS_AIN_PinGet                        goto	MIOS_DISABLED_FUNCTION
_MIOS_AIN_PinLSBGet                     goto	MIOS_DISABLED_FUNCTION
_MIOS_AIN_PinMSBGet                     goto	MIOS_DISABLED_FUNCTION
_MIOS_AIN_UnMuxed                       goto	MIOS_DISABLED_FUNCTION
#endif

#if MIOS_ENABLE_AIN && MIOS_ENABLE_MF
_MIOS_MF_PWM_DutyCycleDownGet           goto	MIOS_MF_PWM_DutyCycleDownGet
_MIOS_MF_PWM_DutyCycleDownSet           goto	MIOS_MF_PWM_DutyCycleDownSet
_MIOS_MF_PWM_DutyCycleUpGet             goto	MIOS_MF_PWM_DutyCycleUpGet
_MIOS_MF_PWM_DutyCycleUpSet             goto	MIOS_MF_PWM_DutyCycleUpSet
_MIOS_MF_DeadbandGet                    goto	MIOS_MF_DeadbandGet
_MIOS_MF_DeadbandSet                    goto	MIOS_MF_DeadbandSet
_MIOS_MF_Disable                        goto	MIOS_MF_Disable
_MIOS_MF_Enable                         goto	MIOS_MF_Enable
_MIOS_MF_FaderMove                      goto	MIOS_MF_FaderMove
_MIOS_MF_SuspendDisable                 goto	MIOS_MF_SuspendDisable
_MIOS_MF_SuspendEnable                  goto	MIOS_MF_SuspendEnable
_MIOS_MF_SuspendGet                     goto	MIOS_MF_SuspendGet
#else
_MIOS_MF_PWM_DutyCycleDownGet           goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_PWM_DutyCycleDownSet           goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_PWM_DutyCycleUpGet             goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_PWM_DutyCycleUpSet             goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_DeadbandGet                    goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_DeadbandSet                    goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_Disable                        goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_Enable                         goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_FaderMove                      goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_SuspendDisable                 goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_SuspendEnable                  goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_SuspendGet                     goto	MIOS_DISABLED_FUNCTION
#endif

_MIOS_DIN_PinGet                        goto	MIOS_DIN_PinGet
_MIOS_DIN_SRGet                         goto	MIOS_DIN_SRGet
_MIOS_DOUT_PinGet                       goto	MIOS_DOUT_PinGet
_MIOS_DOUT_PinSet                       goto	MIOS_DOUT_PinSet
_MIOS_DOUT_PinSet0                      goto	MIOS_DOUT_PinSet0
_MIOS_DOUT_PinSet1                      goto	MIOS_DOUT_PinSet1
_MIOS_DOUT_SRGet                        goto	MIOS_DOUT_SRGet
_MIOS_DOUT_SRSet                        goto	MIOS_DOUT_SRSet

#if MIOS_ENABLE_ENC
_MIOS_ENC_Abs7bitAdd                    goto	MIOS_ENC_Abs7bitAdd
_MIOS_ENC_Abs7bitGet                    goto	MIOS_ENC_Abs7bitGet
_MIOS_ENC_Abs7bitSet                    goto	MIOS_ENC_Abs7bitSet
_MIOS_ENC_NumberGet                     goto	MIOS_ENC_NumberGet
_MIOS_ENC_SpeedGet                      goto	MIOS_ENC_SpeedGet
_MIOS_ENC_SpeedSet                      goto	MIOS_ENC_SpeedSet
#else
_MIOS_ENC_Abs7bitAdd                    goto	MIOS_DISABLED_FUNCTION
_MIOS_ENC_Abs7bitGet                    goto	MIOS_DISABLED_FUNCTION
_MIOS_ENC_Abs7bitSet                    goto	MIOS_DISABLED_FUNCTION
_MIOS_ENC_NumberGet                     goto	MIOS_DISABLED_FUNCTION
_MIOS_ENC_SpeedGet                      goto	MIOS_DISABLED_FUNCTION
_MIOS_ENC_SpeedSet                      goto	MIOS_DISABLED_FUNCTION
#endif

_MIOS_SRIO_NumberGet                    goto	MIOS_SRIO_NumberGet
_MIOS_SRIO_NumberSet                    goto	MIOS_SRIO_NumberSet
_MIOS_SRIO_TS_SensitivityGet            goto	MIOS_SRIO_TS_SensitivityGet
_MIOS_SRIO_TS_SensitivitySet            goto	MIOS_SRIO_TS_SensitivitySet
_MIOS_SRIO_UpdateFrqGet                 goto	MIOS_SRIO_UpdateFrqGet
_MIOS_SRIO_UpdateFrqSet                 goto	MIOS_SRIO_UpdateFrqSet
_MIOS_LCD_Clear                         goto	MIOS_LCD_Clear
_MIOS_LCD_Cmd                           goto	MIOS_LCD_Cmd
_MIOS_LCD_CursorGet                     goto	MIOS_LCD_CursorGet
_MIOS_LCD_CursorSet                     goto	MIOS_LCD_CursorSet
_MIOS_LCD_Data                          goto	MIOS_LCD_Data
_MIOS_LCD_Init                          goto	MIOS_LCD_Init
_MIOS_LCD_PrintBCD1                     goto	MIOS_LCD_PrintBCD1
_MIOS_LCD_PrintBCD2                     goto	MIOS_LCD_PrintBCD2
_MIOS_LCD_PrintBCD3                     goto	MIOS_LCD_PrintBCD3
_MIOS_LCD_PrintChar                     goto	MIOS_LCD_PrintChar
_MIOS_LCD_PrintHex1                     goto	MIOS_LCD_PrintHex1
_MIOS_LCD_PrintHex2                     goto	MIOS_LCD_PrintHex2
_MIOS_LCD_PrintMessage                  goto	MIOS_LCD_PrintMessage
_MIOS_LCD_PrintPreconfString            goto	MIOS_LCD_PrintPreconfString
_MIOS_LCD_PrintString                   goto	MIOS_LCD_PrintString
_MIOS_LCD_TypeAutoSet                   goto	MIOS_LCD_TypeAutoSet
_MIOS_LCD_TypeGet                       goto	MIOS_LCD_TypeGet
_MIOS_LCD_TypeSet                       goto	MIOS_LCD_TypeSet
_MIOS_LCD_YAddressGet                   goto	MIOS_LCD_YAddressGet
_MIOS_LCD_YAddressSet                   goto	MIOS_LCD_YAddressSet
_MIOS_GLCD_FontInit                     goto	MIOS_GLCD_FontInit
_MIOS_GLCD_GCursorGet                   goto	MIOS_GLCD_GCursorGet
_MIOS_GLCD_GCursorSet                   goto	MIOS_GLCD_GCursorSet
_MIOS_EEPROM_Read                       goto	MIOS_EEPROM_Read
_MIOS_EEPROM_Write                      goto	MIOS_EEPROM_Write
_MIOS_FLASH_Read                        goto	MIOS_FLASH_Read
_MIOS_FLASH_Write                       goto	MIOS_FLASH_Write
_MIOS_BANKSTICK_Read                    goto	MIOS_BANKSTICK_Read
_MIOS_BANKSTICK_Write                   goto	MIOS_BANKSTICK_Write
_MIOS_TIMER_Init                        goto	MIOS_TIMER_Init
_MIOS_TIMER_Start                       goto	MIOS_TIMER_Start
_MIOS_TIMER_Stop                        goto	MIOS_TIMER_Stop
_MIOS_HLP_AddressFromTable              goto	MIOS_HLP_AddressFromTable
_MIOS_HLP_GetBitANDMask                 goto	MIOS_HLP_GetBitANDMask
_MIOS_HLP_GetBitORMask                  goto	MIOS_HLP_GetBitORMask
_MIOS_HLP_GetIndex_2bytes               goto	MIOS_HLP_GetIndex_2bytes
_MIOS_HLP_GetIndex_4bytes               goto	MIOS_HLP_GetIndex_4bytes
_MIOS_HLP_IndirectJump                  goto	MIOS_HLP_IndirectJump
_MIOS_Delay                             goto	MIOS_Delay
_MIOS_GPCounterGet                      goto	MIOS_GPCounterGet
_MIOS_Reset                             goto	MIOS_Reset
_MIOS_SystemResume                      goto	MIOS_SystemResume
_MIOS_SystemSuspend                     goto	MIOS_SystemSuspend
_MIOS_UserResume                        goto	MIOS_UserResume
_MIOS_UserSuspend                       goto	MIOS_UserSuspend
_MIOS_HLP_16bitAddSaturate		goto	MIOS_HLP_16bitAddSaturate
_MIOS_DIN_PinAutoRepeatEnable		goto	MIOS_DIN_PinAutoRepeatEnable
_MIOS_DIN_PinAutoRepeatDisable		goto	MIOS_DIN_PinAutoRepeatDisable
_MIOS_DIN_PinAutoRepeatGet		goto	MIOS_DIN_PinAutoRepeatGet
_MIOS_CLCD_SpecialCharInit		goto	MIOS_CLCD_SpecialCharInit
_MIOS_CLCD_SpecialCharsInit		goto	MIOS_CLCD_SpecialCharsInit

#if MIOS_ENABLE_AIN
_MIOS_AIN_DynamicPrioSet		goto	MIOS_AIN_DynamicPrioSet
_MIOS_AIN_DynamicPrioGet		goto	MIOS_AIN_DynamicPrioGet
_MIOS_AIN_LastPinsGet			goto	MIOS_AIN_LastPinsGet
#else
_MIOS_AIN_DynamicPrioSet		goto	MIOS_DISABLED_FUNCTION
_MIOS_AIN_DynamicPrioGet		goto	MIOS_DISABLED_FUNCTION
_MIOS_AIN_LastPinsGet			goto	MIOS_DISABLED_FUNCTION
#endif

_MIOS_BANKSTICK_CtrlSet			goto	MIOS_BANKSTICK_CtrlSet
_MIOS_BANKSTICK_CtrlGet			goto	MIOS_BANKSTICK_CtrlGet
_MIOS_BANKSTICK_WritePage		goto	MIOS_BANKSTICK_WritePage
_MIOS_HLP_Dec2BCD			goto	MIOS_HLP_Dec2BCD
_MIOS_LCD_PrintBCD4			goto	MIOS_LCD_PrintBCD4
_MIOS_LCD_PrintBCD5                     goto	MIOS_LCD_PrintBCD5
_MIOS_LCD_MessageStart			goto	MIOS_LCD_MessageStart
_MIOS_LCD_MessageStop			goto	MIOS_LCD_MessageStop

#if MIOS_ENABLE_AIN && MIOS_ENABLE_MF
_MIOS_MF_PWM_PeriodGet                  goto	MIOS_MF_PWM_PeriodGet
_MIOS_MF_PWM_PeriodSet                  goto	MIOS_MF_PWM_PeriodSet
#else
_MIOS_MF_PWM_PeriodGet                  goto	MIOS_DISABLED_FUNCTION
_MIOS_MF_PWM_PeriodSet                  goto	MIOS_DISABLED_FUNCTION
#endif

_MIOS_IIC_Start				goto	MIOS_IIC_Start
_MIOS_IIC_Stop				goto	MIOS_IIC_Stop
_MIOS_IIC_ByteSend			goto	MIOS_IIC_ByteSend
_MIOS_IIC_ByteReceive			goto	MIOS_IIC_ByteReceive
_MIOS_IIC_AckSend			goto	MIOS_IIC_AckSend
_MIOS_IIC_NakSend			goto	MIOS_IIC_NakSend
_MIOS_IIC_CtrlSet			goto	MIOS_IIC_CtrlSet
_MIOS_IIC_CtrlGet			goto	MIOS_IIC_CtrlGet
_MIOS_SRIO_DebounceSet			goto	MIOS_SRIO_DebounceSet
_MIOS_SRIO_DebounceGet			goto	MIOS_SRIO_DebounceGet

#if MIOS_ENABLE_AIN && MIOS_ENABLE_MF
_MIOS_MF_TouchDetectionReset		goto	MIOS_MF_TouchDetectionReset
#else
_MIOS_MF_TouchDetectionReset		goto	MIOS_DISABLED_FUNCTION
#endif

_MIOS_BANKSTICK_ReadPage		goto	MIOS_BANKSTICK_ReadPage
_MIOS_EEPROM_ReadPage			goto	MIOS_EEPROM_ReadPage
_MIOS_EEPROM_WritePage			goto	MIOS_EEPROM_WritePage
_MIOS_TIMER_ReInit			goto	MIOS_TIMER_ReInit
MIOS_RESERVED_144                       return
					nop
MIOS_RESERVED_145                       return
					nop
MIOS_RESERVED_146                       return
					nop
MIOS_RESERVED_147                       return
					nop
MIOS_RESERVED_148                       return
					nop
MIOS_RESERVED_149                       return
					nop
MIOS_RESERVED_150                       return
					nop
MIOS_RESERVED_151                       return
					nop
MIOS_RESERVED_152                       return
					nop
MIOS_RESERVED_153                       return
					nop
MIOS_RESERVED_154                       return
					nop
MIOS_RESERVED_155                       return
					nop
MIOS_RESERVED_156                       return
					nop
MIOS_RESERVED_157                       return
					nop
MIOS_RESERVED_158                       return
					nop
MIOS_RESERVED_159                       return
					nop
MIOS_RESERVED_160                       return
					nop
MIOS_RESERVED_161                       return
					nop
MIOS_RESERVED_162                       return
					nop
MIOS_RESERVED_163                       return
					nop
MIOS_RESERVED_164                       return
					nop
MIOS_RESERVED_165                       return
					nop
MIOS_RESERVED_166                       return
					nop
MIOS_RESERVED_167                       return
					nop
MIOS_RESERVED_168                       return
					nop
MIOS_RESERVED_169                       return
					nop
MIOS_RESERVED_170                       return
					nop
MIOS_RESERVED_171                       return
					nop
MIOS_RESERVED_172                       return
					nop
MIOS_RESERVED_173                       return
					nop
MIOS_RESERVED_174                       return
					nop
MIOS_RESERVED_175                       return
					nop
MIOS_RESERVED_176                       return
					nop
MIOS_RESERVED_177                       return
					nop
MIOS_RESERVED_178                       return
					nop
MIOS_RESERVED_179                       return
					nop
MIOS_RESERVED_180                       return
					nop
MIOS_RESERVED_181                       return
					nop
MIOS_RESERVED_182                       return
					nop
MIOS_RESERVED_183                       return
					nop
MIOS_RESERVED_184                       return
					nop
MIOS_RESERVED_185                       return
					nop
MIOS_RESERVED_186                       return
					nop
MIOS_RESERVED_187                       return
					nop
MIOS_RESERVED_188                       return
					nop
MIOS_RESERVED_189                       return
					nop
MIOS_RESERVED_190                       return
					nop
MIOS_RESERVED_191                       return
					nop
MIOS_RESERVED_192                       return
					nop
MIOS_RESERVED_193                       return
					nop
MIOS_RESERVED_194                       return
					nop
MIOS_RESERVED_195                       return
					nop
MIOS_RESERVED_196                       return
					nop
MIOS_RESERVED_197                       return
					nop
MIOS_RESERVED_198                       return
					nop
MIOS_RESERVED_199                       return
					nop
MIOS_RESERVED_200                       return
					nop
MIOS_RESERVED_201                       return
					nop
MIOS_RESERVED_202                       return
					nop
MIOS_RESERVED_203                       return
					nop
MIOS_RESERVED_204                       return
					nop
MIOS_RESERVED_205                       return
					nop
MIOS_RESERVED_206                       return
					nop
MIOS_RESERVED_207                       return
					nop
MIOS_RESERVED_208                       return
					nop
MIOS_RESERVED_209                       return
					nop
MIOS_RESERVED_210                       return
					nop
MIOS_RESERVED_211                       return
					nop
MIOS_RESERVED_212                       return
					nop
MIOS_RESERVED_213                       return
					nop
MIOS_RESERVED_214                       return
					nop
MIOS_RESERVED_215                       return
					nop
MIOS_RESERVED_216                       return
					nop
MIOS_RESERVED_217                       return
					nop
MIOS_RESERVED_218                       return
					nop
MIOS_RESERVED_219                       return
					nop
MIOS_RESERVED_220                       return
					nop
MIOS_RESERVED_221                       return
					nop
MIOS_RESERVED_222                       return
					nop
MIOS_RESERVED_223                       return
					nop
MIOS_RESERVED_224                       return
					nop
MIOS_RESERVED_225                       return
					nop
MIOS_RESERVED_226                       return
					nop
MIOS_RESERVED_227                       return
					nop
MIOS_RESERVED_228                       return
					nop
MIOS_RESERVED_229                       return
					nop
MIOS_RESERVED_230                       return
					nop
MIOS_RESERVED_231                       return
					nop
MIOS_RESERVED_232                       return
					nop
MIOS_RESERVED_233                       return
					nop
MIOS_RESERVED_234                       return
					nop
MIOS_RESERVED_235                       return
					nop
MIOS_RESERVED_236                       return
					nop
MIOS_RESERVED_237                       return
					nop
MIOS_RESERVED_238                       return
					nop
MIOS_RESERVED_239                       return
					nop
MIOS_RESERVED_240                       return
					nop
MIOS_RESERVED_241                       return
					nop
MIOS_RESERVED_242                       return
					nop
MIOS_RESERVED_243                       return
					nop
MIOS_RESERVED_244                       return
					nop
MIOS_RESERVED_245                       return
					nop
MIOS_RESERVED_246                       return
					nop
MIOS_RESERVED_247                       return
					nop
MIOS_RESERVED_248                       return
					nop
MIOS_RESERVED_249                       return
					nop
MIOS_RESERVED_250                       return
					nop
MIOS_RESERVED_251                       return
					nop
MIOS_RESERVED_252                       return
					nop
MIOS_RESERVED_253                       return
					nop
MIOS_RESERVED_254                       return
					nop
MIOS_DISABLED_FUNCTION                  return
					nop
