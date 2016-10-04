; Copyright (C) 2006 B. Poettering
; 
; This program is free software; you can redistribute and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version. Whenever you redistribute a copy
; of this document, make sure to include the copyright and license
; agreement without modification.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
; The license text can be found here: http://www.gnu.org/licenses/gpl.txt

;                http://point-at-infinity.org/avraes/
;
; This AES implementation was written in November 2006 by B. Poettering.  
; It is published under the terms of the GNU General Public License. If 
; you need AES code, but this license is unsuitable for your project, 
; feel free to contact me: avraes AT point-at-infinity.org


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;                             RijndaelFantastic
;
; This is a microcontroller implementation of the Rijndael block cipher, better
; known as AES. The target device class is Atmel's AVR, a family of very fast 
; and very powerful flash MCUs, operating at clock rates up to 16 MHz,  
; executing one instruction per clock cycle (16 MIPS). The implementation 
; given here is optimized for RAM requirement, and achieves an encryption 
; rate of about 63 kByte/sec (on a 16MHz MCU). Decryption is done with
; 55 kByte/sec.
; 
; The implemented algorithm is restricted to block and key sizes of 128 bit. 
; Larger key sizes can be obtained by altering the key scheduling code, which
; should be easy. 
;
; This implementation makes extensive use of the AVR's "lpm" instruction,
; which loads data bytes from program memory at given addresses (the s-boxes 
; are realized that way). Some members of the AVR family don't offer that 
; instruction at all (e.g. AT90S1200), others only in a restricted way 
; (forcing the target register to be r0). The code below requires the least 
; restricted lpm instruction (with free choice of the target register).
; The ATmega161 devices meet the above mentioned requirements.
; 
; Statistics: 
; 
; 16 MHz MCU | clock cycles | blocks per second | bytes per second
; -----------+--------------+-------------------+------------------
; encryption |    4059      |      3942         |      63070
; decryption |    4675      |      3422         |      54759
;
; (key preprocessing time is not considered)
; 
; This source code consists of some routines and an example application, 
; which encrypts a certain plaintext and decrypts it afterwards with the
; same key. Comments in the code clarify the interaction between the key 
; expansion and the encryption/decryption routines.
;
; I encourage to read the following Rijndael-related papers/books/sites:
; [1] "The Design of Rijndael", Daemen & Rijmen, Springer, ISBN 3-540-42580-2
; [2] http://www.esat.kuleuven.ac.be/~rijmen/rijndael/
; [3] http://www.esat.kuleuven.ac.be/~rijmen/rijndael/rijndaeldocV2.zip
; [4] http://www.esat.kuleuven.ac.be/~rijmen/rijndael/atmal.zip
; [5] http://csrc.nist.gov/CryptoToolkit/aes/rijndael/
;
; [1] is *the* book about Rijndael, [2] is the official Rijndael homepage,
; [3] contains the complete Rijndael AES specification, [4] is another
; Rijndael-implementation for AVR MCUs (but much slower than this one, 
; taking 3815 clock cycles per encryption), [5] is the official NIST AES 
; site with further links.
;
; AVR and ATmega are registered trademarks by the ATMEL corporation.
; See http://www.atmel.com and http://www.atmel.com/products/avr/ for
; further details.
	

.include "m161def.inc"

.def H1		= r16
.def H2		= r17
.def Rcon	= r18
	
main:	cli			; initialize stack
	ldi r31,high(RAMEND)
	out SPH,r31
	ldi r31,low(RAMEND)
	out SPL,r31

	ldi ZH, high(key<<1)	; load key to RAM position $0060
	ldi ZL, low(key<<1)
	ldi YH, high($0060)
	ldi YL, low($0060)
main0:	lpm r16, Z+
	st Y+, r16
	cpi YL, low($0060+16)
	brne main0

	ldi ZH, high(text<<1)	; load plaintext to r0-r15
	ldi ZL, low(text<<1)
	lpm r0, Z+
	lpm r1, Z+
	lpm r2, Z+
	lpm r3, Z+
	lpm r4, Z+
	lpm r5, Z+
	lpm r6, Z+
	lpm r7, Z+
	lpm r8, Z+
	lpm r9, Z+
	lpm r10, Z+
	lpm r11, Z+
	lpm r12, Z+
	lpm r13, Z+
	lpm r14, Z+
	lpm r15, Z+

	ldi YH, high($0060)	; encrypt the plaintext
	ldi YL, low($0060)
	rcall AESEncrypt128
	rcall AESKeyRewind

	ldi YH, high($0060)	; prepare key for decryption
	ldi YL, low($0060)
	rcall AESKeyDecPreprocess
	rcall AESDecrypt128	; decrypt the cipher text
	
