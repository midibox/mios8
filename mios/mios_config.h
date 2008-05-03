; $Id$
;
; MIOS configuration flags
; Allows you to disable features to get some memory free for own extensions
;
; ==========================================================================
;
;  Copyright 1998-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; AIN Driver (ca. 500 bytes)
#define MIOS_ENABLE_AIN 1

;; Motorfader Driver (ca. 800 bytes) - requires AIN driver
#define MIOS_ENABLE_MF 1

;; Encoder Driver (ca. 700 bytes)
#define MIOS_ENABLE_ENC 1

;; GLCD0 Driver (ca. 400 bytes)
#define MIOS_ENABLE_GLCD0 1

;; MLCD Driver (ca. 100 bytes)
#define MIOS_ENABLE_MLCD 1



;; more enable flags will be added on request
