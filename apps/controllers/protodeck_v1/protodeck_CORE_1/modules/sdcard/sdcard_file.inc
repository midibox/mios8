; $Id: sdcard_file.inc 378 2008-07-06 20:38:32Z tk $
;
; FAT support for SD card driver
; Based on informations from following websites:
;   http://mirror.href.com/thestarman/asm/mbr/PartTables.htm
;   http://home.teleport.com/~brainy/fat16.htm
;   http://home.teleport.com/~brainy/lfn.htm
; 
; ==========================================================================
;
;  Copyright 1998-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; Note: keep codes in-sync with definitions in sdcard_file.h!
#define SDCARD_ERROR_NONE                 0x00

#define SDCARD_ERROR_SDC_ACCESS           0x01

#define SDCARD_ERROR_MBR_MISSING_MARKER   0x10
#define SDCARD_ERROR_MBR_WRONG_PARTI_TYPE 0x11

#define SDCARD_ERROR_BR_MISSING_MARKER    0x20
#define SDCARD_ERROR_BR_UNSUPP_SEC_SIZE   0x21
#define SDCARD_ERROR_BR_UNSUPP_CLUS_SIZE  0x22

#define SDCARD_ERROR_FILE_NOT_FOUND       0x30

#define SDCARD_ERROR_SEEK_INVALID_CLUS    0x40
#define SDCARD_ERROR_SEEK_INVALID_FAT     0x41
#define SDCARD_ERROR_SEEK_END_REACHED     0x42


#define SDCARD_FILE_MODE_CASE_SENSITIVE   0	; mode flag 0 controls case sensitivity
#define SDCARD_FILE_MODE_BACKWARD_SEARCH  1	; mode flag 1 controls forward/backward search
	

;; --------------------------------------------------------------------------
;;  Reads the master boot record and parition to prepare for file access
;;  IN: -
;;  OUT: error code in WREG
;;       sector of first partition in _sdcard_file_sector0
;;       sector of FAT in _sdcard_file_sector_fat
;;       sector of Root directory in _sdcard_file_sector_root
;;       Number of directory sectors in _sdcard_file_root_sectors
;;       sector of cluster 2 in _sdcard_file_sector_cl2
;;       cluster size in _sdcard_file_cluster_size
;;  USES: PROD[LH]
;; --------------------------------------------------------------------------
_SDCARD_FILE_Init
SDCARD_FILE_Init
	;; read MBR
	SET_BSR	_sdcard_address
	clrf	_sdcard_address+0, BANKED
	clrf	_sdcard_address+1, BANKED
	clrf	_sdcard_address+2, BANKED
	clrf	_sdcard_address+3, BANKED
	call	SDCARD_SectorRead
	bnz	SDCARD_FILE_Init_SD_Error

	;; references for MBR structure:
	;; http://mirror.href.com/thestarman/asm/mbr/PartTables.htm
	;; http://home.teleport.com/~brainy/fat16.htm

	;; check for boot marker
	movff	_sdcard_buffer_p1+0xfe, WREG
	xorlw	0x55
	bnz	SDCARD_FILE_Init_NoMBRMarker
	movff	_sdcard_buffer_p1+0xff, WREG
	xorlw	0xaa
	bz	SDCARD_FILE_Init_MBRMarkerOk
SDCARD_FILE_Init_NoMBRMarker
	movlw	SDCARD_ERROR_MBR_MISSING_MARKER
	rgoto	SDCARD_FILE_Init_End
SDCARD_FILE_Init_MBRMarkerOk

	;; check for type of partition (expecting FAT16 larger than 32MB)
	movff	_sdcard_buffer_p1+0xbe+4, WREG
	xorlw	0x06
	bz	SDCARD_FILE_Init_PartiTypeOk
SDCARD_FILE_Init_PartiTypeWrong
	movlw	SDCARD_ERROR_MBR_WRONG_PARTI_TYPE
	rgoto	SDCARD_FILE_Init_End
