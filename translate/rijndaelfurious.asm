; Copyright (C) 2003,2006 B. Poettering
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
; This AES implementation was written in May 2003 by B. Poettering. It is 
; published under the terms of the GNU General Public License. If you need 
; AES code, but this license is unsuitable for your project, feel free to 
; contact me: avraes AT point-at-infinity.org


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;                            RijndaelFurious
;
; This is a microcontroller implementation of the Rijndael block cipher, better
; known as AES. The target device class is Atmel's AVR, a family of very fast 
; and very powerful flash MCUs, operating at clock rates up to 16 MHz,  
; executing one instruction per clock cycle (16 MIPS). The implementation 
; given here achieves an encryption rate of about 93 kByte per second (on a 
; 16MHz MCU). The decryption performs about 30% slower than encryption (typical 
; for Rijndael).
; 
; The implemented algorithm is restricted to block and key sizes of 128 bit. 
; Larger key sizes can be obtained by altering the key scheduling code, which
; should be easy. As the cipher's state is completely kept in registers
; (which are limited in number), the block size is not that easy to enlarge.
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
; encryption |    2739      |       5842        |       93465
; decryption |    3579      |       4471        |       71528
; 
; KEY SETUP TIME: 756 clock cycles
; 
; CODE SIZE
; instructions:  802 byte ( 401 words)
; sboxes:        768 byte ( 384 words) = 3 * 256 byte
; total:        1570 byte ( 785 words)
;
; RAM REQUIREMENTS
; 16 * 11 = 176 byte for each expanded key
;
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

	
	
;;; ***************************************************************************
;;; The Rijndael cipher acts on a so-called (128 bit) "state matrix", 
;;; represented here by the 4x4 state bytes ST11-ST44. To guarantee maximum
;;; performance on AVR MCUs, these bytes are kept in registers (defaulted to
;;; the 16 low order registers r0-r15, but this may be changed if required).
;;; 
;;; The implementation makes use of four auxiliary registers (H1-H3 and I),
;;; some of which must reside in the upper registers (r16-r31). In addition
;;; ramp-registers YH:YL and ZH:ZL are used.
;;;
;;; If the context *really* requires more registers than the remaining ones, 
;;; it seems promising to move the I-register to a (fixed) ram location. 
;;; In the time crititcal routines the I-value is rarely used, thus the 
;;; speed loss obtained by dropping it from the register file is acceptible. 

.include "m161def.inc"

.def ST11=r0
.def ST21=r1
.def ST31=r2
.def ST41=r3
.def ST12=r4
.def ST22=r5
.def ST32=r6
.def ST42=r7
.def ST13=r8
.def ST23=r9
.def ST33=r10
.def ST43=r11
.def ST14=r12
.def ST24=r13
.def ST34=r14
.def ST44=r15
.def H1=r16
.def H2=r17
.def H3=r18
.def I=r21


main:	cli			; initialize stack
	ldi r31,HIGH(RAMEND)
	out SPH,r31
	ldi r31,LOW(RAMEND)
	out SPL,r31

	ldi ZH, high(key<<1)	; load key into ST11-ST44
	ldi ZL, low(key<<1)
	lpm ST11, Z+
	lpm ST21, Z+
	lpm ST31, Z+
	lpm ST41, Z+
	lpm ST12, Z+
	lpm ST22, Z+
	lpm ST32, Z+
	lpm ST42, Z+
	lpm ST13, Z+
	lpm ST23, Z+
	lpm ST33, Z+
	lpm ST43, Z+
	lpm ST14, Z+
	lpm ST24, Z+
	lpm ST34, Z+
	lpm ST44, Z+
	
	ldi YH, $00		; expand key to the memory
	ldi YL, $60		; locations $60..$60+(16*11-1)
	rcall key_expand

	ldi ZH, high(text<<1)	; load plaintext into ST11-ST44
	ldi ZL, low(text<<1)
	lpm ST11, Z+
	lpm ST21, Z+
	lpm ST31, Z+
	lpm ST41, Z+
	lpm ST12, Z+
	lpm ST22, Z+
	lpm ST32, Z+
	lpm ST42, Z+
	lpm ST13, Z+
	lpm ST23, Z+
	lpm ST33, Z+
	lpm ST43, Z+
	lpm ST14, Z+
	lpm ST24, Z+
	lpm ST34, Z+
	lpm ST44, Z+

	; now the registers ST11-ST44 contain the plaintext given below
	
	ldi YH, 0		; initialize YH:YL to
	ldi YL, 0x60		; expanded key and call
	rcall encrypt		; encryption routine

	; now the registers ST11-ST44 contain the enciphered text

	ldi YH, high($60+16*11) ; initialize YH:YL to point BEHIND 
	ldi YL, low($60+16*11)	; decryption key material and
	rcall decrypt		; call decryption routine

	; now the registers ST11-ST44 contain the plaintext again

