/*
*	Copyright (c) 2011-2012, Dominik Kilian <kontakt@dominik.cc>
*	All rights reserved.
*	See license.txt for details.
*/

#ifndef aes_h
#define aes_h

/*
*	*** CONFIGURATION ***
*	Comment/uncomment following definition in order to
*	reconfigure AVR-AES module.
*/


/*
*	Module uses software SBOX - values are calculated by function
*	insted of getting them from program memory directly.
*/
//#define AES_SWSBOX


/*
*	Module uses software SBOX for inverse cipher
*/
//#define AES_SWINVSBOX


/*
*	Module implements forward cipher (aesCipher)
*/
#define AES_CIPHER


/*
*	Module implements inverse cipher (aesInvCipher)
*/
//#define AES_INVCIPHER


/*
*	Allow inline assembler.
*/
#define AES_USEASM


/*
*	*** MODULE API ***
*/


/*
*	Contains 128-bit key for both encryption and decrytption.
*/
extern unsigned char aesKey[16];


/*
*	In-place forward AES cipher.
*		state - plaintext on input, and ciphertext on output
*/
#ifdef AES_CIPHER
void aesCipher(unsigned char* state);
#endif

/*
*	In-place inverse AES cipher.
*		state -  ciphertext on input, and plaintext on output
*/
#ifdef AES_INVCIPHER
void aesInvCipher(unsigned char* state);
#endif


#endif
