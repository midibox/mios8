; $Id$
;
; Here the labels for all banks and parameters are defined
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


BANK_ENTRY MACRO label
	db	label
	ENDM

	;; here the labels for all banks are defined
	;; if VALUES_PER_PAGE is 5 (for 2x20 displays), we have 51 banks, 
	;; if VALUES_PER_PAGE is 4 (for 2x16 displays), we have 64 banks,
	;; the VALUES_PER_PAGE setting is defined in app_defines.h
TC_BANK_LABELS
	;; NOTE: every entry must consist of 16 characters, so that the
	;; routine in tc_display.inc can calulcate the pointer to the
	;; string, depending on TC_BANK (pointer = TC_BANK_LABELS + 16*TC_BANK)

	;;              |<-------------->|
	BANK_ENTRY	"Demo Bank       "
	BANK_ENTRY	"Bank  2         "
	BANK_ENTRY	"Bank  3         "
	BANK_ENTRY	"Bank  4         "
	BANK_ENTRY	"Bank  5         "
	BANK_ENTRY	"Bank  6         "
	BANK_ENTRY	"Bank  7         "
	BANK_ENTRY	"Bank  8         "
	BANK_ENTRY	"Bank  9         "
	BANK_ENTRY	"Bank 10         "
	BANK_ENTRY	"Bank 11         "
	BANK_ENTRY	"Bank 12         "
	BANK_ENTRY	"Bank 13         "
	BANK_ENTRY	"Bank 14         "
	BANK_ENTRY	"Bank 15         "
	BANK_ENTRY	"Bank 16         "
	BANK_ENTRY	"Bank 17         "
	BANK_ENTRY	"Bank 18         "
	BANK_ENTRY	"Bank 19         "
	BANK_ENTRY	"Bank 20         "
	BANK_ENTRY	"Bank 21         "
	BANK_ENTRY	"Bank 22         "
	BANK_ENTRY	"Bank 23         "
	BANK_ENTRY	"Bank 24         "
	BANK_ENTRY	"Bank 25         "
	BANK_ENTRY	"Bank 26         "
	BANK_ENTRY	"Bank 27         "
	BANK_ENTRY	"Bank 28         "
	BANK_ENTRY	"Bank 29         "
	BANK_ENTRY	"Bank 30         "
	BANK_ENTRY	"Bank 31         "
	BANK_ENTRY	"Bank 32         "
	BANK_ENTRY	"Bank 33         "
	BANK_ENTRY	"Bank 34         "
	BANK_ENTRY	"Bank 35         "
	BANK_ENTRY	"Bank 36         "
	BANK_ENTRY	"Bank 37         "
	BANK_ENTRY	"Bank 38         "
	BANK_ENTRY	"Bank 39         "
	BANK_ENTRY	"Bank 40         "
	BANK_ENTRY	"Bank 41         "
	BANK_ENTRY	"Bank 42         "
	BANK_ENTRY	"Bank 43         "
	BANK_ENTRY	"Bank 44         "
	BANK_ENTRY	"Bank 45         "
	BANK_ENTRY	"Bank 46         "
	BANK_ENTRY	"Bank 47         "
	BANK_ENTRY	"Bank 48         "
	BANK_ENTRY	"Bank 49         "
	BANK_ENTRY	"Bank 50         "
	BANK_ENTRY	"Bank 51         "


PARA_ENTRY MACRO short, long, mode
	db	short, long, mode
	ENDM

	;; here the labels for all parameters are defined
	;; if VALUES_PER_PAGE is 5, we have 255 parameters (in 51 banks...)
	;; if VALUES_PER_PAGE is 4, we have 256 parameters (in 64 banks...)
	;; the VALUES_PER_PAGE setting is defined in app_defines.h