main0:	rjmp main0		; stop
	

text:
.db $32,$43,$f6,$a8,$88,$5a,$30,$8d,$31,$31,$98,$a2,$e0,$37,$07,$34
key:
.db $2b,$7e,$15,$16,$28,$ae,$d2,$a6,$ab,$f7,$15,$88,$09,$cf,$4f,$3c


;;; ***************************************************************************
;;; 
;;; KEY_EXPAND
;;; The following routine implements the Rijndael key expansion algorithm. The 
;;; caller supplies the 128 bit key in the registers ST11-ST44 and a pointer 
;;; in the YH:YL register pair. The key is expanded to the memory 
;;; positions [Y : Y+16*11-1]. Note: the key expansion is necessary for both
;;; encryption and decryption.
;;; 
;;; Parameters:
;;;     ST11-ST44:	the 128 bit key
;;;         YH:YL:	pointer to ram location
;;; Touched registers:
;;;     ST11-ST44,H1-H3,ZH,ZL,YH,YL
;;; Clock cycles:	756

key_expand:
;BEGIN_C KEY_EXPAND
	ldi H1, 1
	ldi H2, $1b
	ldi ZH, high(sbox<<1)
	rjmp keyexp1
keyexp0:mov ZL, ST24
	lpm H3, Z
	eor ST11, H3
	eor ST11, H1
	mov ZL, ST34
	lpm H3, Z
	eor ST21, H3
	mov ZL, ST44
	lpm H3, Z
	eor ST31, H3
	mov ZL, ST14
	lpm H3, Z
	eor ST41, H3
	eor ST12, ST11
	eor ST22, ST21
	eor ST32, ST31
	eor ST42, ST41
	eor ST13, ST12
	eor ST23, ST22
	eor ST33, ST32
	eor ST43, ST42
	eor ST14, ST13
	eor ST24, ST23
	eor ST34, ST33
	eor ST44, ST43
	lsl H1
	brcc keyexp1
	eor H1, H2
keyexp1:st Y+, ST11
	st Y+, ST21
	st Y+, ST31
	st Y+, ST41
	st Y+, ST12
	st Y+, ST22
	st Y+, ST32
	st Y+, ST42
	st Y+, ST13
	st Y+, ST23
	st Y+, ST33
	st Y+, ST43
	st Y+, ST14
	st Y+, ST24
	st Y+, ST34
	st Y+, ST44
	cpi H1, $6c
	brne keyexp0
;END_C
	ret


;;; ***************************************************************************
;;; 
;;; ENCRYPT 
;;; This routine encrypts a 128 bit plaintext block (supplied in ST11-ST44), 
;;; using an expanded key given in YH:YL. The resulting 128 bit ciphertext
;;; block is stored in ST11-ST44.
;;;
;;; Parameters:
;;;         YH:YL:	pointer to expanded key
;;;         ST11-ST44:  128 bit plaintext block
;;; Touched registers:
;;;     ST11-ST44,H1,H2,H3,I,ZH,ZL,YH,YL
;;; Clock cycles:	2739
		
encrypt:
;BEGIN_C ENCRYPT
	ldi I, 10
encryp1:rcall addroundkey	; AddRoundKey
	ldi ZH, high(sbox<<1)	; SubBytes + ShiftRows
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
	dec I
	breq addroundkey	; AddRoundKey
	rcall mixcolumns	; MixColumns
	rjmp encryp1


;;; ***************************************************************************
;;; 
;;; ADDROUNDKEY
;;; This routine adds a round key to the state matrix (AddRoundKey). 
;;;
;;; Note: This routine is part of the encryption and decryption routines. You
;;; normally won't be interested in calling this routine directly.
;;;
;;; Parameters:
;;;         ST11-ST44:  128 bit state matrix
;;;         YH:YL:      pointer to ram location
;;; Touched registers:
;;;     ST11-ST41,H1,YH,YL

