
//------------------------   KEY_EXPAND   ------------------------\\
	asm volatile (
		#define _H1 ""
		#define _H2 ""
		#define _H3 ""
		#define _ST11 ""
		#define _ST12 ""
		#define _ST13 ""
		#define _ST14 ""
		#define _ST21 ""
		#define _ST22 ""
		#define _ST23 ""
		#define _ST24 ""
		#define _ST31 ""
		#define _ST32 ""
		#define _ST33 ""
		#define _ST34 ""
		#define _ST41 ""
		#define _ST42 ""
		#define _ST43 ""
		#define _ST44 ""
		#define _Y ""
		#define _Z ""
		#define _ZH ""
		#define _ZL ""
		"ldi "_H1", 1		\n"
		"ldi "_H2", 0x1b		\n"
		"ldi "_ZH", hi8(aes_tab_sbox)		\n"
		"rjmp keyexp1		\n"
		"keyexp0:mov "_ZL", "_ST24"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST11", "_H3"		\n"
		"eor "_ST11", "_H1"		\n"
		"mov "_ZL", "_ST34"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST21", "_H3"		\n"
		"mov "_ZL", "_ST44"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST31", "_H3"		\n"
		"mov "_ZL", "_ST14"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST41", "_H3"		\n"
		"eor "_ST12", "_ST11"		\n"
		"eor "_ST22", "_ST21"		\n"
		"eor "_ST32", "_ST31"		\n"
		"eor "_ST42", "_ST41"		\n"
		"eor "_ST13", "_ST12"		\n"
		"eor "_ST23", "_ST22"		\n"
		"eor "_ST33", "_ST32"		\n"
		"eor "_ST43", "_ST42"		\n"
		"eor "_ST14", "_ST13"		\n"
		"eor "_ST24", "_ST23"		\n"
		"eor "_ST34", "_ST33"		\n"
		"eor "_ST44", "_ST43"		\n"
		"lsl "_H1"		\n"
		"brcc keyexp1		\n"
		"eor "_H1", "_H2"		\n"
		"keyexp1:st "_Y"+, "_ST11"		\n"
		"st "_Y"+, "_ST21"		\n"
		"st "_Y"+, "_ST31"		\n"
		"st "_Y"+, "_ST41"		\n"
		"st "_Y"+, "_ST12"		\n"
		"st "_Y"+, "_ST22"		\n"
		"st "_Y"+, "_ST32"		\n"
		"st "_Y"+, "_ST42"		\n"
		"st "_Y"+, "_ST13"		\n"
		"st "_Y"+, "_ST23"		\n"
		"st "_Y"+, "_ST33"		\n"
		"st "_Y"+, "_ST43"		\n"
		"st "_Y"+, "_ST14"		\n"
		"st "_Y"+, "_ST24"		\n"
		"st "_Y"+, "_ST34"		\n"
		"st "_Y"+, "_ST44"		\n"
		"cpi "_H1", 0x6c		\n"
		"brne keyexp0		\n"
		:
		:
		: "memory", _H1, _H2, _H3, _ST11, _ST12, _ST13, _ST14, _ST21, _ST22, _ST23, _ST24, _ST31, _ST32, _ST33, _ST34, _ST41, _ST42, _ST43, _ST44, _Y, _Z, _ZH, _ZL
		#undef _H1
		#undef _H2
		#undef _H3
		#undef _ST11
		#undef _ST12
		#undef _ST13
		#undef _ST14
		#undef _ST21
		#undef _ST22
		#undef _ST23
		#undef _ST24
		#undef _ST31
		#undef _ST32
		#undef _ST33
		#undef _ST34
		#undef _ST41
		#undef _ST42
		#undef _ST43
		#undef _ST44
		#undef _Y
		#undef _Z
		#undef _ZH
		#undef _ZL
	);