SDCARD_FILE_Init_PartiTypeOk

	;; get first sector of partition
	movff	_sdcard_buffer_p1+0xbe+8, _sdcard_file_sector0+0
	movff	_sdcard_buffer_p1+0xbe+9, _sdcard_file_sector0+1

	;; read FAT16 boot record at sector 0 of partition
	movff	_sdcard_file_sector0+0, _sdcard_address+0
	movff	_sdcard_file_sector0+1, _sdcard_address+1
	SET_BSR	_sdcard_address
	clrf	_sdcard_address+2, BANKED
	clrf	_sdcard_address+3, BANKED
	call	SDCARD_SectorRead
	bnz	SDCARD_FILE_Init_SD_Error

	;; check for boot marker
	movff	_sdcard_buffer_p1+0xfe, WREG
	xorlw	0x55
	bnz	SDCARD_FILE_Init_NoBRMarker
	movff	_sdcard_buffer_p1+0xff, WREG
	xorlw	0xaa
	bz	SDCARD_FILE_Init_BRMarkerOk
SDCARD_FILE_Init_NoBRMarker
	movlw	SDCARD_ERROR_BR_MISSING_MARKER
	rgoto	SDCARD_FILE_Init_End
SDCARD_FILE_Init_BRMarkerOk

	;; check that bytes per sectors is 512
	movff	_sdcard_buffer_p0+0x0b, WREG
	bnz	SDCARD_FILE_Init_UnsuppSecSize
	movff	_sdcard_buffer_p0+0x0c, WREG
	xorlw	0x02
	bz	SDCARD_FILE_Init_SecSizeOk
SDCARD_FILE_Init_UnsuppSecSize
	movlw	SDCARD_ERROR_BR_UNSUPP_SEC_SIZE
	rgoto	SDCARD_FILE_Init_End
SDCARD_FILE_Init_SecSizeOk

	;; sectors per clusters
	movff	_sdcard_buffer_p0+0x0d, _sdcard_file_cluster_size

	;; check if cluster size is supported (currently, we only support 2^n values)
	rcall	SDCARD_FILE_GetClusterShiftNum
	call	MIOS_HLP_GetBitORMask
	xorwf	_sdcard_file_cluster_size, W, BANKED
	bz	SDCARD_FILE_Init_ClusSizeOk
SDCARD_FILE_Init_UnsuppClusSize
	movlw	SDCARD_ERROR_BR_UNSUPP_CLUS_SIZE
	rgoto	SDCARD_FILE_Init_End
SDCARD_FILE_Init_ClusSizeOk

	;; calculate address to FAT
	SET_BSR	_sdcard_file_sector0
	movff	_sdcard_buffer_p0+0x0e, WREG	; (reserved sectors)
	addwf	_sdcard_file_sector0+0, W, BANKED
	movwf	_sdcard_file_sector_fat+0, BANKED
	movff	_sdcard_buffer_p0+0x0f, WREG	; (reserved sectors)
	addwfc	_sdcard_file_sector0+1, W, BANKED
	movwf	_sdcard_file_sector_fat+1, BANKED

	;; number of FAT sectors
	movff	_sdcard_buffer_p0+0x16, PRODL
	movff	_sdcard_buffer_p0+0x17, PRODH

	;; calculate address to root directory
	clrc			; (FAT sectors*2)
	rlf	PRODL, F
	rlf	PRODH, F
	movf	_sdcard_file_sector_fat+0, W, BANKED
	addwf	PRODL, W
	movwf	_sdcard_file_sector_root+0, BANKED
	movf	_sdcard_file_sector_fat+1, W, BANKED
	addwfc	PRODH, W
	movwf	_sdcard_file_sector_root+1, BANKED

	;; max number of root entries
	movff	_sdcard_buffer_p0+0x11, PRODL
	movff	_sdcard_buffer_p0+0x12, PRODH

	;; number of directory sectors (root entries / 16)
	swapf	PRODL, W
	andlw	0x0f
	movwf	_sdcard_file_root_sectors, BANKED
	swapf	PRODH, W
	andlw	0xf0
	iorwf	_sdcard_file_root_sectors, F, BANKED

	;; calculate address to cluster 2 (start of data)
	movf	_sdcard_file_sector_root+0, W, BANKED
	addwf	_sdcard_file_root_sectors+0, W, BANKED
	movwf	_sdcard_file_sector_cl2+0, BANKED
	movf	_sdcard_file_sector_root+1, W, BANKED
	addwfc	_sdcard_file_root_sectors+1, W, BANKED
	movwf	_sdcard_file_sector_cl2+1, BANKED

	;; no error
	movlw	SDCARD_ERROR_NONE
