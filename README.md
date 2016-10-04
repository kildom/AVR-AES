# AVR-AES

AES cipher implementation for 8-bit AVR microcontrollers

AVR-AES library can be used to encrypt and decrypt data with AES algorithm (Rijndael-128).
Key size is limited to 128 bits. Short description of library API is in header file. It is
configurable with definitions contained also in header file. 

The library contains different implementations of AES combined in single file. One
implementation can be selected by the configuration. Some of the implementations
are from [point-at-infinity.org/avraes/][pai] and some are designed specially for
AVR-AES. Each implementation have different speed, code size and memory usage.

   [pai]: <http://point-at-infinity.org/avraes/>
   