//------------------------   ENCRYPT   ------------------------\\
	asm volatile (
		#define _H1 ""
		#define _H2 ""
		#define _H3 ""
		#define _I ""
		#define _ST11 ""
		#define _ST12 ""
		#define _ST13 ""
		#define _ST14 ""
		#define _ST21 ""
		#define _ST22 ""
		#define _ST23 ""
		#define _ST24 ""
		#define _ST31 ""
		#define _ST32 ""
		#define _ST33 ""
		#define _ST34 ""
		#define _ST41 ""
		#define _ST42 ""
		#define _ST43 ""
		#define _ST44 ""
		#define _Y ""
		#define _Z ""
		#define _ZH ""
		#define _ZL ""
		"ldi "_I", 10		\n"
		"encryp1:rcall addroundkey		\n"		// AddRoundKey
		"ldi "_ZH", hi8(aes_tab_sbox)		\n"		// SubBytes + ShiftRows
		"mov "_ZL", "_ST11"		\n"
		"lpm "_ST11", "_Z"		\n"
		"mov "_ZL", "_ST12"		\n"
		"lpm "_ST12", "_Z"		\n"
		"mov "_ZL", "_ST13"		\n"
		"lpm "_ST13", "_Z"		\n"
		"mov "_ZL", "_ST14"		\n"
		"lpm "_ST14", "_Z"		\n"
		"mov "_H1", "_ST21"		\n"
		"mov "_ZL", "_ST22"		\n"
		"lpm "_ST21", "_Z"		\n"
		"mov "_ZL", "_ST23"		\n"
		"lpm "_ST22", "_Z"		\n"
		"mov "_ZL", "_ST24"		\n"
		"lpm "_ST23", "_Z"		\n"
		"mov "_ZL", "_H1"		\n"
		"lpm "_ST24", "_Z"		\n"
		"mov "_H1", "_ST31"		\n"
		"mov "_ZL", "_ST33"		\n"
		"lpm "_ST31", "_Z"		\n"
		"mov "_ZL", "_H1"		\n"
		"lpm "_ST33", "_Z"		\n"
		"mov "_H1", "_ST32"		\n"
		"mov "_ZL", "_ST34"		\n"
		"lpm "_ST32", "_Z"		\n"
		"mov "_ZL", "_H1"		\n"
		"lpm "_ST34", "_Z"		\n"
		"mov "_H1", "_ST44"		\n"
		"mov "_ZL", "_ST43"		\n"
		"lpm "_ST44", "_Z"		\n"
		"mov "_ZL", "_ST42"		\n"
		"lpm "_ST43", "_Z"		\n"
		"mov "_ZL", "_ST41"		\n"
		"lpm "_ST42", "_Z"		\n"
		"mov "_ZL", "_H1"		\n"
		"lpm "_ST41", "_Z"		\n"
		"dec "_I"		\n"
		"breq addroundkey		\n"		// AddRoundKey
		"rcall mixcolumns		\n"		// MixColumns
		"rjmp encryp1		\n"
		// ;; ***************************************************************************
		// ;;
		// ;; ADDROUNDKEY
		// ;; This routine adds a round key to the state matrix (AddRoundKey).
		// ;;
		// ;; Note: This routine is part of the encryption and decryption routines. You
		// ;; normally won't be interested in calling this routine directly.
		// ;;
		// ;; Parameters:
		// ;;         ST11-ST44:  128 bit state matrix
		// ;;         YH:YL:      pointer to ram location
		// ;; Touched registers:
		// ;;     ST11-ST41,H1,YH,YL
		"addroundkey:		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST11", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST21", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST31", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST41", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST12", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST22", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST32", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST42", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST13", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST23", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST33", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST43", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST14", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST24", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST34", "_H1"		\n"
		"ld "_H1", "_Y"+		\n"
		"eor "_ST44", "_H1"		\n"
		"ret		\n"
		// ;; ***************************************************************************
		// ;;
		// ;; MIXCOLUMNS
		// ;; This routine applies the MixColumns diffusion operator to the whole
		// ;; state matrix. The code is used for both encryption and decryption.
		// ;;
		// ;; Note: This routine is part of the encryption and decryption routines. You
		// ;; normally wont be interested in calling this routine directly.
		// ;;
		// ;; Parameters:
		// ;;         ST11-ST44:  128 bit state matrix
		// ;; Touched registers:
		// ;;     ST11-ST41,H1,H2,H3,ZH,ZL
		"mixcolumns:		\n"
		"ldi "_ZH", hi8(aes_tab_xtime)		\n"
		"mov "_H1", "_ST11"		\n"
		"eor "_H1", "_ST21"		\n"
		"eor "_H1", "_ST31"		\n"
		"eor "_H1", "_ST41"		\n"
		"mov "_H2", "_ST11"		\n"
		"mov "_H3", "_ST11"		\n"
		"eor "_H3", "_ST21"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST11", "_H3"		\n"
		"eor "_ST11", "_H1"		\n"
		"mov "_H3", "_ST21"		\n"
		"eor "_H3", "_ST31"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST21", "_H3"		\n"
		"eor "_ST21", "_H1"		\n"
		"mov "_H3", "_ST31"		\n"
		"eor "_H3", "_ST41"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST31", "_H3"		\n"
		"eor "_ST31", "_H1"		\n"
		"mov "_H3", "_ST41"		\n"
		"eor "_H3", "_H2"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST41", "_H3"		\n"
		"eor "_ST41", "_H1"		\n"
		"mov "_H1", "_ST12"		\n"
		"eor "_H1", "_ST22"		\n"
		"eor "_H1", "_ST32"		\n"
		"eor "_H1", "_ST42"		\n"
		"mov "_H2", "_ST12"		\n"
		"mov "_H3", "_ST12"		\n"
		"eor "_H3", "_ST22"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST12", "_H3"		\n"
		"eor "_ST12", "_H1"		\n"
		"mov "_H3", "_ST22"		\n"
		"eor "_H3", "_ST32"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST22", "_H3"		\n"
		"eor "_ST22", "_H1"		\n"
		"mov "_H3", "_ST32"		\n"
		"eor "_H3", "_ST42"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST32", "_H3"		\n"
		"eor "_ST32", "_H1"		\n"
		"mov "_H3", "_ST42"		\n"
		"eor "_H3", "_H2"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST42", "_H3"		\n"
		"eor "_ST42", "_H1"		\n"
		"mov "_H1", "_ST13"		\n"
		"eor "_H1", "_ST23"		\n"
		"eor "_H1", "_ST33"		\n"
		"eor "_H1", "_ST43"		\n"
		"mov "_H2", "_ST13"		\n"
		"mov "_H3", "_ST13"		\n"
		"eor "_H3", "_ST23"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST13", "_H3"		\n"
		"eor "_ST13", "_H1"		\n"
		"mov "_H3", "_ST23"		\n"
		"eor "_H3", "_ST33"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST23", "_H3"		\n"
		"eor "_ST23", "_H1"		\n"
		"mov "_H3", "_ST33"		\n"
		"eor "_H3", "_ST43"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST33", "_H3"		\n"
		"eor "_ST33", "_H1"		\n"
		"mov "_H3", "_ST43"		\n"
		"eor "_H3", "_H2"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST43", "_H3"		\n"
		"eor "_ST43", "_H1"		\n"
		"mov "_H1", "_ST14"		\n"
		"eor "_H1", "_ST24"		\n"
		"eor "_H1", "_ST34"		\n"
		"eor "_H1", "_ST44"		\n"
		"mov "_H2", "_ST14"		\n"
		"mov "_H3", "_ST14"		\n"
		"eor "_H3", "_ST24"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST14", "_H3"		\n"
		"eor "_ST14", "_H1"		\n"
		"mov "_H3", "_ST24"		\n"
		"eor "_H3", "_ST34"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST24", "_H3"		\n"
		"eor "_ST24", "_H1"		\n"
		"mov "_H3", "_ST34"		\n"
		"eor "_H3", "_ST44"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST34", "_H3"		\n"
		"eor "_ST34", "_H1"		\n"
		"mov "_H3", "_ST44"		\n"
		"eor "_H3", "_H2"		\n"
		"mov "_ZL", "_H3"		\n"
		"lpm "_H3", "_Z"		\n"
		"eor "_ST44", "_H3"		\n"
		"eor "_ST44", "_H1"		\n"
		:
		:
		: "memory", _H1, _H2, _H3, _I, _ST11, _ST12, _ST13, _ST14, _ST21, _ST22, _ST23, _ST24, _ST31, _ST32, _ST33, _ST34, _ST41, _ST42, _ST43, _ST44, _Y, _Z, _ZH, _ZL
		#undef _H1
		#undef _H2
		#undef _H3
		#undef _I
		#undef _ST11
		#undef _ST12
		#undef _ST13
		#undef _ST14
		#undef _ST21
		#undef _ST22
		#undef _ST23
		#undef _ST24
		#undef _ST31
		#undef _ST32
		#undef _ST33
		#undef _ST34
		#undef _ST41
		#undef _ST42
		#undef _ST43
		#undef _ST44
		#undef _Y
		#undef _Z
		#undef _ZH
		#undef _ZL
	);