TC_PARA_LABELS
	;; NOTE: we are using the following structure for every entry:
	;; 4 characters for the short label
	;; 13 characters for the long label
	;; one mode byte which switches between different number formats:
	;; PARA_VALUE_HEX:    hex value from 00 to 7F   (i.E for volume)
	;; PARA_VALUE_PM_HEX: hex value from -40 to 3F  (i.E for panorama)
	;; PARA_VALUE_DEC:    dec value from 1 to 128   (i.E for volume)
	;; PARA_VALUE_PM_DEC: dec value from -64 to 63  (i.E for panorama)
	;; values are defined in app_defines.h
	;; feel free to add more modes!

	;; don't change the string lengths, they are important to calculate
	;; the pointer to the entry (see tc_display.inc)

	;; Bank 1
	;;              |<-->|  |<----------->|
	PARA_ENTRY	" Vol", "Volume Chn1  ", PARA_VALUE_DEC
	PARA_ENTRY	" Pan", "Pan    Chn1  ", PARA_VALUE_PM_DEC
	PARA_ENTRY	" EQL", "EQ Low Chn1  ", PARA_VALUE_DEC
	PARA_ENTRY	" EQM", "EQ Mid Chn1  ", PARA_VALUE_DEC
	PARA_ENTRY	" EQH", "EQ Hig Chn1  ", PARA_VALUE_DEC

	;; Bank 2
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#006", "CC#  6 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#007", "CC#  7 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#008", "CC#  8 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#009", "CC#  9 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#010", "CC# 10 Chn.1 ", PARA_VALUE_DEC

	;; Bank 3
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#011", "CC# 11 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#012", "CC# 12 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#013", "CC# 13 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#014", "CC# 14 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#015", "CC# 15 Chn.1 ", PARA_VALUE_DEC

	;; Bank 4
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#016", "CC# 16 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#017", "CC# 17 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#018", "CC# 18 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#019", "CC# 19 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#020", "CC# 20 Chn.1 ", PARA_VALUE_DEC

	;; Bank 5
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#021", "CC# 21 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#022", "CC# 22 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#023", "CC# 23 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#024", "CC# 24 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#025", "CC# 25 Chn.1 ", PARA_VALUE_DEC

	;; Bank 6
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#026", "CC# 26 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#027", "CC# 27 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#028", "CC# 28 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#029", "CC# 29 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#030", "CC# 30 Chn.1 ", PARA_VALUE_DEC

	;; Bank 7
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#031", "CC# 31 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#032", "CC# 32 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#033", "CC# 33 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#034", "CC# 34 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#035", "CC# 35 Chn.1 ", PARA_VALUE_DEC

	;; Bank 8
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#036", "CC# 36 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#037", "CC# 37 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#038", "CC# 38 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#039", "CC# 39 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#040", "CC# 40 Chn.1 ", PARA_VALUE_DEC

	;; Bank 9
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#041", "CC# 41 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#042", "CC# 42 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#043", "CC# 43 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#044", "CC# 44 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#045", "CC# 45 Chn.1 ", PARA_VALUE_DEC

	;; Bank 10
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#046", "CC# 46 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#047", "CC# 47 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#048", "CC# 48 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#049", "CC# 49 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#050", "CC# 50 Chn.1 ", PARA_VALUE_DEC

	;; Bank 11
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#051", "CC# 51 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#052", "CC# 52 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#053", "CC# 53 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#054", "CC# 54 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#055", "CC# 55 Chn.1 ", PARA_VALUE_DEC

	;; Bank 12
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#056", "CC# 56 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#057", "CC# 57 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#058", "CC# 58 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#059", "CC# 59 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#060", "CC# 60 Chn.1 ", PARA_VALUE_DEC

	;; Bank 13
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#061", "CC# 61 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#062", "CC# 62 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#063", "CC# 63 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#064", "CC# 64 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#065", "CC# 65 Chn.1 ", PARA_VALUE_DEC

	;; Bank 14
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#066", "CC# 66 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#067", "CC# 67 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#068", "CC# 68 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#069", "CC# 69 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#070", "CC# 70 Chn.1 ", PARA_VALUE_DEC

	;; Bank 15
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#071", "CC# 71 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#072", "CC# 72 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#073", "CC# 73 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#074", "CC# 74 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#075", "CC# 75 Chn.1 ", PARA_VALUE_DEC

	;; Bank 16
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#076", "CC# 76 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#077", "CC# 77 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#078", "CC# 78 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#079", "CC# 79 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#080", "CC# 80 Chn.1 ", PARA_VALUE_DEC

	;; Bank 17
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#081", "CC# 81 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#082", "CC# 82 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#083", "CC# 83 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#084", "CC# 84 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#085", "CC# 85 Chn.1 ", PARA_VALUE_DEC

	;; Bank 18
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#086", "CC# 86 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#087", "CC# 87 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#088", "CC# 88 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#089", "CC# 89 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#090", "CC# 90 Chn.1 ", PARA_VALUE_DEC

	;; Bank 19
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#091", "CC# 91 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#092", "CC# 92 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#093", "CC# 93 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#094", "CC# 94 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#095", "CC# 95 Chn.1 ", PARA_VALUE_DEC

	;; Bank 20
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#096", "CC# 96 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#097", "CC# 97 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#098", "CC# 98 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#099", "CC# 99 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#100", "CC#100 Chn.1 ", PARA_VALUE_DEC

	;; Bank 21
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#101", "CC#101 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#102", "CC#102 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#103", "CC#103 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#104", "CC#104 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#105", "CC#105 Chn.1 ", PARA_VALUE_DEC

	;; Bank 22
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#106", "CC#106 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#107", "CC#107 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#108", "CC#108 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#109", "CC#109 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#110", "CC#110 Chn.1 ", PARA_VALUE_DEC

	;; Bank 23
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#111", "CC#111 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#112", "CC#112 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#113", "CC#113 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#114", "CC#114 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#115", "CC#115 Chn.1 ", PARA_VALUE_DEC

	;; Bank 24
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#116", "CC#116 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#117", "CC#117 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#118", "CC#118 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#119", "CC#119 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#120", "CC#120 Chn.1 ", PARA_VALUE_DEC

	;; Bank 25
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#121", "CC#121 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#122", "CC#122 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#123", "CC#123 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#124", "CC#124 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#125", "CC#125 Chn.1 ", PARA_VALUE_DEC

	;; Bank 26
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#126", "CC#126 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#127", "CC#127 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#128", "CC#128 Chn.1 ", PARA_VALUE_DEC
	PARA_ENTRY	"#129", "CC#  1 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#130", "CC#  2 Chn.2 ", PARA_VALUE_DEC

	;; Bank 27
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#131", "CC#  3 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#132", "CC#  4 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#133", "CC#  5 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#134", "CC#  6 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#135", "CC#  7 Chn.2 ", PARA_VALUE_DEC

	;; Bank 28
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#136", "CC#  8 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#137", "CC#  9 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#138", "CC# 10 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#139", "CC# 11 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#140", "CC# 12 Chn.2 ", PARA_VALUE_DEC

	;; Bank 29
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#141", "CC# 13 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#142", "CC# 14 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#143", "CC# 15 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#144", "CC# 16 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#145", "CC# 17 Chn.2 ", PARA_VALUE_DEC

	;; Bank 30
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#146", "CC# 18 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#147", "CC# 19 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#148", "CC# 20 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#149", "CC# 21 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#150", "CC# 22 Chn.2 ", PARA_VALUE_DEC

	;; Bank 31
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#151", "CC# 23 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#152", "CC# 24 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#153", "CC# 25 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#154", "CC# 26 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#155", "CC# 27 Chn.2 ", PARA_VALUE_DEC

	;; Bank 32
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#156", "CC# 28 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#157", "CC# 29 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#158", "CC# 30 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#159", "CC# 31 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#160", "CC# 32 Chn.2 ", PARA_VALUE_DEC

	;; Bank 33
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#161", "CC# 33 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#162", "CC# 34 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#163", "CC# 35 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#164", "CC# 36 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#165", "CC# 37 Chn.2 ", PARA_VALUE_DEC

	;; Bank 34
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#166", "CC# 38 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#167", "CC# 39 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#168", "CC# 40 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#169", "CC# 41 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#170", "CC# 42 Chn.2 ", PARA_VALUE_DEC

	;; Bank 35
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#171", "CC# 43 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#172", "CC# 44 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#173", "CC# 45 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#174", "CC# 46 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#175", "CC# 47 Chn.2 ", PARA_VALUE_DEC

	;; Bank 36
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#176", "CC# 48 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#177", "CC# 49 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#178", "CC# 50 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#179", "CC# 51 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#180", "CC# 52 Chn.2 ", PARA_VALUE_DEC

	;; Bank 37
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#181", "CC# 53 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#182", "CC# 54 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#183", "CC# 55 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#184", "CC# 56 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#185", "CC# 57 Chn.2 ", PARA_VALUE_DEC

	;; Bank 38
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#186", "CC# 58 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#187", "CC# 59 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#188", "CC# 60 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#189", "CC# 61 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#190", "CC# 62 Chn.2 ", PARA_VALUE_DEC

	;; Bank 39
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#191", "CC# 63 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#192", "CC# 64 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#193", "CC# 65 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#194", "CC# 66 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#195", "CC# 67 Chn.2 ", PARA_VALUE_DEC

	;; Bank 40
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#196", "CC# 68 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#197", "CC# 69 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#198", "CC# 70 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#199", "CC# 71 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#200", "CC# 72 Chn.2 ", PARA_VALUE_DEC

	;; Bank 41
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#201", "CC# 73 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#202", "CC# 74 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#203", "CC# 75 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#204", "CC# 76 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#205", "CC# 77 Chn.2 ", PARA_VALUE_DEC

	;; Bank 42
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#206", "CC# 78 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#207", "CC# 79 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#208", "CC# 80 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#209", "CC# 81 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#210", "CC# 82 Chn.2 ", PARA_VALUE_DEC

	;; Bank 43
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#211", "CC# 83 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#212", "CC# 84 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#213", "CC# 85 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#214", "CC# 86 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#215", "CC# 87 Chn.2 ", PARA_VALUE_DEC

	;; Bank 44
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#216", "CC# 88 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#217", "CC# 89 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#218", "CC# 90 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#219", "CC# 91 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#220", "CC# 92 Chn.2 ", PARA_VALUE_DEC

	;; Bank 45
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#221", "CC# 93 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#222", "CC# 94 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#223", "CC# 95 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#224", "CC# 96 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#225", "CC# 97 Chn.2 ", PARA_VALUE_DEC

	;; Bank 46
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#226", "CC# 98 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#227", "CC# 99 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#228", "CC#100 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#229", "CC#101 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#230", "CC#102 Chn.2 ", PARA_VALUE_DEC

	;; Bank 47
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#231", "CC#103 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#232", "CC#104 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#233", "CC#105 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#234", "CC#106 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#235", "CC#107 Chn.2 ", PARA_VALUE_DEC

	;; Bank 48
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#236", "CC#108 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#237", "CC#109 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#238", "CC#110 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#239", "CC#111 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#240", "CC#112 Chn.2 ", PARA_VALUE_DEC

	;; Bank 49
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#241", "CC#113 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#242", "CC#114 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#243", "CC#115 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#244", "CC#116 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#245", "CC#117 Chn.2 ", PARA_VALUE_DEC

	;; Bank 50
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#246", "CC#118 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#247", "CC#119 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#248", "CC#120 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#249", "CC#121 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#250", "CC#122 Chn.2 ", PARA_VALUE_DEC

	;; Bank 51
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#251", "CC#123 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#252", "CC#124 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#253", "CC#125 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#254", "CC#126 Chn.2 ", PARA_VALUE_DEC
	PARA_ENTRY	"#255", "CC#127 Chn.2 ", PARA_VALUE_DEC

	;; not used
	;;              |<-->|  |<----------->|
	PARA_ENTRY	"#256", "CC#128 Chn.2 ", PARA_VALUE_DEC
