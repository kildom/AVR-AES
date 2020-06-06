
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <avr_mcu_section.h>

#include "simu.h"


AVR_MCU(F_CPU, "atmega88");
AVR_MCU_VCD_FILE("aes_test.vcd", 2);
const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
	{ AVR_MCU_VCD_SYMBOL("SPH"), .what = (void*)&SPH, },
	{ AVR_MCU_VCD_SYMBOL("SPL"), .what = (void*)&SPL, },
	{ AVR_MCU_VCD_SYMBOL("DAT"), .what = (void*)&GPIOR0, },
	{ AVR_MCU_VCD_SYMBOL("CMD"), .what = (void*)&GPIOR1, },
};

void simuExit()
{
	cli();
	sleep_enable();
	sleep_cpu();
}

void simuDataStr(const char* str, uint8_t cmd)
{
	while (*str) {
		DAT = *str++;
	}
    CMD = cmd;
}

void simuDataRaw(const void* data, uint8_t size, uint8_t cmd)
{
    const uint8_t* ptr = (const uint8_t*)data;
	while (size) {
		DAT = *ptr++;
		size--;
	}
    CMD = cmd;
}

void simuCmp(const void* ptr1, const void* ptr2, uint16_t size, bool exp)
{
	bool ok = (memcmp(ptr1, ptr2, size) == 0) ? exp : !exp;
    DAT = ok ? 1 : 0;
    CMD = SIMU_CMD_UINT;
}