main1:	rjmp main1		; stop

text:
.db $32,$43,$f6,$a8,$88,$5a,$30,$8d,$31,$31,$98,$a2,$e0,$37,$07,$34
key:
.db $2b,$7e,$15,$16,$28,$ae,$d2,$a6,$ab,$f7,$15,$88,$09,$cf,$4f,$3c


;******************************************************************************
; Encrypt the 16 byte block defined by r0-r15 under the 128 bit key [Y].
; Note that calling this function modifies [Y]. Therefore, before encrypting a 
; second block with the same key [Y] has to be restored. This can be done by
; calling the function AESKeyRewind, but it is faster to simply backup the 
; original key somewhere to RAM and to restore it after calling AESEncrypt128.
;
; Touched registers: Rcon, H1, H2, Z
;
AESEncrypt128:
;BEGIN_C AESEncrypt128
	ldi Rcon, 1
AESEnc1:rcall AddRoundKey
	rcall RAMIncKey128
	rcall ShiftRowsSubBytes
	cpi Rcon, 0x6c
	breq AddRoundKey
	rcall MixColumns
	rjmp AESEnc1


AddRoundKey:	; Touched registers: ST11-ST44, H1
	ld H1, Y
	eor ST11, H1
	ldd H1, Y+1
	eor ST21, H1
	ldd H1, Y+2
	eor ST31, H1
	ldd H1, Y+3
	eor ST41, H1
	ldd H1, Y+4
	eor ST12, H1
	ldd H1, Y+5
	eor ST22, H1
	ldd H1, Y+6
	eor ST32, H1
	ldd H1, Y+7
	eor ST42, H1
	ldd H1, Y+8
	eor ST13, H1
	ldd H1, Y+9
	eor ST23, H1
	ldd H1, Y+10
	eor ST33, H1
	ldd H1, Y+11
	eor ST43, H1
	ldd H1, Y+12
	eor ST14, H1
	ldd H1, Y+13
	eor ST24, H1
	ldd H1, Y+14
	eor ST34, H1
	ldd H1, Y+15
	eor ST44, H1
	ret

	
RAMIncKey128:	; Touched registers: Rcon, H1, H2, Z
	ldi ZH, high(sbox<<1)
	ldd H2, Y+12
	ldd ZL, Y+13
	lpm ZL, Z
	eor ZL, Rcon
	lsl Rcon
	brcc PC+2
	ldi Rcon, 0x1b
	rcall RAMInc1
	ldd ZL, Y+13
	lpm ZL, Z
	rcall RAMInc1
	ldd ZL, Y+13
	lpm ZL, Z
	rcall RAMInc1
	mov ZL, H2
	lpm ZL, Z
	rcall RAMInc1
	sbiw YL, 4
	ret
RAMInc1:ld H1, Y
 	eor ZL, H1
 	st Y+, ZL
 	ldd H1, Y+3
 	eor ZL, H1
 	std Y+3, ZL
 	ldd H1, Y+7
 	eor ZL, H1
 	std Y+7, ZL
 	ldd H1, Y+11
 	eor ZL, H1
 	std Y+11, ZL
 	ret

	
ShiftRowsSubBytes:	; Touched registers: ST11-ST44, H1, Z
	ldi ZH, high(sbox<<1)
	mov ZL, ST11
	lpm ST11, Z
	mov ZL, ST12
	lpm ST12, Z
	mov ZL, ST13
	lpm ST13, Z
	mov ZL, ST14
	lpm ST14, Z
	mov H1, ST21
	mov ZL, ST22
	lpm ST21, Z
	mov ZL, ST23
	lpm ST22, Z
	mov ZL, ST24
	lpm ST23, Z
	mov ZL, H1
	lpm ST24, Z
	mov H1, ST31
	mov ZL, ST33
	lpm ST31, Z
	mov ZL, H1
	lpm ST33, Z
	mov H1, ST32
	mov ZL, ST34
	lpm ST32, Z
	mov ZL, H1
	lpm ST34, Z
	mov H1, ST44
	mov ZL, ST43
	lpm ST44, Z
	mov ZL, ST42
	lpm ST43, Z
	mov ZL, ST41
	lpm ST42, Z
	mov ZL, H1
	lpm ST41, Z
	ret	