SDCARD_FILE_Init_End
	andlw	0xff		; (fix ZERO flag)
	return

SDCARD_FILE_Init_SD_Error
	movlw	SDCARD_ERROR_SDC_ACCESS
	rgoto	SDCARD_FILE_Init_End



;; --------------------------------------------------------------------------
;;  Searches for file in root directory and returns the starting sector
;;  IN: variables, which have been set by SDCARD_FILE_Init
;;      mode in _sdcard_file_mode:
;;        bit 0: 0: ignore lower/upper case characters, 1: case sensitive search
;;        bit 1: 0: forward search, 1: backward search
;;      reference to filename (8+3 characters) in _sdcard_file_name_ptr[3]
;;      _sdcard_file_name_ptr+2.7 == 0: filename located in code memory, == 1: in RAM
;;        0xff characters used as wildcard
;;      number of dir entry from which we start to search in _sdcard_file_dir_entry_num
;;      (set to 0 to start from zero)
;;  OUT: error code in WREG
;;       cluster of file in _sdcard_file_cluster (2 bytes)
;;       sector of file in _sdcard_file_sector (4 bytes)
;;       number of directory entry in _sdcard_file_dir_entry_num (2 bytes)
;;       pointer to directory entry in _sdcard_file_dir_entry_ptr (3 bytes)
;;  USES: TMP[12345], PROD[LH], FSR1 and TABLAT
;; --------------------------------------------------------------------------
SDCARD_FILE_Open
	;; reference: http://home.teleport.com/~brainy/lfn.htm

	movff	FSR2L, TMP4	; store current FSR0[LH] into TMP[45] 
	movff	FSR2H, TMP5


	;; determine initial sector from dir entry number (16 entries per sector)
	;; TMP1 used as sector offset/counter
	SET_BSR	_sdcard_file_dir_entry_num
	swapf	_sdcard_file_dir_entry_num+0, W, BANKED
	andlw	0x0f
	movwf	TMP1
	swapf	_sdcard_file_dir_entry_num+1, W, BANKED
	andlw	0xf0
	iorwf	TMP1, F

	;; check if last sector already reached
	movf	_sdcard_file_root_sectors, W, BANKED
	cpfslt	TMP1, ACCESS
	rgoto	SDCARD_FILE_Open_DirLoop_Failed

SDCARD_FILE_Open_DirLoop
	;; feed watchdog
	clrwdt

	;; read root directory sector
	movf	_sdcard_file_sector_root+0, W, BANKED
	addwf	TMP1, W		; sector counter
	movff	WREG, _sdcard_address+0
	movlw	0x00
	addwfc	_sdcard_file_sector_root+1, W, BANKED
	SET_BSR	_sdcard_address
	movwf	_sdcard_address+1, BANKED
	clrf	_sdcard_address+2, BANKED
	clrf	_sdcard_address+3, BANKED
	call	SDCARD_SectorRead
	skpz
	rgoto	SDCARD_FILE_Open_SD_Error
	SET_BSR	_sdcard_file_name_ptr

SDCARD_FILE_Open_DirLoop_OL
	SET_BSR	_sdcard_file_name_ptr
	movlw	HIGH(_sdcard_buffer_p0)			; determine pointer to dir entry
	btfsc	_sdcard_file_dir_entry_num+0, 3, BANKED
	movlw	HIGH(_sdcard_buffer_p1)
	movwf	FSR1H
	movf	_sdcard_file_dir_entry_num+0, W, BANKED
	andlw	0x07
	mullw	0x20
	movff	PRODL, FSR1L

	movff	FSR1L, _sdcard_file_dir_entry_ptr+0	; store pointer to dir entry (used as return value)
	movff	FSR1H, _sdcard_file_dir_entry_ptr+1
	movlw	0x80					; (RAM pointer)
	movwf	_sdcard_file_dir_entry_ptr+2, BANKED

	movlw	0x1a					; ignore file if cluster size == 0
	movf	PLUSW1, W				; cluster located at dir entry offset 0x1a/0x1b
	bnz	SDCARD_FILE_Open_DirLoop_OLG
	movlw	0x1b
	movf	PLUSW1, W
	bz	SDCARD_FILE_Open_DirLoop_OLN
