;
; BankStick Data Content (see main.asm for further details)
;
; ==========================================================================
;
; Copyright (C) 2003  Thorsten Klose (tk@midibox.org)
; 
; ==========================================================================
; 
; This file is part of an MIOS example application
;
; This application is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This application is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this application; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
; ==========================================================================

	;; BankStick address range:
	;; 32k BankStick (24LC256): 0x400000-0x407fff
	;; 64k BankStick (24LC512): 0x400000-0x40ffff
	
	org	0x400000

	;; 16 bytes of data
	db	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
	db	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f

	;; a string
	db	"This string is stored in BankStick!", 0x00

	;; some words (16 bit)
	dw	0xdead, 0xbeef
	dw	0xaffe, 0xdead

	org	0x400400
	db	0x12