addroundkey:
	ld H1, Y+
	eor ST11, H1
	ld H1, Y+
	eor ST21, H1
	ld H1, Y+
	eor ST31, H1
	ld H1, Y+
	eor ST41, H1
	ld H1, Y+
	eor ST12, H1
	ld H1, Y+
	eor ST22, H1
	ld H1, Y+
	eor ST32, H1
	ld H1, Y+
	eor ST42, H1
	ld H1, Y+
	eor ST13, H1
	ld H1, Y+
	eor ST23, H1
	ld H1, Y+
	eor ST33, H1
	ld H1, Y+
	eor ST43, H1
	ld H1, Y+
	eor ST14, H1
	ld H1, Y+
	eor ST24, H1
	ld H1, Y+
	eor ST34, H1
	ld H1, Y+
	eor ST44, H1
	ret


;;; ***************************************************************************
;;; 
;;; MIXCOLUMNS
;;; This routine applies the MixColumns diffusion operator to the whole 
;;; state matrix. The code is used for both encryption and decryption.
;;;
;;; Note: This routine is part of the encryption and decryption routines. You
;;; normally wont be interested in calling this routine directly.
;;;
;;; Parameters:
;;;         ST11-ST44:  128 bit state matrix
;;; Touched registers:
;;;     ST11-ST41,H1,H2,H3,ZH,ZL

mixcolumns:
	ldi ZH, high(xtime<<1)
	mov H1, ST11
	eor H1, ST21
	eor H1, ST31
	eor H1, ST41
	mov H2, ST11
	mov H3, ST11
	eor H3, ST21
	mov ZL, H3
	lpm H3, Z
	eor ST11, H3
	eor ST11, H1
	mov H3, ST21
	eor H3, ST31
	mov ZL, H3
	lpm H3, Z
	eor ST21, H3
	eor ST21, H1
	mov H3, ST31
	eor H3, ST41
	mov ZL, H3
	lpm H3, Z
	eor ST31, H3
	eor ST31, H1
	mov H3, ST41
	eor H3, H2
	mov ZL, H3
	lpm H3, Z
	eor ST41, H3
	eor ST41, H1
	
	mov H1, ST12
	eor H1, ST22
	eor H1, ST32
	eor H1, ST42
	mov H2, ST12
	mov H3, ST12
	eor H3, ST22
	mov ZL, H3
	lpm H3, Z
	eor ST12, H3
	eor ST12, H1
	mov H3, ST22
	eor H3, ST32
	mov ZL, H3
	lpm H3, Z
	eor ST22, H3
	eor ST22, H1
	mov H3, ST32
	eor H3, ST42
	mov ZL, H3
	lpm H3, Z
	eor ST32, H3
	eor ST32, H1
	mov H3, ST42
	eor H3, H2
	mov ZL, H3
	lpm H3, Z
	eor ST42, H3
	eor ST42, H1
	
	mov H1, ST13
	eor H1, ST23
	eor H1, ST33
	eor H1, ST43
	mov H2, ST13
	mov H3, ST13
	eor H3, ST23
	mov ZL, H3
	lpm H3, Z
	eor ST13, H3
	eor ST13, H1
	mov H3, ST23
	eor H3, ST33
	mov ZL, H3
	lpm H3, Z
	eor ST23, H3
	eor ST23, H1
	mov H3, ST33
	eor H3, ST43
	mov ZL, H3
	lpm H3, Z
	eor ST33, H3
	eor ST33, H1
	mov H3, ST43
	eor H3, H2
	mov ZL, H3
	lpm H3, Z
	eor ST43, H3
	eor ST43, H1
	
	mov H1, ST14
	eor H1, ST24
	eor H1, ST34
	eor H1, ST44
	mov H2, ST14
	mov H3, ST14
	eor H3, ST24
	mov ZL, H3
	lpm H3, Z
	eor ST14, H3
	eor ST14, H1
	mov H3, ST24
	eor H3, ST34
	mov ZL, H3
	lpm H3, Z
	eor ST24, H3
	eor ST24, H1
	mov H3, ST34
	eor H3, ST44
	mov ZL, H3
	lpm H3, Z
	eor ST34, H3
	eor ST34, H1
	mov H3, ST44
	eor H3, H2
	mov ZL, H3
	lpm H3, Z
	eor ST44, H3
	eor ST44, H1
