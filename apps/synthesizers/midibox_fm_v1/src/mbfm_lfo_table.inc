; $Id$
;
; MIDIbox FM
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
MBFM_LFO_TABLE_Get
	clrc
	rlf	WREG, W
	addlw	MBFM_LFO_TABLE & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	MBFM_LFO_TABLE >> 8
	addwfc	TBLPTRH, F

	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2	; -> ADD value
	tblrd*+
	movf	TABLAT, W
	movwf	MIOS_PARAMETER1		; -> CTR value
	return

;   x | Frq.   | CTR | ADD | Result Frq
; ----+--------+-----+-----+-----------
;   1 |  0.000 |   0 |   0 |  0.000
;   2 |  0.031 |  31 |   1 |  0.032
;   3 |  0.063 |  31 |   2 |  0.063
;   4 |  0.096 |  10 |   1 |  0.098
;   5 |  0.130 |  15 |   2 |  0.130
;   6 |  0.165 |   6 |   1 |  0.163
;   7 |  0.201 |  19 |   4 |  0.206
;   8 |  0.238 |  21 |   5 |  0.233
;   9 |  0.276 |   7 |   2 |  0.279
;  10 |  0.316 |  19 |   6 |  0.308
;  11 |  0.357 |  11 |   4 |  0.355
;  12 |  0.399 |   5 |   2 |  0.391
;  13 |  0.442 |   9 |   4 |  0.434
;  14 |  0.487 |   2 |   1 |  0.488
;  15 |  0.533 |   9 |   5 |  0.543
;  16 |  0.580 |   5 |   3 |  0.586
;  17 |  0.629 |  11 |   7 |  0.621
;  18 |  0.680 |  10 |   7 |  0.684
;  19 |  0.732 |   4 |   3 |  0.732
;  20 |  0.785 |   5 |   4 |  0.781
;  21 |  0.840 |   7 |   6 |  0.837
;  22 |  0.897 |  10 |   9 |  0.879
;  23 |  0.956 |   1 |   1 |  0.977
;  24 |  1.017 |  15 |  16 |  1.042
;  25 |  1.079 |   8 |   9 |  1.099
;  26 |  1.144 |   5 |   6 |  1.172
;  27 |  1.210 |   4 |   5 |  1.221
;  28 |  1.278 |   3 |   4 |  1.302
;  29 |  1.349 |   5 |   7 |  1.367
;  30 |  1.422 |   7 |  10 |  1.395
;  31 |  1.497 |   2 |   3 |  1.465
;  32 |  1.574 |   5 |   8 |  1.563
;  33 |  1.654 |   3 |   5 |  1.628
;  34 |  1.736 |   4 |   7 |  1.709
;  35 |  1.821 |   6 |  11 |  1.790
;  36 |  1.908 |   1 |   2 |  1.953
;  37 |  1.998 |   1 |   2 |  1.953
;  38 |  2.091 |   6 |  13 |  2.116
;  39 |  2.187 |   4 |   9 |  2.197
;  40 |  2.285 |   3 |   7 |  2.279
;  41 |  2.387 |   2 |   5 |  2.441
;  42 |  2.492 |   2 |   5 |  2.441
;  43 |  2.600 |   3 |   8 |  2.604
;  44 |  2.712 |   4 |  11 |  2.686
;  45 |  2.827 |   6 |  17 |  2.767
;  46 |  2.945 |   1 |   3 |  2.930
;  47 |  3.067 |   5 |  16 |  3.125
;  48 |  3.193 |   3 |  10 |  3.255
;  49 |  3.323 |   3 |  10 |  3.255
;  50 |  3.457 |   2 |   7 |  3.418
;  51 |  3.595 |   3 |  11 |  3.581
;  52 |  3.737 |   4 |  15 |  3.662
;  53 |  3.884 |   1 |   4 |  3.906
;  54 |  4.035 |   5 |  21 |  4.102
;  55 |  4.191 |   3 |  13 |  4.232
;  56 |  4.352 |   2 |   9 |  4.395
;  57 |  4.518 |   3 |  14 |  4.557
;  58 |  4.689 |   4 |  19 |  4.639
;  59 |  4.865 |   1 |   5 |  4.883
;  60 |  5.047 |   4 |  21 |  5.127
;  61 |  5.234 |   3 |  16 |  5.208
;  62 |  5.427 |   2 |  11 |  5.371
;  63 |  5.626 |   3 |  17 |  5.534
;  64 |  5.831 |   1 |   6 |  5.859
;  65 |  6.043 |   3 |  19 |  6.185
;  66 |  6.261 |   2 |  13 |  6.348
;  67 |  6.486 |   2 |  13 |  6.348
;  68 |  6.718 |   1 |   7 |  6.836
;  69 |  6.957 |   1 |   7 |  6.836
;  70 |  7.203 |   2 |  15 |  7.324
;  71 |  7.457 |   2 |  15 |  7.324
;  72 |  7.719 |   1 |   8 |  7.813
;  73 |  7.989 |   1 |   8 |  7.813
;  74 |  8.267 |   2 |  17 |  8.301
;  75 |  8.554 |   3 |  26 |  8.464
;  76 |  8.850 |   1 |   9 |  8.789
;  77 |  9.155 |   2 |  19 |  9.277
;  78 |  9.470 |   2 |  19 |  9.277
;  79 |  9.794 |   1 |  10 |  9.766
;  80 | 10.128 |   2 |  21 | 10.254
;  81 | 10.473 |   2 |  21 | 10.254
;  82 | 10.828 |   1 |  11 | 10.742
;  83 | 11.195 |   2 |  23 | 11.230
;  84 | 11.572 |   1 |  12 | 11.719
;  85 | 11.962 |   1 |  12 | 11.719
;  86 | 12.363 |   2 |  25 | 12.207
;  87 | 12.777 |   1 |  13 | 12.695
;  88 | 13.204 |   2 |  27 | 13.184
;  89 | 13.644 |   1 |  14 | 13.672
;  90 | 14.097 |   2 |  29 | 14.160
;  91 | 14.565 |   1 |  15 | 14.648
;  92 | 15.047 |   2 |  31 | 15.137
;  93 | 15.544 |   1 |  16 | 15.625
;  94 | 16.056 |   2 |  33 | 16.113
;  95 | 16.584 |   1 |  17 | 16.602
;  96 | 17.129 |   2 |  35 | 17.090
;  97 | 17.690 |   1 |  18 | 17.578
;  98 | 18.269 |   1 |  19 | 18.555
;  99 | 18.866 |   1 |  19 | 18.555
; 100 | 19.481 |   1 |  20 | 19.531
; 101 | 20.115 |   1 |  21 | 20.508
; 102 | 20.769 |   1 |  21 | 20.508
; 103 | 21.443 |   1 |  22 | 21.484
; 104 | 22.139 |   1 |  23 | 22.461
; 105 | 22.855 |   1 |  23 | 22.461
; 106 | 23.594 |   1 |  24 | 23.438
; 107 | 24.356 |   1 |  25 | 24.414
; 108 | 25.141 |   1 |  26 | 25.391
; 109 | 25.950 |   1 |  26 | 25.391
; 110 | 26.785 |   1 |  27 | 26.367
; 111 | 27.646 |   1 |  28 | 27.344
; 112 | 28.533 |   1 |  29 | 28.320
; 113 | 29.447 |   1 |  30 | 29.297
; 114 | 30.390 |   1 |  31 | 30.273
; 115 | 31.362 |   1 |  32 | 31.250
; 116 | 32.365 |   1 |  33 | 32.227
; 117 | 33.398 |   1 |  34 | 33.203
; 118 | 34.463 |   1 |  35 | 34.180
; 119 | 35.562 |   1 |  36 | 35.156
; 120 | 36.694 |   1 |  37 | 36.133
; 121 | 37.861 |   1 |  38 | 37.109
; 122 | 39.065 |   1 |  40 | 39.063
; 123 | 40.306 |   1 |  41 | 40.039
; 124 | 41.585 |   1 |  42 | 41.016
; 125 | 42.904 |   1 |  43 | 41.992
; 126 | 44.263 |   1 |  45 | 43.945
; 127 | 45.665 |   1 |  46 | 44.922

