#ifndef _SIMU_H_
#define _SIMU_H_

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#define DAT GPIOR0
#define CMD GPIOR1

#define TIMERS(name, ...) do { simuTimerStart(name); __VA_ARGS__; simuTimerStop(); } while (0)
#define CMP_EQ(name, x, y) do { simuDataStr(name, SIMU_CMD_NAME); simuCmp((x), (y), sizeof(x), true); } while (0)
#define CMP_NE(name, x, y) do { simuDataStr(name, SIMU_CMD_NAME); simuCmp((x), (y), sizeof(x), false); } while (0)

enum {
	SIMU_CMD_TIMER_START = 1,
	SIMU_CMD_TIMER_STOP = 2,
	SIMU_CMD_NAME = 3,
	SIMU_CMD_UINT = 4,
    SIMU_CMD_STR = 5,
};

void simuExit();
void simuDataStr(const char* str, uint8_t cmd);
void simuDataRaw(const void* data, uint8_t size, uint8_t cmd);
void simuCmp(const void* ptr1, const void* ptr2, uint16_t size, bool exp);

static inline void simuTimerStart(const char* name) { simuDataStr(name, SIMU_CMD_NAME); __asm__ volatile ("":::"memory"); CMD = SIMU_CMD_TIMER_START; __asm__ volatile ("":::"memory"); }
static inline void simuTimerStop() { __asm__ volatile ("":::"memory"); CMD = SIMU_CMD_TIMER_STOP; __asm__ volatile ("":::"memory"); }
static inline void simuMsgU8(const char* name, uint8_t value) { simuDataStr(name, SIMU_CMD_NAME); DAT = value; CMD = SIMU_CMD_UINT; }
static inline void simuMsgU16(const char* name, uint16_t value) { simuDataStr(name, SIMU_CMD_NAME); simuDataRaw(&value, 2, SIMU_CMD_UINT); }
static inline void simuMsgU32(const char* name, uint32_t value) { simuDataStr(name, SIMU_CMD_NAME); simuDataRaw(&value, 4, SIMU_CMD_UINT); }
static inline void simuMsgStr(const char* name, const char* value) { simuDataStr(name, SIMU_CMD_NAME); simuDataStr(value, SIMU_CMD_STR); }

#endif