;END_C
	ret

		
;;; ***************************************************************************
;;; 
;;; DECRYPT
;;; This routine decrypts a 128 bit ciphertext block (given in ST11-ST44), 
;;; using an expanded key supplied in the 16*11 memory locations BEFORE YH:YL
;;; (YH:YL points behind the last byte of the key material!). The resulting 128
;;; bit plaintext block is stored in ST11-ST44. 
;;;
;;; Parameters:
;;;         YH:YL:	pointer behind key
;;;         ST11-ST44:  128 bit ciphertext block
;;; Touched registers:
;;;     ST11-ST41,H1,I,ZH,ZL,YH,YL
;;; Clock cycles:	3579

decrypt:
;BEGIN_C DECRYPT
	ldi I, 10
	sbiw YL,16
	rcall addroundkey	; AddRoundKey
	sbiw YL,32
decryp1:ldi ZH, high(isbox<<1)	; SubBytes + ShiftRows
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
	mov H1, ST33
	mov ZL, ST31
	lpm ST33, Z
	mov ZL, H1
	lpm ST31, Z
	mov H1, ST34
	mov ZL, ST32
	lpm ST34, Z
	mov ZL, H1
	lpm ST32, Z
	mov H1, ST41
	mov ZL, ST42
	lpm ST41, Z
	mov ZL, ST43
	lpm ST42, Z
	mov ZL, ST44
	lpm ST43, Z
	mov ZL, H1
	lpm ST44, Z
	rcall addroundkey	; AddRoundKey
	sbiw YL,32
	dec I
	brne PC+2
	ret
	ldi ZH, high(xtime<<1)	; preprocessing to use MixColumns
	mov ZL, ST11
	eor ZL, ST31
	lpm H2, Z
	mov ZL, H2
	lpm H1, Z
	mov ZL, ST21
	eor ZL, ST41
	lpm H2, Z
	mov ZL, H2
	lpm H2, Z
	eor ST11, H1
	eor ST21, H2
	eor ST31, H1
	eor ST41, H2
	mov ZL, ST12
	eor ZL, ST32
	lpm H2, Z
	mov ZL, H2
	lpm H1, Z
	mov ZL, ST22
	eor ZL, ST42
	lpm H2, Z
	mov ZL, H2
	lpm H2, Z
	eor ST12, H1
	eor ST22, H2
	eor ST32, H1
	eor ST42, H2
	mov ZL, ST13
	eor ZL, ST33
	lpm H2, Z
	mov ZL, H2
	lpm H1, Z
	mov ZL, ST23
	eor ZL, ST43
	lpm H2, Z
	mov ZL, H2
	lpm H2, Z
	eor ST13, H1
	eor ST23, H2
	eor ST33, H1
	eor ST43, H2
	mov ZL, ST14
	eor ZL, ST34
	lpm H2, Z
	mov ZL, H2
	lpm H1, Z
	mov ZL, ST24
	eor ZL, ST44
	lpm H2, Z
	mov ZL, H2
	lpm H2, Z
	eor ST14, H1
	eor ST24, H2
	eor ST34, H1
	eor ST44, H2
	rcall mixcolumns	; MixColumns
	rjmp decryp1
;END_C

;;; ***************************************************************************
;;; 
;;; S-BOX and "xtime" tables
;;; Rijndael consists of a non-linear step in its rounds (called "sbox step"), 
;;; here implemented with two hard-coded lookup tables (the sbox itself and its
;;; inverse for decryption). To provide an implementation secure against power 
;;; analysis attacks, the polynomial multiplication in the MixColumns operation 
;;; is done via an auxiliary lookup table called xtime. See [1] for details.
;;;
;;; The three tables have to be aligned to a flash position with its lower 
;;; address byte equal to $00. In assembler syntax: low(sbox<<1) == 0.
;;; To ensure the proper alignment of the sboxes, the assembler directive
;;; .ORG is used (below the sboxes are defined to begin at $800). Note, that 
;;; any other address can be used as well, as long as the lower byte is equal 
;;; to $00.
;;;
;;; The order of the sboxes is totally arbitrary. They even do not have to be
;;; allocated in adjacent memory areas.
	
.CSEG	
.ORG $800
	
;BEGIN_C BOXES
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

;END_C
