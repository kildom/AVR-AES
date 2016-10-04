
//------------------------   AESEncrypt128   ------------------------\\
	asm volatile (
		#define _H1 ""
		#define _H2 ""
		#define _Rcon ""
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
		#define _YL ""
		#define _Z ""
		#define _ZH ""
		#define _ZL ""
		"ldi "_Rcon", 1		\n"
		"AESEnc1:rcall AddRoundKey		\n"
		"rcall RAMIncKey128		\n"
		"rcall ShiftRowsSubBytes		\n"
		"cpi "_Rcon", 0x6c		\n"
		"breq AddRoundKey		\n"
		"rcall MixColumns		\n"
		"rjmp AESEnc1		\n"
		"AddRoundKey:		\n"		// Touched registers: ST11-ST44, H1
		"ld "_H1", "_Y"		\n"
		"eor "_ST11", "_H1"		\n"
		"ldd "_H1", "_Y"+1		\n"
		"eor "_ST21", "_H1"		\n"
		"ldd "_H1", "_Y"+2		\n"
		"eor "_ST31", "_H1"		\n"
		"ldd "_H1", "_Y"+3		\n"
		"eor "_ST41", "_H1"		\n"
		"ldd "_H1", "_Y"+4		\n"
		"eor "_ST12", "_H1"		\n"
		"ldd "_H1", "_Y"+5		\n"
		"eor "_ST22", "_H1"		\n"
		"ldd "_H1", "_Y"+6		\n"
		"eor "_ST32", "_H1"		\n"
		"ldd "_H1", "_Y"+7		\n"
		"eor "_ST42", "_H1"		\n"
		"ldd "_H1", "_Y"+8		\n"
		"eor "_ST13", "_H1"		\n"
		"ldd "_H1", "_Y"+9		\n"
		"eor "_ST23", "_H1"		\n"
		"ldd "_H1", "_Y"+10		\n"
		"eor "_ST33", "_H1"		\n"
		"ldd "_H1", "_Y"+11		\n"
		"eor "_ST43", "_H1"		\n"
		"ldd "_H1", "_Y"+12		\n"
		"eor "_ST14", "_H1"		\n"
		"ldd "_H1", "_Y"+13		\n"
		"eor "_ST24", "_H1"		\n"
		"ldd "_H1", "_Y"+14		\n"
		"eor "_ST34", "_H1"		\n"
		"ldd "_H1", "_Y"+15		\n"
		"eor "_ST44", "_H1"		\n"
		"ret		\n"
		"RAMIncKey128:		\n"		// Touched registers: Rcon, H1, H2, Z
		"ldi "_ZH", hi8(aes_tab_sbox)		\n"
		"ldd "_H2", "_Y"+12		\n"
		"ldd "_ZL", "_Y"+13		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ZL", "_Rcon"		\n"
		"lsl "_Rcon"		\n"
		"brcc \.+2		\n"
		"ldi "_Rcon", 0x1b		\n"
		"rcall RAMInc1		\n"
		"ldd "_ZL", "_Y"+13		\n"
		"lpm "_ZL", "_Z"		\n"
		"rcall RAMInc1		\n"
		"ldd "_ZL", "_Y"+13		\n"
		"lpm "_ZL", "_Z"		\n"
		"rcall RAMInc1		\n"
		"mov "_ZL", "_H2"		\n"
		"lpm "_ZL", "_Z"		\n"
		"rcall RAMInc1		\n"
		"sbiw "_YL", 4		\n"
		"ret		\n"
		"RAMInc1:ld "_H1", "_Y"		\n"
		"eor "_ZL", "_H1"		\n"
		"st "_Y"+, "_ZL"		\n"
		"ldd "_H1", "_Y"+3		\n"
		"eor "_ZL", "_H1"		\n"
		"std "_Y"+3, "_ZL"		\n"
		"ldd "_H1", "_Y"+7		\n"
		"eor "_ZL", "_H1"		\n"
		"std "_Y"+7, "_ZL"		\n"
		"ldd "_H1", "_Y"+11		\n"
		"eor "_ZL", "_H1"		\n"
		"std "_Y"+11, "_ZL"		\n"
		"ret		\n"
		"ShiftRowsSubBytes:		\n"		// Touched registers: ST11-ST44, H1, Z
		"ldi "_ZH", hi8(aes_tab_sbox)		\n"
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
		"ret		\n"
		"MixColumns:		\n"		// Touched registers: ST11-ST44, H1, H2, Z
		"ldi "_ZH", hi8(aes_tab_xtime)		\n"
		"mov "_H1", "_ST11"		\n"		// Tmp = a[0] ^ a[1] ^ a[2] ^ a[3]
		"eor "_H1", "_ST21"		\n"
		"mov "_ZL", "_H1"		\n"
		"eor "_H1", "_ST31"		\n"
		"eor "_H1", "_ST41"		\n"
		"mov "_H2", "_ST11"		\n"		// save a[0] for later use
		"lpm "_ZL", "_Z"		\n"		// Tm = xtime(a[0] ^ a[1])
		"eor "_ST11", "_ZL"		\n"		// a[0] ^= Tm ^ Tmp
		"eor "_ST11", "_H1"		\n"
		"mov "_ZL", "_ST21"		\n"		// Tm = xtime(a[1] ^ a[2])
		"eor "_ZL", "_ST31"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST21", "_ZL"		\n"		// a[1] ^= Tm ^ Tmp
		"eor "_ST21", "_H1"		\n"
		"mov "_ZL", "_ST31"		\n"		// Tm = xtime(a[2] ^ a[3])
		"eor "_ZL", "_ST41"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST31", "_ZL"		\n"		// a[2] ^= Tm ^ Tmp
		"eor "_ST31", "_H1"		\n"
		"mov "_ZL", "_ST41"		\n"		// Tm = xtime(a[3] ^ a[0])
		"eor "_ZL", "_H2"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST41", "_ZL"		\n"		// a[3] ^= Tm ^ Tmp
		"eor "_ST41", "_H1"		\n"
		"mov "_H1", "_ST12"		\n"
		"eor "_H1", "_ST22"		\n"
		"mov "_ZL", "_H1"		\n"
		"eor "_H1", "_ST32"		\n"
		"eor "_H1", "_ST42"		\n"
		"mov "_H2", "_ST12"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST12", "_ZL"		\n"
		"eor "_ST12", "_H1"		\n"
		"mov "_ZL", "_ST22"		\n"
		"eor "_ZL", "_ST32"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST22", "_ZL"		\n"
		"eor "_ST22", "_H1"		\n"
		"mov "_ZL", "_ST32"		\n"
		"eor "_ZL", "_ST42"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST32", "_ZL"		\n"
		"eor "_ST32", "_H1"		\n"
		"mov "_ZL", "_ST42"		\n"
		"eor "_ZL", "_H2"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST42", "_ZL"		\n"
		"eor "_ST42", "_H1"		\n"
		"mov "_H1", "_ST13"		\n"
		"eor "_H1", "_ST23"		\n"
		"mov "_ZL", "_H1"		\n"
		"eor "_H1", "_ST33"		\n"
		"eor "_H1", "_ST43"		\n"
		"mov "_H2", "_ST13"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST13", "_ZL"		\n"
		"eor "_ST13", "_H1"		\n"
		"mov "_ZL", "_ST23"		\n"
		"eor "_ZL", "_ST33"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST23", "_ZL"		\n"
		"eor "_ST23", "_H1"		\n"
		"mov "_ZL", "_ST33"		\n"
		"eor "_ZL", "_ST43"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST33", "_ZL"		\n"
		"eor "_ST33", "_H1"		\n"
		"mov "_ZL", "_ST43"		\n"
		"eor "_ZL", "_H2"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST43", "_ZL"		\n"
		"eor "_ST43", "_H1"		\n"
		"mov "_H1", "_ST14"		\n"
		"eor "_H1", "_ST24"		\n"
		"mov "_ZL", "_H1"		\n"
		"eor "_H1", "_ST34"		\n"
		"eor "_H1", "_ST44"		\n"
		"mov "_H2", "_ST14"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST14", "_ZL"		\n"
		"eor "_ST14", "_H1"		\n"
		"mov "_ZL", "_ST24"		\n"
		"eor "_ZL", "_ST34"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST24", "_ZL"		\n"
		"eor "_ST24", "_H1"		\n"
		"mov "_ZL", "_ST34"		\n"
		"eor "_ZL", "_ST44"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST34", "_ZL"		\n"
		"eor "_ST34", "_H1"		\n"
		"mov "_ZL", "_ST44"		\n"
		"eor "_ZL", "_H2"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST44", "_ZL"		\n"
		"eor "_ST44", "_H1"		\n"
		"ret		\n"
		:
		:
		: "memory", _H1, _H2, _Rcon, _ST11, _ST12, _ST13, _ST14, _ST21, _ST22, _ST23, _ST24, _ST31, _ST32, _ST33, _ST34, _ST41, _ST42, _ST43, _ST44, _Y, _YL, _Z, _ZH, _ZL
		#undef _H1
		#undef _H2
		#undef _Rcon
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
		#undef _YL
		#undef _Z
		#undef _ZH
		#undef _ZL
	);