SDCARD_FILE_Open_DirLoop_OLG

	clrf	TMP2					; used as loop counter for filename
	movff	_sdcard_file_name_ptr+0, TBLPTRL	; get pointer to filename
	movff	_sdcard_file_name_ptr+1, TBLPTRH
	movff	_sdcard_file_name_ptr+2, TBLPTRU
	movff	_sdcard_file_name_ptr+0, FSR2L		; for the case that we are doing RAM accesses
	movff	_sdcard_file_name_ptr+1, FSR2H
SDCARD_FILE_Open_DirLoop_IL
	BRA_IFCLR _sdcard_file_name_ptr+2, 7, BANKED, SDCARD_FILE_Open_DirLoop_Fls
SDCARD_FILE_Open_DirLoop_RAM
	movff	POSTINC2, TABLAT			; read from RAM
	rgoto	SDCARD_FILE_Open_DirLoop_Cnt
SDCARD_FILE_Open_DirLoop_Fls
	tblrd*+						; read from Flash
SDCARD_FILE_Open_DirLoop_Cnt
	incf	TABLAT, W				; ignore if we read 0xff (used as wildcard)
	bz	SDCARD_FILE_Open_DirLoop_Nxt

	BRA_IFSET _sdcard_file_mode, SDCARD_FILE_MODE_CASE_SENSITIVE, BANKED, SDCARD_FILE_Open_DirLoop_Cs1
	movf	TABLAT, W				; ignore lower/upper case of characters
	andlw	0xe0					; 0x6x/0x7x -> 0x4x/0x5x
	xorlw	0x60
	bnz	SDCARD_FILE_Open_DirLoop_Cs1
	bcf	TABLAT, 5
SDCARD_FILE_Open_DirLoop_Cs1

	movf	TMP2, W					; compare with name of directory entry
	movf	PLUSW1, W
	BRA_IFSET _sdcard_file_mode, SDCARD_FILE_MODE_CASE_SENSITIVE, BANKED, SDCARD_FILE_Open_DirLoop_Cs2
	andlw	0xe0					; 0x6x/0x7x -> 0x4x/0x5x
	xorlw	0x60
	bnz	SDCARD_FILE_Open_DirLoop_C22
	movf	TMP2, W
	movf	PLUSW1, W
	bcf	WREG, 5
	rgoto	SDCARD_FILE_Open_DirLoop_Cs2
SDCARD_FILE_Open_DirLoop_C22
	movf	TMP2, W
	movf	PLUSW1, W
SDCARD_FILE_Open_DirLoop_Cs2

	xorwf	TABLAT, W
	bnz	SDCARD_FILE_Open_DirLoop_OLN	; failed - next name

SDCARD_FILE_Open_DirLoop_Nxt	
	incf	TMP2, F					; loop 8+3 times
	movlw	8+3
	xorwf	TMP2, W
	bnz	SDCARD_FILE_Open_DirLoop_IL

	rgoto	SDCARD_FILE_Open_DirLoop_Found		; file found!

SDCARD_FILE_Open_DirLoop_OLN
	;; loop until last file entry reached
	BRA_IFSET _sdcard_file_mode, SDCARD_FILE_MODE_BACKWARD_SEARCH, BANKED, SDCARD_FILE_Open_DirLoop_Bck
SDCARD_FILE_Open_DirLoop_Fwd
	incf	_sdcard_file_dir_entry_num+0, F, BANKED	; increment dir entry number until last entry reached
	skpnz
	incf	_sdcard_file_dir_entry_num+1, F, BANKED
	movf	_sdcard_file_dir_entry_num+0, W, BANKED
	andlw	0x0f
	bnz	SDCARD_FILE_Open_DirLoop_OL
	rgoto	SDCARD_FILE_Open_DirLoop_Next