MBFM_LFO_TABLE
	db	0x00, 0x00	; ADD/CTR for frq=  0.00
	db	0x01, 0x1f	; ADD/CTR for frq=  0.03
	db	0x02, 0x1f	; ADD/CTR for frq=  0.06
	db	0x01, 0x0a	; ADD/CTR for frq=  0.10
	db	0x02, 0x0f	; ADD/CTR for frq=  0.13
	db	0x01, 0x06	; ADD/CTR for frq=  0.16
	db	0x04, 0x13	; ADD/CTR for frq=  0.21
	db	0x05, 0x15	; ADD/CTR for frq=  0.23
	db	0x02, 0x07	; ADD/CTR for frq=  0.28
	db	0x06, 0x13	; ADD/CTR for frq=  0.31
	db	0x04, 0x0b	; ADD/CTR for frq=  0.36
	db	0x02, 0x05	; ADD/CTR for frq=  0.39
	db	0x04, 0x09	; ADD/CTR for frq=  0.43
	db	0x01, 0x02	; ADD/CTR for frq=  0.49
	db	0x05, 0x09	; ADD/CTR for frq=  0.54
	db	0x03, 0x05	; ADD/CTR for frq=  0.59
	db	0x07, 0x0b	; ADD/CTR for frq=  0.62
	db	0x07, 0x0a	; ADD/CTR for frq=  0.68
	db	0x03, 0x04	; ADD/CTR for frq=  0.73
	db	0x04, 0x05	; ADD/CTR for frq=  0.78
	db	0x06, 0x07	; ADD/CTR for frq=  0.84
	db	0x09, 0x0a	; ADD/CTR for frq=  0.88
	db	0x01, 0x01	; ADD/CTR for frq=  0.98
	db	0x10, 0x0f	; ADD/CTR for frq=  1.04
	db	0x09, 0x08	; ADD/CTR for frq=  1.10
	db	0x06, 0x05	; ADD/CTR for frq=  1.17
	db	0x05, 0x04	; ADD/CTR for frq=  1.22
	db	0x04, 0x03	; ADD/CTR for frq=  1.30
	db	0x07, 0x05	; ADD/CTR for frq=  1.37
	db	0x0a, 0x07	; ADD/CTR for frq=  1.40
	db	0x03, 0x02	; ADD/CTR for frq=  1.46
	db	0x08, 0x05	; ADD/CTR for frq=  1.56
	db	0x05, 0x03	; ADD/CTR for frq=  1.63
	db	0x07, 0x04	; ADD/CTR for frq=  1.71
	db	0x0b, 0x06	; ADD/CTR for frq=  1.79
	db	0x02, 0x01	; ADD/CTR for frq=  1.95
	db	0x02, 0x01	; ADD/CTR for frq=  1.95
	db	0x0d, 0x06	; ADD/CTR for frq=  2.12
	db	0x09, 0x04	; ADD/CTR for frq=  2.20
	db	0x07, 0x03	; ADD/CTR for frq=  2.28
	db	0x05, 0x02	; ADD/CTR for frq=  2.44
	db	0x05, 0x02	; ADD/CTR for frq=  2.44
	db	0x08, 0x03	; ADD/CTR for frq=  2.60
	db	0x0b, 0x04	; ADD/CTR for frq=  2.69
	db	0x11, 0x06	; ADD/CTR for frq=  2.77
	db	0x03, 0x01	; ADD/CTR for frq=  2.93
	db	0x10, 0x05	; ADD/CTR for frq=  3.13
	db	0x0a, 0x03	; ADD/CTR for frq=  3.26
	db	0x0a, 0x03	; ADD/CTR for frq=  3.26
	db	0x07, 0x02	; ADD/CTR for frq=  3.42
	db	0x0b, 0x03	; ADD/CTR for frq=  3.58
	db	0x0f, 0x04	; ADD/CTR for frq=  3.66
	db	0x04, 0x01	; ADD/CTR for frq=  3.91
	db	0x15, 0x05	; ADD/CTR for frq=  4.10
	db	0x0d, 0x03	; ADD/CTR for frq=  4.23
	db	0x09, 0x02	; ADD/CTR for frq=  4.39
	db	0x0e, 0x03	; ADD/CTR for frq=  4.56
	db	0x13, 0x04	; ADD/CTR for frq=  4.64
	db	0x05, 0x01	; ADD/CTR for frq=  4.88
	db	0x15, 0x04	; ADD/CTR for frq=  5.13
	db	0x10, 0x03	; ADD/CTR for frq=  5.21
	db	0x0b, 0x02	; ADD/CTR for frq=  5.37
	db	0x11, 0x03	; ADD/CTR for frq=  5.53
	db	0x06, 0x01	; ADD/CTR for frq=  5.86
	db	0x13, 0x03	; ADD/CTR for frq=  6.18
	db	0x0d, 0x02	; ADD/CTR for frq=  6.35
	db	0x0d, 0x02	; ADD/CTR for frq=  6.35
	db	0x07, 0x01	; ADD/CTR for frq=  6.84
	db	0x07, 0x01	; ADD/CTR for frq=  6.84
	db	0x0f, 0x02	; ADD/CTR for frq=  7.32
	db	0x0f, 0x02	; ADD/CTR for frq=  7.32
	db	0x08, 0x01	; ADD/CTR for frq=  7.81
	db	0x08, 0x01	; ADD/CTR for frq=  7.81
	db	0x11, 0x02	; ADD/CTR for frq=  8.30
	db	0x1a, 0x03	; ADD/CTR for frq=  8.46
	db	0x09, 0x01	; ADD/CTR for frq=  8.79
	db	0x13, 0x02	; ADD/CTR for frq=  9.28
	db	0x13, 0x02	; ADD/CTR for frq=  9.28
	db	0x0a, 0x01	; ADD/CTR for frq=  9.77
	db	0x15, 0x02	; ADD/CTR for frq= 10.25
	db	0x15, 0x02	; ADD/CTR for frq= 10.25
	db	0x0b, 0x01	; ADD/CTR for frq= 10.74
	db	0x17, 0x02	; ADD/CTR for frq= 11.23
	db	0x0c, 0x01	; ADD/CTR for frq= 11.72
	db	0x0c, 0x01	; ADD/CTR for frq= 11.72
	db	0x19, 0x02	; ADD/CTR for frq= 12.21
	db	0x0d, 0x01	; ADD/CTR for frq= 12.70
	db	0x1b, 0x02	; ADD/CTR for frq= 13.18
	db	0x0e, 0x01	; ADD/CTR for frq= 13.67
	db	0x1d, 0x02	; ADD/CTR for frq= 14.16
	db	0x0f, 0x01	; ADD/CTR for frq= 14.65
	db	0x1f, 0x02	; ADD/CTR for frq= 15.14
	db	0x10, 0x01	; ADD/CTR for frq= 15.63
	db	0x21, 0x02	; ADD/CTR for frq= 16.11
	db	0x11, 0x01	; ADD/CTR for frq= 16.60
	db	0x23, 0x02	; ADD/CTR for frq= 17.09
	db	0x12, 0x01	; ADD/CTR for frq= 17.58
	db	0x13, 0x01	; ADD/CTR for frq= 18.55
	db	0x13, 0x01	; ADD/CTR for frq= 18.55
	db	0x14, 0x01	; ADD/CTR for frq= 19.53
	db	0x15, 0x01	; ADD/CTR for frq= 20.51
	db	0x15, 0x01	; ADD/CTR for frq= 20.51
	db	0x16, 0x01	; ADD/CTR for frq= 21.48
	db	0x17, 0x01	; ADD/CTR for frq= 22.46
	db	0x17, 0x01	; ADD/CTR for frq= 22.46
	db	0x18, 0x01	; ADD/CTR for frq= 23.44
	db	0x19, 0x01	; ADD/CTR for frq= 24.41
	db	0x1a, 0x01	; ADD/CTR for frq= 25.39
	db	0x1a, 0x01	; ADD/CTR for frq= 25.39
	db	0x1b, 0x01	; ADD/CTR for frq= 26.37
	db	0x1c, 0x01	; ADD/CTR for frq= 27.34
	db	0x1d, 0x01	; ADD/CTR for frq= 28.32
	db	0x1e, 0x01	; ADD/CTR for frq= 29.30
	db	0x1f, 0x01	; ADD/CTR for frq= 30.27
	db	0x20, 0x01	; ADD/CTR for frq= 31.25
	db	0x21, 0x01	; ADD/CTR for frq= 32.23
	db	0x22, 0x01	; ADD/CTR for frq= 33.20
	db	0x23, 0x01	; ADD/CTR for frq= 34.18
	db	0x24, 0x01	; ADD/CTR for frq= 35.16
	db	0x25, 0x01	; ADD/CTR for frq= 36.13
	db	0x26, 0x01	; ADD/CTR for frq= 37.11
	db	0x28, 0x01	; ADD/CTR for frq= 39.06
	db	0x29, 0x01	; ADD/CTR for frq= 40.04
	db	0x2a, 0x01	; ADD/CTR for frq= 41.02
	db	0x2b, 0x01	; ADD/CTR for frq= 41.99
	db	0x2d, 0x01	; ADD/CTR for frq= 43.95
	db	0x2e, 0x01	; ADD/CTR for frq= 44.92
	db	0xff, 0x01	; ADD/CTR for frq= 44.92
