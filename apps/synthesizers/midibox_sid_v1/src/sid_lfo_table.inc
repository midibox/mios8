; $Id$
;
; MIDIbox SID
; LFO Table
; generated with lfo_frq.pl - calculates sweep from 0.001 Hz to 50 Hz
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;; In: index (0x00-0x7f) in WREG
;; Out: CTR value in WREG and MIOS_PARAMETER1, ADD value in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SID_LFO_TABLE_Get
	clrc
	rlf	WREG, W
	addlw	SID_LFO_TABLE & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	SID_LFO_TABLE >> 8
	addwfc	TBLPTRH, F

	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2	; -> ADD value
	tblrd*+
	movf	TABLAT, W
	movwf	MIOS_PARAMETER1		; -> CTR value
	return

;   x | Frq.   | CTR | ADD | Result Frq
; ----+--------+-----+-----+-----------
;   1 |  0.000 | 255 |   1 |  0.001
;   2 |  0.031 |  77 |   1 |  0.032
;   3 |  0.063 |  38 |   1 |  0.064
;   4 |  0.096 |  25 |   1 |  0.098
;   5 |  0.130 |  19 |   1 |  0.128
;   6 |  0.165 |  15 |   1 |  0.163
;   7 |  0.201 |  12 |   1 |  0.203
;   8 |  0.238 |  21 |   2 |  0.233
;   9 |  0.276 |   9 |   1 |  0.271
;  10 |  0.316 |  23 |   3 |  0.318
;  11 |  0.357 |   7 |   1 |  0.349
;  12 |  0.399 |   6 |   1 |  0.407
;  13 |  0.442 |  11 |   2 |  0.444
;  14 |  0.487 |   5 |   1 |  0.488
;  15 |  0.533 |   9 |   2 |  0.543
;  16 |  0.580 |  17 |   4 |  0.574
;  17 |  0.629 |  19 |   5 |  0.642
;  18 |  0.680 |  11 |   3 |  0.666
;  19 |  0.732 |  10 |   3 |  0.732
;  20 |  0.785 |  19 |   6 |  0.771
;  21 |  0.840 |  20 |   7 |  0.854
;  22 |  0.897 |   8 |   3 |  0.916
;  23 |  0.956 |   5 |   2 |  0.977
;  24 |  1.017 |  12 |   5 |  1.017
;  25 |  1.079 |   9 |   4 |  1.085
;  26 |  1.144 |  13 |   6 |  1.127
;  27 |  1.210 |   2 |   1 |  1.221
;  28 |  1.278 |  15 |   8 |  1.302
;  29 |  1.349 |   9 |   5 |  1.356
;  30 |  1.422 |   7 |   4 |  1.395
;  31 |  1.497 |   5 |   3 |  1.465
;  32 |  1.574 |  11 |   7 |  1.554
;  33 |  1.654 |   3 |   2 |  1.628
;  34 |  1.736 |   7 |   5 |  1.744
;  35 |  1.821 |   4 |   3 |  1.831
;  36 |  1.908 |   5 |   4 |  1.953
;  37 |  1.998 |   5 |   4 |  1.953
;  38 |  2.091 |   7 |   6 |  2.093
;  39 |  2.187 |   8 |   7 |  2.136
;  40 |  2.285 |  12 |  11 |  2.238
;  41 |  2.387 |   1 |   1 |  2.441
;  42 |  2.492 |   1 |   1 |  2.441
;  43 |  2.600 |  11 |  12 |  2.663
;  44 |  2.712 |   8 |   9 |  2.747
;  45 |  2.827 |   6 |   7 |  2.848
;  46 |  2.945 |   5 |   6 |  2.930
;  47 |  3.067 |   4 |   5 |  3.052
;  48 |  3.193 |   3 |   4 |  3.255
;  49 |  3.323 |   3 |   4 |  3.255
;  50 |  3.457 |   5 |   7 |  3.418
;  51 |  3.595 |   2 |   3 |  3.662
;  52 |  3.737 |   2 |   3 |  3.662
;  53 |  3.884 |   5 |   8 |  3.906
;  54 |  4.035 |   3 |   5 |  4.069
;  55 |  4.191 |   4 |   7 |  4.272
;  56 |  4.352 |   4 |   7 |  4.272
;  57 |  4.518 |   6 |  11 |  4.476
;  58 |  4.689 |   8 |  15 |  4.578
;  59 |  4.865 |   1 |   2 |  4.883
;  60 |  5.047 |   9 |  19 |  5.154
;  61 |  5.234 |   6 |  13 |  5.290
;  62 |  5.427 |   4 |   9 |  5.493
;  63 |  5.626 |   3 |   7 |  5.697
;  64 |  5.831 |   3 |   7 |  5.697
;  65 |  6.043 |   2 |   5 |  6.104
;  66 |  6.261 |   5 |  13 |  6.348
;  67 |  6.486 |   3 |   8 |  6.510
;  68 |  6.718 |   4 |  11 |  6.714
;  69 |  6.957 |   5 |  14 |  6.836
;  70 |  7.203 |   1 |   3 |  7.324
;  71 |  7.457 |   1 |   3 |  7.324
;  72 |  7.719 |   5 |  16 |  7.812
;  73 |  7.989 |   3 |  10 |  8.138
;  74 |  8.267 |   3 |  10 |  8.138
;  75 |  8.554 |   2 |   7 |  8.545
;  76 |  8.850 |   3 |  11 |  8.952
;  77 |  9.155 |   3 |  11 |  8.952
;  78 |  9.470 |   5 |  19 |  9.277
;  79 |  9.794 |   1 |   4 |  9.766
;  80 | 10.128 |   4 |  17 | 10.376
;  81 | 10.473 |   3 |  13 | 10.579
;  82 | 10.828 |   2 |   9 | 10.986
;  83 | 11.195 |   2 |   9 | 10.986
;  84 | 11.572 |   3 |  14 | 11.393
;  85 | 11.962 |   1 |   5 | 12.207
;  86 | 12.363 |   1 |   5 | 12.207
;  87 | 12.777 |   3 |  16 | 13.021
;  88 | 13.204 |   2 |  11 | 13.428
;  89 | 13.644 |   2 |  11 | 13.428
;  90 | 14.097 |   3 |  17 | 13.835
;  91 | 14.565 |   1 |   6 | 14.648
;  92 | 15.047 |   4 |  25 | 15.259
;  93 | 15.544 |   2 |  13 | 15.869
;  94 | 16.056 |   2 |  13 | 15.869
;  95 | 16.584 |   3 |  20 | 16.276
;  96 | 17.129 |   1 |   7 | 17.090
;  97 | 17.690 |   3 |  22 | 17.904
;  98 | 18.269 |   2 |  15 | 18.311
;  99 | 18.866 |   3 |  23 | 18.717
; 100 | 19.481 |   1 |   8 | 19.531
; 101 | 20.115 |   3 |  25 | 20.345
; 102 | 20.769 |   2 |  17 | 20.752
; 103 | 21.443 |   1 |   9 | 21.973
; 104 | 22.139 |   1 |   9 | 21.973
; 105 | 22.855 |   2 |  19 | 23.193
; 106 | 23.594 |   2 |  19 | 23.193
; 107 | 24.356 |   1 |  10 | 24.414
; 108 | 25.141 |   2 |  21 | 25.635
; 109 | 25.950 |   2 |  21 | 25.635
; 110 | 26.785 |   1 |  11 | 26.855
; 111 | 27.646 |   2 |  23 | 28.076
; 112 | 28.533 |   2 |  23 | 28.076
; 113 | 29.447 |   1 |  12 | 29.297
; 114 | 30.390 |   2 |  25 | 30.518
; 115 | 31.362 |   1 |  13 | 31.738
; 116 | 32.365 |   1 |  13 | 31.738
; 117 | 33.398 |   1 |  14 | 34.180
; 118 | 34.463 |   1 |  14 | 34.180
; 119 | 35.562 |   2 |  29 | 35.400
; 120 | 36.694 |   1 |  15 | 36.621
; 121 | 37.861 |   2 |  31 | 37.842
; 122 | 39.065 |   1 |  16 | 39.062
; 123 | 40.306 |   2 |  33 | 40.283
; 124 | 41.585 |   1 |  17 | 41.504
; 125 | 42.904 |   1 |  18 | 43.945
; 126 | 44.263 |   1 |  18 | 43.945
; 127 | 45.665 |   1 |  19 | 46.387