SDCARD_FILE_Open_DirLoop_Bck
	decf	_sdcard_file_dir_entry_num+0, F, BANKED	; decrement dir entry number
	skpc
	decf	_sdcard_file_dir_entry_num+1, F, BANKED

	incf	_sdcard_file_dir_entry_num+0, W, BANKED	; loop 16 times
	andlw	0x0f					; (mask out 16 file entries)
	bz	SDCARD_FILE_Open_DirLoop_Next
	rgoto	SDCARD_FILE_Open_DirLoop_OL

SDCARD_FILE_Open_DirLoop_Next
	;; loop until last sector reached
	BRA_IFSET _sdcard_file_mode, SDCARD_FILE_MODE_BACKWARD_SEARCH, BANKED, SDCARD_FILE_Open_DirLoop_NextBck
SDCARD_FILE_Open_DirLoop_NextFwd
	incf	TMP1, F
	movf	_sdcard_file_root_sectors, W, BANKED
	cpfslt	TMP1, ACCESS
	rgoto	SDCARD_FILE_Open_DirLoop_Failed
	rgoto	SDCARD_FILE_Open_DirLoop

SDCARD_FILE_Open_DirLoop_NextBck
	movf	TMP1, W
	bz	SDCARD_FILE_Open_DirLoop_Failed
	decf	TMP1, F
	rgoto	SDCARD_FILE_Open_DirLoop

SDCARD_FILE_Open_DirLoop_Failed
	;; file not found
	movlw	SDCARD_ERROR_FILE_NOT_FOUND
	rgoto	SDCARD_FILE_Open_End



;; --------------------------------------------

SDCARD_FILE_Open_DirLoop_Found
	;; get starting cluster of file
	movlw	26
	movff	PLUSW1, _sdcard_file_cluster+0
	movlw	27
	movff	PLUSW1, _sdcard_file_cluster+1

	;; determine file sector:
	;; sector_cl2 + (file_cluster-2) * cluster_size;
	rcall	SDCARD_FILE_GetSector

	;; no error
	movlw	SDCARD_ERROR_NONE
SDCARD_FILE_Open_End
	movff	TMP4, FSR2L	; restore FSR2[LH] from TMP[45] 
	movff	TMP5, FSR2H
	andlw	0xff		; (fix ZERO flag)
	return

SDCARD_FILE_Open_SD_Error
	movlw	SDCARD_ERROR_SDC_ACCESS
	rgoto	SDCARD_FILE_Open_End


;; --------------------------------------------------------------------------
;;  Returns the sector of a file depending on given offset
;;  IN: offset in _sdcard_file_seek_offset (32bit value)
;;      first cluster of file in _sdcard_file_cluster (output of SDCARD_FILE_Open)
;;  OUT: error code in WREG
;;       sector of file in _sdcard_file_sector (4 bytes)
;;       cluster of file in _sdcard_file_seek_cluster (4 bytes)
;;  USES: PROD[LH], FSR1 and TABLAT
;; --------------------------------------------------------------------------
SDCARD_FILE_Seek
	SET_BSR	_sdcard_file_seek_cluster

	;; determine sector, temporary store it in _sdcard_file_seek_cluster
	movff	_sdcard_file_seek_offset+0, _sdcard_file_seek_cluster+0
	movff	_sdcard_file_seek_offset+1, _sdcard_file_seek_cluster+1
	movff	_sdcard_file_seek_offset+2, _sdcard_file_seek_cluster+2
	movff	_sdcard_file_seek_offset+3, _sdcard_file_seek_cluster+3
	movlw	9	; (each sector stores 512 bytes)
	rcall	SDCARD_FILE_Seek_ShiftCluster

	;; determine cluster number from given offset
	rcall	SDCARD_FILE_GetClusterShiftNum
	rcall	SDCARD_FILE_Seek_ShiftCluster

	;; using TMP[12] as cluster counter (FAT16 supports 2^16 clusters, therefore 16bit operations are sufficient here)
	movff	_sdcard_file_seek_cluster+0, TMP1
	movff	_sdcard_file_seek_cluster+1, TMP2

	;; if bit 17..31 still set, something went wrong
	movf	_sdcard_file_seek_cluster+2, W, BANKED
	bnz	SDCARD_FILE_Seek_ClusError
	movf	_sdcard_file_seek_cluster+3, W, BANKED
	bz	SDCARD_FILE_Seek_ClusOk