MixColumns:	; Touched registers: ST11-ST44, H1, H2, Z
	ldi ZH, high(xtime<<1)
	mov H1, ST11		; Tmp = a[0] ^ a[1] ^ a[2] ^ a[3]
	eor H1, ST21
	mov ZL, H1
	eor H1, ST31
	eor H1, ST41
	mov H2, ST11		; save a[0] for later use
	lpm ZL, Z		; Tm = xtime(a[0] ^ a[1])
	eor ST11, ZL		; a[0] ^= Tm ^ Tmp
	eor ST11, H1
	mov ZL, ST21		; Tm = xtime(a[1] ^ a[2])
	eor ZL, ST31
	lpm ZL, Z
	eor ST21, ZL		; a[1] ^= Tm ^ Tmp
	eor ST21, H1
	mov ZL, ST31		; Tm = xtime(a[2] ^ a[3])
	eor ZL, ST41
	lpm ZL, Z
	eor ST31, ZL		; a[2] ^= Tm ^ Tmp
	eor ST31, H1
	mov ZL, ST41		; Tm = xtime(a[3] ^ a[0])
	eor ZL, H2
	lpm ZL, Z
	eor ST41, ZL		; a[3] ^= Tm ^ Tmp
	eor ST41, H1
	
	mov H1, ST12
	eor H1, ST22
	mov ZL, H1
	eor H1, ST32
	eor H1, ST42
	mov H2, ST12
	lpm ZL, Z
	eor ST12, ZL
	eor ST12, H1
	mov ZL, ST22
	eor ZL, ST32
	lpm ZL, Z
	eor ST22, ZL
	eor ST22, H1
	mov ZL, ST32
	eor ZL, ST42
	lpm ZL, Z
	eor ST32, ZL
	eor ST32, H1
	mov ZL, ST42
	eor ZL, H2
	lpm ZL, Z
	eor ST42, ZL
	eor ST42, H1
	
	mov H1, ST13
	eor H1, ST23
	mov ZL, H1
	eor H1, ST33
	eor H1, ST43
	mov H2, ST13
	lpm ZL, Z
	eor ST13, ZL
	eor ST13, H1
	mov ZL, ST23
	eor ZL, ST33
	lpm ZL, Z
	eor ST23, ZL
	eor ST23, H1
	mov ZL, ST33
	eor ZL, ST43
	lpm ZL, Z
	eor ST33, ZL
	eor ST33, H1
	mov ZL, ST43
	eor ZL, H2
	lpm ZL, Z
	eor ST43, ZL
	eor ST43, H1

	mov H1, ST14
	eor H1, ST24
	mov ZL, H1
	eor H1, ST34
	eor H1, ST44
	mov H2, ST14
	lpm ZL, Z
	eor ST14, ZL
	eor ST14, H1
	mov ZL, ST24
	eor ZL, ST34
	lpm ZL, Z
	eor ST24, ZL
	eor ST24, H1
	mov ZL, ST34
	eor ZL, ST44
	lpm ZL, Z
	eor ST34, ZL
	eor ST34, H1
	mov ZL, ST44
	eor ZL, H2
	lpm ZL, Z
	eor ST44, ZL
	eor ST44, H1
	ret
	

;END_C




;******************************************************************************
; Rewind the key given in [Y]. See AESEncrypt128 for more details.
;
; Touched registers: Rcon, H1, H2, Z
;
AESKeyRewind:
;BEGIN_C AESKeyRewind:
	ldi Rcon, 0x36
AESKeyR:rcall RAMDecKey128
	cpi Rcon, 0
	brne AESKeyR
	ret
;END_C

	
;******************************************************************************
; Preprocess the key given in [Y] for use for decryption. See AESDecrypt128
; for more details.
;
; Touched registers: Rcon, H1, H2, Z
;
AESKeyDecPreprocess:
;BEGIN_C AESKeyDecPreprocess
	ldi Rcon, 1
AESKeyF:rcall RAMIncKey128
	cpi Rcon, 0x6c
	brne AESKeyF
	ret