//------------------------   DECRYPT   ------------------------\\
	asm volatile (
		#define _H1 ""
		#define _H2 ""
		#define _I ""
		#define _ST11 ""
		#define _ST12 ""
		#define _ST13 ""
		#define _ST14 ""
		#define _ST21 ""
		#define _ST22 ""
		#define _ST23 ""
		#define _ST24 ""
		#define _ST31 ""
		#define _ST32 ""
		#define _ST33 ""
		#define _ST34 ""
		#define _ST41 ""
		#define _ST42 ""
		#define _ST43 ""
		#define _ST44 ""
		#define _YL ""
		#define _Z ""
		#define _ZH ""
		#define _ZL ""
		"ldi "_I", 10		\n"
		"sbiw "_YL",16		\n"
		"rcall addroundkey		\n"		// AddRoundKey
		"sbiw "_YL",32		\n"
		"decryp1:ldi "_ZH", hi8(aes_tab_isbox)		\n"		// SubBytes + ShiftRows
		"mov "_ZL", "_ST11"		\n"
		"lpm "_ST11", "_Z"		\n"
		"mov "_ZL", "_ST12"		\n"
		"lpm "_ST12", "_Z"		\n"
		"mov "_ZL", "_ST13"		\n"
		"lpm "_ST13", "_Z"		\n"
		"mov "_ZL", "_ST14"		\n"
		"lpm "_ST14", "_Z"		\n"
		"mov "_H1", "_ST21"		\n"
		"mov "_ZL", "_ST24"		\n"
		"lpm "_ST21", "_Z"		\n"
		"mov "_ZL", "_ST23"		\n"
		"lpm "_ST24", "_Z"		\n"
		"mov "_ZL", "_ST22"		\n"
		"lpm "_ST23", "_Z"		\n"
		"mov "_ZL", "_H1"		\n"
		"lpm "_ST22", "_Z"		\n"
		"mov "_H1", "_ST33"		\n"
		"mov "_ZL", "_ST31"		\n"
		"lpm "_ST33", "_Z"		\n"
		"mov "_ZL", "_H1"		\n"
		"lpm "_ST31", "_Z"		\n"
		"mov "_H1", "_ST34"		\n"
		"mov "_ZL", "_ST32"		\n"
		"lpm "_ST34", "_Z"		\n"
		"mov "_ZL", "_H1"		\n"
		"lpm "_ST32", "_Z"		\n"
		"mov "_H1", "_ST41"		\n"
		"mov "_ZL", "_ST42"		\n"
		"lpm "_ST41", "_Z"		\n"
		"mov "_ZL", "_ST43"		\n"
		"lpm "_ST42", "_Z"		\n"
		"mov "_ZL", "_ST44"		\n"
		"lpm "_ST43", "_Z"		\n"
		"mov "_ZL", "_H1"		\n"
		"lpm "_ST44", "_Z"		\n"
		"rcall addroundkey		\n"		// AddRoundKey
		"sbiw "_YL",32		\n"
		"dec "_I"		\n"
		"brne \.+2		\n"
		"ret		\n"
		"ldi "_ZH", hi8(aes_tab_xtime)		\n"		// preprocessing to use MixColumns
		"mov "_ZL", "_ST11"		\n"
		"eor "_ZL", "_ST31"		\n"
		"lpm "_H2", "_Z"		\n"
		"mov "_ZL", "_H2"		\n"
		"lpm "_H1", "_Z"		\n"
		"mov "_ZL", "_ST21"		\n"
		"eor "_ZL", "_ST41"		\n"
		"lpm "_H2", "_Z"		\n"
		"mov "_ZL", "_H2"		\n"
		"lpm "_H2", "_Z"		\n"
		"eor "_ST11", "_H1"		\n"
		"eor "_ST21", "_H2"		\n"
		"eor "_ST31", "_H1"		\n"
		"eor "_ST41", "_H2"		\n"
		"mov "_ZL", "_ST12"		\n"
		"eor "_ZL", "_ST32"		\n"
		"lpm "_H2", "_Z"		\n"
		"mov "_ZL", "_H2"		\n"
		"lpm "_H1", "_Z"		\n"
		"mov "_ZL", "_ST22"		\n"
		"eor "_ZL", "_ST42"		\n"
		"lpm "_H2", "_Z"		\n"
		"mov "_ZL", "_H2"		\n"
		"lpm "_H2", "_Z"		\n"
		"eor "_ST12", "_H1"		\n"
		"eor "_ST22", "_H2"		\n"
		"eor "_ST32", "_H1"		\n"
		"eor "_ST42", "_H2"		\n"
		"mov "_ZL", "_ST13"		\n"
		"eor "_ZL", "_ST33"		\n"
		"lpm "_H2", "_Z"		\n"
		"mov "_ZL", "_H2"		\n"
		"lpm "_H1", "_Z"		\n"
		"mov "_ZL", "_ST23"		\n"
		"eor "_ZL", "_ST43"		\n"
		"lpm "_H2", "_Z"		\n"
		"mov "_ZL", "_H2"		\n"
		"lpm "_H2", "_Z"		\n"
		"eor "_ST13", "_H1"		\n"
		"eor "_ST23", "_H2"		\n"
		"eor "_ST33", "_H1"		\n"
		"eor "_ST43", "_H2"		\n"
		"mov "_ZL", "_ST14"		\n"
		"eor "_ZL", "_ST34"		\n"
		"lpm "_H2", "_Z"		\n"
		"mov "_ZL", "_H2"		\n"
		"lpm "_H1", "_Z"		\n"
		"mov "_ZL", "_ST24"		\n"
		"eor "_ZL", "_ST44"		\n"
		"lpm "_H2", "_Z"		\n"
		"mov "_ZL", "_H2"		\n"
		"lpm "_H2", "_Z"		\n"
		"eor "_ST14", "_H1"		\n"
		"eor "_ST24", "_H2"		\n"
		"eor "_ST34", "_H1"		\n"
		"eor "_ST44", "_H2"		\n"
		"rcall mixcolumns		\n"		// MixColumns
		"rjmp decryp1		\n"
		:
		:
		: "memory", _H1, _H2, _I, _ST11, _ST12, _ST13, _ST14, _ST21, _ST22, _ST23, _ST24, _ST31, _ST32, _ST33, _ST34, _ST41, _ST42, _ST43, _ST44, _YL, _Z, _ZH, _ZL
		#undef _H1
		#undef _H2
		#undef _I
		#undef _ST11
		#undef _ST12
		#undef _ST13
		#undef _ST14
		#undef _ST21
		#undef _ST22
		#undef _ST23
		#undef _ST24
		#undef _ST31
		#undef _ST32
		#undef _ST33
		#undef _ST34
		#undef _ST41
		#undef _ST42
		#undef _ST43
		#undef _ST44
		#undef _YL
		#undef _Z
		#undef _ZH
		#undef _ZL
	);