SDCARD_FILE_Seek_ClusError
	movlw	SDCARD_ERROR_SEEK_INVALID_CLUS
	rgoto	SDCARD_FILE_Seek_End
SDCARD_FILE_Seek_ClusOk

	;; determine absolute cluster number
	movff	_sdcard_file_cluster+0, _sdcard_file_seek_cluster+0
	movff	_sdcard_file_cluster+1, _sdcard_file_seek_cluster+1
	clrf	_sdcard_file_seek_cluster+2, BANKED ; (should already be 0, just to ensure...)
	clrf	_sdcard_file_seek_cluster+3, BANKED ; (should already be 0, just to ensure...)

	setf	TMP3		; for caching: memorize last FAT sector
SDCARD_FILE_Seek_ClusterLoop
	;; no FAT read required if counter is (or reached) 0
	movf	TMP1, W
	iorwf	TMP2, W
	bz	SDCARD_FILE_Seek_Cluster_Cont

	;; FAT16 contains 256 cluster entries per sector

	;; check if the sector already has been read
	movf	TMP3, W		; (contains last read FAT sector)
	xorwf	_sdcard_file_seek_cluster+1, W, BANKED
	bz	SDCARD_FILE_Seek_ClusterLoopNoRd

	;; memorize this sector number of next loop
	movff	_sdcard_file_seek_cluster+1, TMP3
	
	;; determine FAT sector
	movf	_sdcard_file_sector_fat+0, W, BANKED
	addwf	_sdcard_file_seek_cluster+1, W, BANKED ; (cluster >> 8)
	movff	WREG, _sdcard_address+0
	movlw	0x00
	addwfc	_sdcard_file_sector_fat+1, W, BANKED
	SET_BSR	_sdcard_address
	movwf	_sdcard_address+1, BANKED
	clrf	_sdcard_address+2, BANKED
	clrf	_sdcard_address+3, BANKED
	call	SDCARD_SectorRead
	skpz
	rgoto	SDCARD_FILE_Seek_SD_Error

SDCARD_FILE_Seek_ClusterLoopNoRd
	;; get next cluster from table (FAT entry = cluster >> 1)
	SET_BSR	_sdcard_file_seek_cluster
	movlw	HIGH(_sdcard_buffer_p0)			; determine pointer to dir entry
	btfsc	_sdcard_file_seek_cluster+0, 7, BANKED
	movlw	HIGH(_sdcard_buffer_p1)
	movwf	FSR1H
	clrc
	rlf	_sdcard_file_seek_cluster+0, W, BANKED
	movwf	FSR1L

	movff	POSTINC1, _sdcard_file_seek_cluster+0
	movff	POSTINC1, _sdcard_file_seek_cluster+1

#if 0
	;; for debugging: output FAT entry via MIDI
	movlw	0xf0
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_seek_cluster+1, WREG
	swapf	WREG, W
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_seek_cluster+1, WREG
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_seek_cluster+0, WREG
	swapf	WREG, W
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_seek_cluster+0, WREG
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movlw	0xf7
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	_sdcard_file_cluster
#endif

	;; check that cluster number >= 2
	movf	_sdcard_file_seek_cluster+1, W, BANKED
	bnz	SDCARD_FILE_Seek_Cluster_GE2Ok
	movlw	2
	cpfslt	_sdcard_file_seek_cluster+0, BANKED
	rgoto	SDCARD_FILE_Seek_Cluster_GE2Ok
SDCARD_FILE_Seek_Cluster_GE2Fail
	movlw	SDCARD_ERROR_SEEK_INVALID_FAT
	rgoto	SDCARD_FILE_Seek_End