;END_C

		
;******************************************************************************
; Decrypt the 16 byte block defined by r0-r15 under the 128 bit key [Y].
; The decryption key has to be preprocessed by AESKeyDecPreprocess before
; calling this funtion. Like in AESEncrypt128 [Y] is modified by this 
; function, but the key can be restored by calling AESKeyDecPreprocess. Again,
; backing up the key to RAM will be faster.
;
; Note that AESKeyRewind and AESKeyDecPreprocess are the inverses of each
; other. In other words: if encryption and decryption are performed in 
; strictly alternating order, the calls to AESKeyRewind and AESKeyPreprocess
; can be ommitted.
;
; Touched registers: Rcon, H1, H2, Z
;
AESDecrypt128:
;BEGIN_C AESDecrypt128
	ldi Rcon, 0x36
	rcall AddRoundKey
	rcall RAMDecKey128
	rcall ShiftRowsSubBytesInverse
AESDec1:rcall AddRoundKey
	rcall RAMDecKey128
	rcall MixColumnsInverse
	rcall ShiftRowsSubBytesInverse
	cpi Rcon, 0
	brne AESDec1

ShiftRowsSubBytesInverse:	; Touched registers: ST11-ST44, H1, Z
	ldi ZH, high(isbox<<1)
	mov ZL, ST11
	lpm ST11, Z
	mov ZL, ST12
	lpm ST12, Z
	mov ZL, ST13
	lpm ST13, Z
	mov ZL, ST14
	lpm ST14, Z
	mov H1, ST21
	mov ZL, ST24
	lpm ST21, Z
	mov ZL, ST23
	lpm ST24, Z
	mov ZL, ST22
	lpm ST23, Z
	mov ZL, H1
	lpm ST22, Z
	mov H1, ST31
	mov ZL, ST33
	lpm ST31, Z
	mov ZL, H1
	lpm ST33, Z
	mov H1, ST32
	mov ZL, ST34
	lpm ST32, Z
	mov ZL, H1
	lpm ST34, Z
	mov H1, ST44
	mov ZL, ST41
	lpm ST44, Z
	mov ZL, ST42
	lpm ST41, Z
	mov ZL, ST43
	lpm ST42, Z
	mov ZL, H1
	lpm ST43, Z
	ret	



MixColumnsInverse:	; Touched registers: ST11-ST44, H1, H2, Z
	ldi ZH, high(xtime<<1)
	mov ZL, ST11		; u = xtime(xtime(a[0] ^ a[2]))
	eor ZL, ST31
	lpm ZL, Z
	lpm ZL, Z
	eor ST11, ZL		; a[0] ^= u
	eor ST31, ZL		; a[2] ^= u
	mov ZL, ST21		; v = xtime(xtime(a[1] ^ a[3]))
	eor ZL, ST41
	lpm ZL, Z
	lpm ZL, Z
	eor ST21, ZL		; a[1] ^= v
	eor ST41, ZL		; a[3] ^= v

	mov ZL, ST12
	eor ZL, ST32
	lpm ZL, Z
	lpm ZL, Z
	eor ST12, ZL
	eor ST32, ZL	
	mov ZL, ST22
	eor ZL, ST42
	lpm ZL, Z
	lpm ZL, Z
	eor ST22, ZL
	eor ST42, ZL	

	mov ZL, ST13
	eor ZL, ST33
	lpm ZL, Z
	lpm ZL, Z
	eor ST13, ZL
	eor ST33, ZL	
	mov ZL, ST23
	eor ZL, ST43
	lpm ZL, Z
	lpm ZL, Z
	eor ST23, ZL
	eor ST43, ZL	

	mov ZL, ST14
	eor ZL, ST34
	lpm ZL, Z
	lpm ZL, Z
	eor ST14, ZL
	eor ST34, ZL	
	mov ZL, ST24
	eor ZL, ST44
	lpm ZL, Z
	lpm ZL, Z
	eor ST24, ZL
	eor ST44, ZL	


RAMDecKey128:	; Touched registers: Rcon, H1, H2, Z
	ldi ZH, high(sbox<<1)
	ldi H1, 4