//------------------------   BOXES   ------------------------\\
	asm volatile (
		#define  ""
		"sbox:		\n"
		".db 0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76		\n"
		".db 0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0		\n"
		".db 0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15		\n"
		".db 0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75		\n"
		".db 0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84		\n"
		".db 0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf		\n"
		".db 0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8		\n"
		".db 0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2		\n"
		".db 0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73		\n"
		".db 0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb		\n"
		".db 0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79		\n"
		".db 0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08		\n"
		".db 0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a		\n"
		".db 0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e		\n"
		".db 0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf		\n"
		".db 0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16		\n"
		"isbox:		\n"
		".db 0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb		\n"
		".db 0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb		\n"
		".db 0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e		\n"
		".db 0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25		\n"
		".db 0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92		\n"
		".db 0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84		\n"
		".db 0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06		\n"
		".db 0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b		\n"
		".db 0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73		\n"
		".db 0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e		\n"
		".db 0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b		\n"
		".db 0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4		\n"
		".db 0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f		\n"
		".db 0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef		\n"
		".db 0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61		\n"
		".db 0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d		\n"
		"xtime:		\n"
		".db 0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e		\n"
		".db 0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e		\n"
		".db 0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e		\n"
		".db 0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e		\n"
		".db 0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e		\n"
		".db 0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe		\n"
		".db 0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde		\n"
		".db 0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe		\n"
		".db 0x1b,0x19,0x1f,0x1d,0x13,0x11,0x17,0x15,0x0b,0x09,0x0f,0x0d,0x03,0x01,0x07,0x05		\n"
		".db 0x3b,0x39,0x3f,0x3d,0x33,0x31,0x37,0x35,0x2b,0x29,0x2f,0x2d,0x23,0x21,0x27,0x25		\n"
		".db 0x5b,0x59,0x5f,0x5d,0x53,0x51,0x57,0x55,0x4b,0x49,0x4f,0x4d,0x43,0x41,0x47,0x45		\n"
		".db 0x7b,0x79,0x7f,0x7d,0x73,0x71,0x77,0x75,0x6b,0x69,0x6f,0x6d,0x63,0x61,0x67,0x65		\n"
		".db 0x9b,0x99,0x9f,0x9d,0x93,0x91,0x97,0x95,0x8b,0x89,0x8f,0x8d,0x83,0x81,0x87,0x85		\n"
		".db 0xbb,0xb9,0xbf,0xbd,0xb3,0xb1,0xb7,0xb5,0xab,0xa9,0xaf,0xad,0xa3,0xa1,0xa7,0xa5		\n"
		".db 0xdb,0xd9,0xdf,0xdd,0xd3,0xd1,0xd7,0xd5,0xcb,0xc9,0xcf,0xcd,0xc3,0xc1,0xc7,0xc5		\n"
		".db 0xfb,0xf9,0xff,0xfd,0xf3,0xf1,0xf7,0xf5,0xeb,0xe9,0xef,0xed,0xe3,0xe1,0xe7,0xe5		\n"
		:
		:
		: "memory", 
		#undef 
	);