SDCARD_FILE_Seek_Cluster_GE2Ok

	;; check that cluster number < 0xfff0
	;; MEMO:
	;; 0x0000: available cluster (not allocated, not expected here)
	;; 0x0002-0xffef: used, next cluster in file (good, expected here!)
	;; 0xfff0-0xfff6: reserved cluster (not expected here)
	;; 0xfff7: bad cluster (not expected here)
	;; 0xfff8-0xffff: used, last cluster in file (not expected here)
	movf	_sdcard_file_seek_cluster+1, W, BANKED
	xorlw	0xff
	bnz	SDCARD_FILE_Seek_Cluster_LTF0Ok
	movf	_sdcard_file_seek_cluster+0, W, BANKED
	andlw	0xf0
	xorlw	0xf0
	bnz	SDCARD_FILE_Seek_Cluster_LTF0Ok
SDCARD_FILE_Seek_Cluster_KTF0Fal
	movlw	SDCARD_ERROR_SEEK_INVALID_FAT
	btfsc	_sdcard_file_seek_cluster+0, 7, BANKED
	movlw	SDCARD_ERROR_SEEK_END_REACHED
	rgoto	SDCARD_FILE_Seek_End
SDCARD_FILE_Seek_Cluster_LTF0Ok

	;; decrement cluster counter and check next entry (if required)
	decf	TMP1, F
	skpc
	decf	TMP2, F
	rgoto	SDCARD_FILE_Seek_ClusterLoop
	

SDCARD_FILE_Seek_Cluster_Cont
	;; determine sector from seek cluster number
	rcall	SDCARD_FILE_GetSectorSeek

	;; add sector of seek offset (we assume 512 bytes per sector)
	rrf	_sdcard_file_seek_offset+2, W, BANKED	; (seek_offset rightshifted 9)
	rrf	_sdcard_file_seek_offset+1, W, BANKED	
	movwf	PRODL

	;; mask depends on number of sectors per cluster
	movf	_sdcard_file_cluster_size, W, BANKED
	bz	SDCARD_FILE_Seek_SectOff_Done
	addlw	-1
	andwf	PRODL, W

	addwf	_sdcard_file_sector+0, F, BANKED
	movlw	0x00
	addwfc	_sdcard_file_sector+1, F, BANKED
	movlw	0x00
	addwfc	_sdcard_file_sector+2, F, BANKED
	movlw	0x00
	addwfc	_sdcard_file_sector+3, F, BANKED
SDCARD_FILE_Seek_SectOff_Done

#if 0
	;; for debugging: output sector address via MIDI
	movlw	0xf0
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_sector+3, WREG
	swapf	WREG, W
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_sector+3, WREG
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_sector+2, WREG
	swapf	WREG, W
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_sector+2, WREG
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_sector+1, WREG
	swapf	WREG, W
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_sector+1, WREG
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_sector+0, WREG
	swapf	WREG, W
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movff	_sdcard_file_sector+0, WREG
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movlw	0xf7
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	_sdcard_file_cluster
#endif

	;; no error
	movlw	SDCARD_ERROR_NONE
SDCARD_FILE_Seek_End
	andlw	0xff		; (fix ZERO flag)
	return

SDCARD_FILE_Seek_SD_Error
	movlw	SDCARD_ERROR_SDC_ACCESS
	rgoto	SDCARD_FILE_Seek_End



;; --------------------------------------------------------------------------
;;  !Internal function!
;;  Returns the number of rightshifts which are required to determine the
;;  cluster number from a given sector number
;;  Cluster size must be a 2^n value - this requirement is checked in SDCARD_FILE_Init
;;  IN: cluster size in _sdcard_file_cluster_size
;;      already prepared BSR to _sdcard_file* variables
;;  OUT: n (number of shift operations)
;;  USES: -
;; --------------------------------------------------------------------------
SDCARD_FILE_GetClusterShiftNum
	btfsc	_sdcard_file_cluster_size, 7, BANKED
	retlw	0x07
	btfsc	_sdcard_file_cluster_size, 6, BANKED
	retlw	0x06
	btfsc	_sdcard_file_cluster_size, 5, BANKED
	retlw	0x05
	btfsc	_sdcard_file_cluster_size, 4, BANKED
	retlw	0x04
	btfsc	_sdcard_file_cluster_size, 3, BANKED
	retlw	0x03
	btfsc	_sdcard_file_cluster_size, 2, BANKED
	retlw	0x02
	btfsc	_sdcard_file_cluster_size, 1, BANKED
	retlw	0x01

	retlw	0x00