RAMDec1:ldd ZL, Y+12
	ldd H2, Y+8
	eor ZL, H2
	std Y+12, ZL
	ldd ZL, Y+4
	eor H2, ZL
	std Y+8, H2
	ld H2, Y+
	eor ZL, H2
	std Y+3, ZL
	dec H1
	brne RAMDec1
	ldd ZL, Y+8
	lpm ZL, Z
	ld H1, -Y
	eor H1, ZL
	st Y, H1
	ldd ZL, Y+12
	lpm ZL, Z
	ld H1, -Y
	eor H1, ZL
	st Y, H1
	ldd ZL, Y+12
	lpm ZL, Z
	ld H1, -Y
	eor H1, ZL
	st Y, H1
	ldd ZL, Y+12
	lpm ZL, Z
	ld H1, -Y
	eor H1, ZL
	eor H1, Rcon
	st Y, H1
	lsr Rcon
	cpi Rcon, 0x0d
	brne PC+2
	ldi Rcon, 0x80
	ret	

;END_C

;;; ***************************************************************************
;;; 
;;; SBOX and "xtime" tables
;;;
;;; The following tables have to be aligned to a flash position with lower 
;;; address byte equal to $00. In assembler syntax: low(sbox<<1) == 0. 
;;; To ensure the proper alignment the assembler directive .ORG should be  
;;; used. The order of the tables is arbitrary. They even do not have to be
;;; allocated in adjacent memory areas.
	
.CSEG	
.ORG $800			; ensure proper alignement
	
sbox:
.db $63,$7c,$77,$7b,$f2,$6b,$6f,$c5,$30,$01,$67,$2b,$fe,$d7,$ab,$76 
.db $ca,$82,$c9,$7d,$fa,$59,$47,$f0,$ad,$d4,$a2,$af,$9c,$a4,$72,$c0 
.db $b7,$fd,$93,$26,$36,$3f,$f7,$cc,$34,$a5,$e5,$f1,$71,$d8,$31,$15 
.db $04,$c7,$23,$c3,$18,$96,$05,$9a,$07,$12,$80,$e2,$eb,$27,$b2,$75 
.db $09,$83,$2c,$1a,$1b,$6e,$5a,$a0,$52,$3b,$d6,$b3,$29,$e3,$2f,$84 
.db $53,$d1,$00,$ed,$20,$fc,$b1,$5b,$6a,$cb,$be,$39,$4a,$4c,$58,$cf 
.db $d0,$ef,$aa,$fb,$43,$4d,$33,$85,$45,$f9,$02,$7f,$50,$3c,$9f,$a8 
.db $51,$a3,$40,$8f,$92,$9d,$38,$f5,$bc,$b6,$da,$21,$10,$ff,$f3,$d2 
.db $cd,$0c,$13,$ec,$5f,$97,$44,$17,$c4,$a7,$7e,$3d,$64,$5d,$19,$73 
.db $60,$81,$4f,$dc,$22,$2a,$90,$88,$46,$ee,$b8,$14,$de,$5e,$0b,$db 
.db $e0,$32,$3a,$0a,$49,$06,$24,$5c,$c2,$d3,$ac,$62,$91,$95,$e4,$79 
.db $e7,$c8,$37,$6d,$8d,$d5,$4e,$a9,$6c,$56,$f4,$ea,$65,$7a,$ae,$08 
.db $ba,$78,$25,$2e,$1c,$a6,$b4,$c6,$e8,$dd,$74,$1f,$4b,$bd,$8b,$8a 
.db $70,$3e,$b5,$66,$48,$03,$f6,$0e,$61,$35,$57,$b9,$86,$c1,$1d,$9e 
.db $e1,$f8,$98,$11,$69,$d9,$8e,$94,$9b,$1e,$87,$e9,$ce,$55,$28,$df 
.db $8c,$a1,$89,$0d,$bf,$e6,$42,$68,$41,$99,$2d,$0f,$b0,$54,$bb,$16 

