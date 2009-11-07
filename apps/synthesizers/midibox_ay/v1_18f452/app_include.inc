;; --------------

;; app_include.inc
;; ========================================================================

;; ---[variables used by application ]---
#include "app_defines.h"

; ==========================================================================

;; ---[configuration table for MIDI processor and rotary encoders ]---
#include "mios_tables.inc"

;; ---[Custom LCD driver ]---
#include "app_lcd.inc"

;; ---[8x8 Scan Matrix functions (C64 KEYBOARD DRIVER BY TK)]---
#include "sm_simple_c64keyb.inc"

;; --------[start screen]--------------
#include "start_screen.inc"


;; ----[NOTE ON to AY TONE FREQ Hz CONVERSATION]----------------------------
#include "note_to_hz.inc"

;; ----[APPLICATION VARIABLES, ETC. INI]------------------------------------------
#include "midibox_ay_ini.inc"

;; ---[simplifies the handling with MIDI events ]---
;; NOTE: NOT USED YET
;; #include "midi_evnt.inc"

;; ----[INTERCONNECTION TO THE AY CHIP]----
#include "ay_sr.inc"
#include "format_bankstick.inc"

;; ----[AY LCD TEXT DEFINITION]---------------------------
#include "lcd_text.inc"

;; ----[PATCH]--------
#include "patch_write.inc"
#include "patch_load.inc"
#include "patch_name_writing.inc"

;; ----[C64 KEYBOARD DECODING]----------------------------
#include "keyb_decoding.inc"
#include "get_key.inc"



;; ----[AY MAIN MENU CODE]----------------------------
#include "main_menu_functions.inc"
#include "main_menu_00_overview.inc"
#include "main_menu_01_patch.inc"
#include "main_menu_02_detune.inc"
#include "main_menu_03_mixer.inc"
#include "main_menu_04_amplitude.inc"
#include "main_menu_05_envelope.inc"
#include "main_menu_06_midi_ch.inc"
#include "main_menu_07_velocity_conf.inc"


;; ----[AY SUB MENU CODE]----------------------------
#include "sub_menu_functions.inc"
#include "sub_menu_key_up.inc"
#include "sub_menu_key_down.inc"
#include "sub_menu_key_plus.inc"
#include "sub_menu_key_plus_menu_0.inc"
#include "sub_menu_key_plus_menu_1.inc"
#include "sub_menu_key_plus_menu_2.inc"
#include "sub_menu_key_plus_menu_3.inc"
#include "sub_menu_key_plus_menu_4.inc"
#include "sub_menu_key_plus_menu_5.inc"
#include "sub_menu_key_plus_menu_6.inc"
#include "sub_menu_key_plus_menu_7.inc"
#include "sub_menu_key_plus_menu_8.inc"

#include "sub_menu_key_minus.inc"
#include "sub_menu_key_minus_menu_0.inc"
#include "sub_menu_key_minus_menu_1.inc"
#include "sub_menu_key_minus_menu_2.inc"
#include "sub_menu_key_minus_menu_3.inc"
#include "sub_menu_key_minus_menu_4.inc"
#include "sub_menu_key_minus_menu_5.inc"
#include "sub_menu_key_minus_menu_6.inc"
#include "sub_menu_key_minus_menu_7.inc"
#include "sub_menu_key_minus_menu_8.inc"

#include "sub_menu_key_a.inc"
#include "sub_menu_key_b.inc"
#include "sub_menu_key_c.inc"



;; ----[USER Tick]----------------------------
#include "user_tick_lcd_redraw.inc"
#include "user_tick_key_repeat.inc"


;; ----[MIDI FUNCTIONS - NOTE ON, NOTE OFF, etc.]----------------------------
#include "midi_event_jump_table.inc"			;; MIDI EVENT SWITCH BOARD
#include "note_off.inc"					;; Note OFF Event HAndling
#include "note_on.inc"					;; Note On Switch Board
#include "midi_controller.inc" 				;; MIDI CC Handling
#include "midi_event_pitch_wheel.inc"			;; Pitch Wheel Handling
#include "midi_evnt_handling.inc"			;; NOT USED MIDI EVENT HERE
#include "note_on_pipeline_a.inc"			;; Note ON Handling
#include "note_on_pipeline_b.inc"			;; Note ON Handling
#include "note_on_pipeline_c.inc"			;; Note ON Handling
#include "note_on_write_ay_reg7.inc"			;; Note ON Handling

;; ----[AY SUB MENU CODE - Fx Menu]--------
#include "main_menu_08_fx.inc"

;; ---[Fx Timer Polling]--------------
#include "fx_timer.inc"

;; ---[Fx Pipeline]-------------------
#include "fx_pipeline.inc"

;; ---[DOUT (LED)]--------------------
#include "dout_calls.inc"