//------------------------   AESKeyRewind:   ------------------------\\
	asm volatile (
		#define _Rcon ""
		"ldi "_Rcon", 0x36		\n"
		"AESKeyR:rcall RAMDecKey128		\n"
		"cpi "_Rcon", 0		\n"
		"brne AESKeyR		\n"
		"ret		\n"
		:
		:
		: "memory", _Rcon
		#undef _Rcon
	);


//------------------------   AESKeyDecPreprocess   ------------------------\\
	asm volatile (
		#define _Rcon ""
		"ldi "_Rcon", 1		\n"
		"AESKeyF:rcall RAMIncKey128		\n"
		"cpi "_Rcon", 0x6c		\n"
		"brne AESKeyF		\n"
		"ret		\n"
		:
		:
		: "memory", _Rcon
		#undef _Rcon
	);


//------------------------   AESDecrypt128   ------------------------\\
	asm volatile (
		#define _H1 ""
		#define _H2 ""
		#define _Rcon ""
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
		"ldi "_Rcon", 0x36		\n"
		"rcall AddRoundKey		\n"
		"rcall RAMDecKey128		\n"
		"rcall ShiftRowsSubBytesInverse		\n"
		"AESDec1:rcall AddRoundKey		\n"
		"rcall RAMDecKey128		\n"
		"rcall MixColumnsInverse		\n"
		"rcall ShiftRowsSubBytesInverse		\n"
		"cpi "_Rcon", 0		\n"
		"brne AESDec1		\n"
		"ShiftRowsSubBytesInverse:		\n"		// Touched registers: ST11-ST44, H1, Z
		"ldi "_ZH", hi8(aes_tab_isbox)		\n"
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
		"mov "_ZL", "_ST41"		\n"
		"lpm "_ST44", "_Z"		\n"
		"mov "_ZL", "_ST42"		\n"
		"lpm "_ST41", "_Z"		\n"
		"mov "_ZL", "_ST43"		\n"
		"lpm "_ST42", "_Z"		\n"
		"mov "_ZL", "_H1"		\n"
		"lpm "_ST43", "_Z"		\n"
		"ret		\n"
		"MixColumnsInverse:		\n"		// Touched registers: ST11-ST44, H1, H2, Z
		"ldi "_ZH", hi8(aes_tab_xtime)		\n"
		"mov "_ZL", "_ST11"		\n"		// u = xtime(xtime(a[0] ^ a[2]))
		"eor "_ZL", "_ST31"		\n"
		"lpm "_ZL", "_Z"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST11", "_ZL"		\n"		// a[0] ^= u
		"eor "_ST31", "_ZL"		\n"		// a[2] ^= u
		"mov "_ZL", "_ST21"		\n"		// v = xtime(xtime(a[1] ^ a[3]))
		"eor "_ZL", "_ST41"		\n"
		"lpm "_ZL", "_Z"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST21", "_ZL"		\n"		// a[1] ^= v
		"eor "_ST41", "_ZL"		\n"		// a[3] ^= v
		"mov "_ZL", "_ST12"		\n"
		"eor "_ZL", "_ST32"		\n"
		"lpm "_ZL", "_Z"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST12", "_ZL"		\n"
		"eor "_ST32", "_ZL"		\n"
		"mov "_ZL", "_ST22"		\n"
		"eor "_ZL", "_ST42"		\n"
		"lpm "_ZL", "_Z"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST22", "_ZL"		\n"
		"eor "_ST42", "_ZL"		\n"
		"mov "_ZL", "_ST13"		\n"
		"eor "_ZL", "_ST33"		\n"
		"lpm "_ZL", "_Z"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST13", "_ZL"		\n"
		"eor "_ST33", "_ZL"		\n"
		"mov "_ZL", "_ST23"		\n"
		"eor "_ZL", "_ST43"		\n"
		"lpm "_ZL", "_Z"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST23", "_ZL"		\n"
		"eor "_ST43", "_ZL"		\n"
		"mov "_ZL", "_ST14"		\n"
		"eor "_ZL", "_ST34"		\n"
		"lpm "_ZL", "_Z"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST14", "_ZL"		\n"
		"eor "_ST34", "_ZL"		\n"
		"mov "_ZL", "_ST24"		\n"
		"eor "_ZL", "_ST44"		\n"
		"lpm "_ZL", "_Z"		\n"
		"lpm "_ZL", "_Z"		\n"
		"eor "_ST24", "_ZL"		\n"
		"eor "_ST44", "_ZL"		\n"
		"RAMDecKey128:		\n"		// Touched registers: Rcon, H1, H2, Z
		"ldi "_ZH", hi8(aes_tab_sbox)		\n"
		"ldi "_H1", 4		\n"
		"RAMDec1:ldd "_ZL", "_Y"+12		\n"
		"ldd "_H2", "_Y"+8		\n"
		"eor "_ZL", "_H2"		\n"
		"std "_Y"+12, "_ZL"		\n"
		"ldd "_ZL", "_Y"+4		\n"
		"eor "_H2", "_ZL"		\n"
		"std "_Y"+8, "_H2"		\n"
		"ld "_H2", "_Y"+		\n"
		"eor "_ZL", "_H2"		\n"
		"std "_Y"+3, "_ZL"		\n"
		"dec "_H1"		\n"
		"brne RAMDec1		\n"
		"ldd "_ZL", "_Y"+8		\n"
		"lpm "_ZL", "_Z"		\n"
		"ld "_H1", -"_Y"		\n"
		"eor "_H1", "_ZL"		\n"
		"st "_Y", "_H1"		\n"
		"ldd "_ZL", "_Y"+12		\n"
		"lpm "_ZL", "_Z"		\n"
		"ld "_H1", -"_Y"		\n"
		"eor "_H1", "_ZL"		\n"
		"st "_Y", "_H1"		\n"
		"ldd "_ZL", "_Y"+12		\n"
		"lpm "_ZL", "_Z"		\n"
		"ld "_H1", -"_Y"		\n"
		"eor "_H1", "_ZL"		\n"
		"st "_Y", "_H1"		\n"
		"ldd "_ZL", "_Y"+12		\n"
		"lpm "_ZL", "_Z"		\n"
		"ld "_H1", -"_Y"		\n"
		"eor "_H1", "_ZL"		\n"
		"eor "_H1", "_Rcon"		\n"
		"st "_Y", "_H1"		\n"
		"lsr "_Rcon"		\n"
		"cpi "_Rcon", 0x0d		\n"
		"brne \.+2		\n"
		"ldi "_Rcon", 0x80		\n"
		"ret		\n"
		:
		:
		: "memory", _H1, _H2, _Rcon, _ST11, _ST12, _ST13, _ST14, _ST21, _ST22, _ST23, _ST24, _ST31, _ST32, _ST33, _ST34, _ST41, _ST42, _ST43, _ST44, _Y, _Z, _ZH, _ZL
		#undef _H1
		#undef _H2
		#undef _Rcon
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