isbox:
.db $52,$09,$6a,$d5,$30,$36,$a5,$38,$bf,$40,$a3,$9e,$81,$f3,$d7,$fb 
.db $7c,$e3,$39,$82,$9b,$2f,$ff,$87,$34,$8e,$43,$44,$c4,$de,$e9,$cb 
.db $54,$7b,$94,$32,$a6,$c2,$23,$3d,$ee,$4c,$95,$0b,$42,$fa,$c3,$4e 
.db $08,$2e,$a1,$66,$28,$d9,$24,$b2,$76,$5b,$a2,$49,$6d,$8b,$d1,$25 
.db $72,$f8,$f6,$64,$86,$68,$98,$16,$d4,$a4,$5c,$cc,$5d,$65,$b6,$92 
.db $6c,$70,$48,$50,$fd,$ed,$b9,$da,$5e,$15,$46,$57,$a7,$8d,$9d,$84 
.db $90,$d8,$ab,$00,$8c,$bc,$d3,$0a,$f7,$e4,$58,$05,$b8,$b3,$45,$06 
.db $d0,$2c,$1e,$8f,$ca,$3f,$0f,$02,$c1,$af,$bd,$03,$01,$13,$8a,$6b 
.db $3a,$91,$11,$41,$4f,$67,$dc,$ea,$97,$f2,$cf,$ce,$f0,$b4,$e6,$73 
.db $96,$ac,$74,$22,$e7,$ad,$35,$85,$e2,$f9,$37,$e8,$1c,$75,$df,$6e 
.db $47,$f1,$1a,$71,$1d,$29,$c5,$89,$6f,$b7,$62,$0e,$aa,$18,$be,$1b 
.db $fc,$56,$3e,$4b,$c6,$d2,$79,$20,$9a,$db,$c0,$fe,$78,$cd,$5a,$f4 
.db $1f,$dd,$a8,$33,$88,$07,$c7,$31,$b1,$12,$10,$59,$27,$80,$ec,$5f 
.db $60,$51,$7f,$a9,$19,$b5,$4a,$0d,$2d,$e5,$7a,$9f,$93,$c9,$9c,$ef 
.db $a0,$e0,$3b,$4d,$ae,$2a,$f5,$b0,$c8,$eb,$bb,$3c,$83,$53,$99,$61 
.db $17,$2b,$04,$7e,$ba,$77,$d6,$26,$e1,$69,$14,$63,$55,$21,$0c,$7d 

xtime:
.db $00,$02,$04,$06,$08,$0a,$0c,$0e,$10,$12,$14,$16,$18,$1a,$1c,$1e
.db $20,$22,$24,$26,$28,$2a,$2c,$2e,$30,$32,$34,$36,$38,$3a,$3c,$3e
.db $40,$42,$44,$46,$48,$4a,$4c,$4e,$50,$52,$54,$56,$58,$5a,$5c,$5e
.db $60,$62,$64,$66,$68,$6a,$6c,$6e,$70,$72,$74,$76,$78,$7a,$7c,$7e
.db $80,$82,$84,$86,$88,$8a,$8c,$8e,$90,$92,$94,$96,$98,$9a,$9c,$9e
.db $a0,$a2,$a4,$a6,$a8,$aa,$ac,$ae,$b0,$b2,$b4,$b6,$b8,$ba,$bc,$be
.db $c0,$c2,$c4,$c6,$c8,$ca,$cc,$ce,$d0,$d2,$d4,$d6,$d8,$da,$dc,$de
.db $e0,$e2,$e4,$e6,$e8,$ea,$ec,$ee,$f0,$f2,$f4,$f6,$f8,$fa,$fc,$fe
.db $1b,$19,$1f,$1d,$13,$11,$17,$15,$0b,$09,$0f,$0d,$03,$01,$07,$05
.db $3b,$39,$3f,$3d,$33,$31,$37,$35,$2b,$29,$2f,$2d,$23,$21,$27,$25
.db $5b,$59,$5f,$5d,$53,$51,$57,$55,$4b,$49,$4f,$4d,$43,$41,$47,$45
.db $7b,$79,$7f,$7d,$73,$71,$77,$75,$6b,$69,$6f,$6d,$63,$61,$67,$65
.db $9b,$99,$9f,$9d,$93,$91,$97,$95,$8b,$89,$8f,$8d,$83,$81,$87,$85
.db $bb,$b9,$bf,$bd,$b3,$b1,$b7,$b5,$ab,$a9,$af,$ad,$a3,$a1,$a7,$a5
.db $db,$d9,$df,$dd,$d3,$d1,$d7,$d5,$cb,$c9,$cf,$cd,$c3,$c1,$c7,$c5
.db $fb,$f9,$ff,$fd,$f3,$f1,$f7,$f5,$eb,$e9,$ef,$ed,$e3,$e1,$e7,$e5