SID_LFO_TABLE
	db	0x01, 0xff	; ADD/CTR for frq=  0.00
	db	0x01, 0xff	; ADD/CTR for frq=  0.00
	db	0x01, 0x4d	; ADD/CTR for frq=  0.03
	db	0x01, 0x26	; ADD/CTR for frq=  0.06
	db	0x01, 0x19	; ADD/CTR for frq=  0.10
	db	0x01, 0x13	; ADD/CTR for frq=  0.13
	db	0x01, 0x0f	; ADD/CTR for frq=  0.16
	db	0x01, 0x0c	; ADD/CTR for frq=  0.20
	db	0x02, 0x15	; ADD/CTR for frq=  0.23
	db	0x01, 0x09	; ADD/CTR for frq=  0.27
	db	0x03, 0x17	; ADD/CTR for frq=  0.32
	db	0x01, 0x07	; ADD/CTR for frq=  0.35
	db	0x01, 0x06	; ADD/CTR for frq=  0.41
	db	0x02, 0x0b	; ADD/CTR for frq=  0.44
	db	0x01, 0x05	; ADD/CTR for frq=  0.49
	db	0x02, 0x09	; ADD/CTR for frq=  0.54
	db	0x04, 0x11	; ADD/CTR for frq=  0.57
	db	0x05, 0x13	; ADD/CTR for frq=  0.64
	db	0x03, 0x0b	; ADD/CTR for frq=  0.67
	db	0x03, 0x0a	; ADD/CTR for frq=  0.73
	db	0x06, 0x13	; ADD/CTR for frq=  0.77
	db	0x07, 0x14	; ADD/CTR for frq=  0.85
	db	0x03, 0x08	; ADD/CTR for frq=  0.92
	db	0x02, 0x05	; ADD/CTR for frq=  0.98
	db	0x05, 0x0c	; ADD/CTR for frq=  1.02
	db	0x04, 0x09	; ADD/CTR for frq=  1.09
	db	0x06, 0x0d	; ADD/CTR for frq=  1.13
	db	0x01, 0x02	; ADD/CTR for frq=  1.22
	db	0x08, 0x0f	; ADD/CTR for frq=  1.30
	db	0x05, 0x09	; ADD/CTR for frq=  1.36
	db	0x04, 0x07	; ADD/CTR for frq=  1.40
	db	0x03, 0x05	; ADD/CTR for frq=  1.46
	db	0x07, 0x0b	; ADD/CTR for frq=  1.55
	db	0x02, 0x03	; ADD/CTR for frq=  1.63
	db	0x05, 0x07	; ADD/CTR for frq=  1.74
	db	0x03, 0x04	; ADD/CTR for frq=  1.83
	db	0x04, 0x05	; ADD/CTR for frq=  1.95
	db	0x04, 0x05	; ADD/CTR for frq=  1.95
	db	0x06, 0x07	; ADD/CTR for frq=  2.09
	db	0x07, 0x08	; ADD/CTR for frq=  2.14
	db	0x0b, 0x0c	; ADD/CTR for frq=  2.24
	db	0x01, 0x01	; ADD/CTR for frq=  2.44
	db	0x01, 0x01	; ADD/CTR for frq=  2.44
	db	0x0c, 0x0b	; ADD/CTR for frq=  2.66
	db	0x09, 0x08	; ADD/CTR for frq=  2.75
	db	0x07, 0x06	; ADD/CTR for frq=  2.85
	db	0x06, 0x05	; ADD/CTR for frq=  2.93
	db	0x05, 0x04	; ADD/CTR for frq=  3.05
	db	0x04, 0x03	; ADD/CTR for frq=  3.26
	db	0x04, 0x03	; ADD/CTR for frq=  3.26
	db	0x07, 0x05	; ADD/CTR for frq=  3.42
	db	0x03, 0x02	; ADD/CTR for frq=  3.66
	db	0x03, 0x02	; ADD/CTR for frq=  3.66
	db	0x08, 0x05	; ADD/CTR for frq=  3.91
	db	0x05, 0x03	; ADD/CTR for frq=  4.07
	db	0x07, 0x04	; ADD/CTR for frq=  4.27
	db	0x07, 0x04	; ADD/CTR for frq=  4.27
	db	0x0b, 0x06	; ADD/CTR for frq=  4.48
	db	0x0f, 0x08	; ADD/CTR for frq=  4.58
	db	0x02, 0x01	; ADD/CTR for frq=  4.88
	db	0x13, 0x09	; ADD/CTR for frq=  5.15
	db	0x0d, 0x06	; ADD/CTR for frq=  5.29
	db	0x09, 0x04	; ADD/CTR for frq=  5.49
	db	0x07, 0x03	; ADD/CTR for frq=  5.70
	db	0x07, 0x03	; ADD/CTR for frq=  5.70
	db	0x05, 0x02	; ADD/CTR for frq=  6.10
	db	0x0d, 0x05	; ADD/CTR for frq=  6.35
	db	0x08, 0x03	; ADD/CTR for frq=  6.51
	db	0x0b, 0x04	; ADD/CTR for frq=  6.71
	db	0x0e, 0x05	; ADD/CTR for frq=  6.84
	db	0x03, 0x01	; ADD/CTR for frq=  7.32
	db	0x03, 0x01	; ADD/CTR for frq=  7.32
	db	0x10, 0x05	; ADD/CTR for frq=  7.81
	db	0x0a, 0x03	; ADD/CTR for frq=  8.14
	db	0x0a, 0x03	; ADD/CTR for frq=  8.14
	db	0x07, 0x02	; ADD/CTR for frq=  8.54
	db	0x0b, 0x03	; ADD/CTR for frq=  8.95
	db	0x0b, 0x03	; ADD/CTR for frq=  8.95
	db	0x13, 0x05	; ADD/CTR for frq=  9.28
	db	0x04, 0x01	; ADD/CTR for frq=  9.77
	db	0x11, 0x04	; ADD/CTR for frq= 10.38
	db	0x0d, 0x03	; ADD/CTR for frq= 10.58
	db	0x09, 0x02	; ADD/CTR for frq= 10.99
	db	0x09, 0x02	; ADD/CTR for frq= 10.99
	db	0x0e, 0x03	; ADD/CTR for frq= 11.39
	db	0x05, 0x01	; ADD/CTR for frq= 12.21
	db	0x05, 0x01	; ADD/CTR for frq= 12.21
	db	0x10, 0x03	; ADD/CTR for frq= 13.02
	db	0x0b, 0x02	; ADD/CTR for frq= 13.43
	db	0x0b, 0x02	; ADD/CTR for frq= 13.43
	db	0x11, 0x03	; ADD/CTR for frq= 13.83
	db	0x06, 0x01	; ADD/CTR for frq= 14.65
	db	0x19, 0x04	; ADD/CTR for frq= 15.26
	db	0x0d, 0x02	; ADD/CTR for frq= 15.87
	db	0x0d, 0x02	; ADD/CTR for frq= 15.87
	db	0x14, 0x03	; ADD/CTR for frq= 16.28
	db	0x07, 0x01	; ADD/CTR for frq= 17.09
	db	0x16, 0x03	; ADD/CTR for frq= 17.90
	db	0x0f, 0x02	; ADD/CTR for frq= 18.31
	db	0x17, 0x03	; ADD/CTR for frq= 18.72
	db	0x08, 0x01	; ADD/CTR for frq= 19.53
	db	0x19, 0x03	; ADD/CTR for frq= 20.35
	db	0x11, 0x02	; ADD/CTR for frq= 20.75
	db	0x09, 0x01	; ADD/CTR for frq= 21.97
	db	0x09, 0x01	; ADD/CTR for frq= 21.97
	db	0x13, 0x02	; ADD/CTR for frq= 23.19
	db	0x13, 0x02	; ADD/CTR for frq= 23.19
	db	0x0a, 0x01	; ADD/CTR for frq= 24.41
	db	0x15, 0x02	; ADD/CTR for frq= 25.63
	db	0x15, 0x02	; ADD/CTR for frq= 25.63
	db	0x0b, 0x01	; ADD/CTR for frq= 26.86
	db	0x17, 0x02	; ADD/CTR for frq= 28.08
	db	0x17, 0x02	; ADD/CTR for frq= 28.08
	db	0x0c, 0x01	; ADD/CTR for frq= 29.30
	db	0x19, 0x02	; ADD/CTR for frq= 30.52
	db	0x0d, 0x01	; ADD/CTR for frq= 31.74
	db	0x0d, 0x01	; ADD/CTR for frq= 31.74
	db	0x0e, 0x01	; ADD/CTR for frq= 34.18
	db	0x0e, 0x01	; ADD/CTR for frq= 34.18
	db	0x1d, 0x02	; ADD/CTR for frq= 35.40
	db	0x0f, 0x01	; ADD/CTR for frq= 36.62
	db	0x1f, 0x02	; ADD/CTR for frq= 37.84
	db	0x10, 0x01	; ADD/CTR for frq= 39.06
	db	0x21, 0x02	; ADD/CTR for frq= 40.28
	db	0x11, 0x01	; ADD/CTR for frq= 41.50
	db	0x12, 0x01	; ADD/CTR for frq= 43.95
	db	0x12, 0x01	; ADD/CTR for frq= 43.95
	db	0x13, 0x01	; ADD/CTR for frq= 46.39
	db	0x13, 0x01	; dummy