;; --------------------------------------------------------------------------
;;  !Internal function!
;;  Right-Shifts _sdcard_file_seek_cluster by the given number of shift operations
;;  IN: number of right-shift operations in WREG
;;      already prepared BSR to _sdcard_file* variables
;;  OUT: rightshifted _sdcard_file_seek_cluster
;;  USES: -
;; --------------------------------------------------------------------------
SDCARD_FILE_Seek_ShiftCluster
	andlw	0xff		; fix ZERO flag
	bz	SDCARD_FILE_Seek_ShiftClusterEnd
SDCARD_FILE_Seek_ShiftClusterLop
	clrc
	rrf	_sdcard_file_seek_cluster+3, F, BANKED
	rrf	_sdcard_file_seek_cluster+2, F, BANKED
	rrf	_sdcard_file_seek_cluster+1, F, BANKED
	rrf	_sdcard_file_seek_cluster+0, F, BANKED
	decfsz	WREG, F
	rgoto	SDCARD_FILE_Seek_ShiftClusterLop
SDCARD_FILE_Seek_ShiftClusterEnd
	return


;; --------------------------------------------------------------------------
;;  !Internal function!
;;  Determines file sector:
;;     sector_cl2 + (file_cluster-2) * cluster_size;
;;  IN: cluster number in _sdcard_file_cluster
;;      sector of cluster2 in _sdcard_file_sector_cl2
;;      already prepared BSR to _sdcard_file* variables
;;  OUT: sector number on _sdcard_file_sector
;;  USES: TMP[12] and PROD[LH]
;; --------------------------------------------------------------------------
SDCARD_FILE_GetSectorSeek
	;; subtract 2 from file_cluster_seek -> TMP[12]
	movlw	-2
	addwf	_sdcard_file_seek_cluster+0, W, BANKED
	movwf	TMP1
	movlw	0xff
	addwfc	_sdcard_file_seek_cluster+1, W, BANKED
	movwf	TMP2
	rgoto	SDCARD_FILE_GetSectorSeekCont

SDCARD_FILE_GetSector
	;; subtract 2 from file_cluster -> TMP[12]
	movlw	-2
	addwf	_sdcard_file_cluster+0, W, BANKED
	movwf	TMP1
	movlw	0xff
	addwfc	_sdcard_file_cluster+1, W, BANKED
	movwf	TMP2

SDCARD_FILE_GetSectorSeekCont
	;; multiply with cluster size (8x16 multiplication) -> _sdcard_file_sector
	movf	TMP1, W
	mulwf	_sdcard_file_cluster_size, BANKED
	movff	PRODL, _sdcard_file_sector+0
	movff	PRODH, _sdcard_file_sector+1

	movf	TMP2, W
	mulwf	_sdcard_file_cluster_size, BANKED
	movf	PRODL, W
	addwf	_sdcard_file_sector+1, F, BANKED
	movf	PRODH, W
	clrf	_sdcard_file_sector+2, BANKED
	addwfc	_sdcard_file_sector+2, F, BANKED
	clrf	_sdcard_file_sector+3, BANKED

	;; add cluster2 offset
	movf	_sdcard_file_sector_cl2+0, W
	addwf	_sdcard_file_sector+0, F, BANKED
	movf	_sdcard_file_sector_cl2+1, W
	addwfc	_sdcard_file_sector+1, F, BANKED
	movlw	0x00
	addwfc	_sdcard_file_sector+2, F, BANKED
	movlw	0x00
	addwfc	_sdcard_file_sector+3, F, BANKED
	return
