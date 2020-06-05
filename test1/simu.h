#ifndef _SIMU_H_
#define _SIMU_H_

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#define DAT GPIOR0
#define CMD GPIOR1

#define TIMERS(name, ...) do { timer_start(name); __VA_ARGS__; timer_stop(); } while (0)
#define COMPARE_EQ(name, x, y) do { send_string(name, CMD_NAME); simu_comp((x), (y), sizeof(x), true); } while (0)
#define COMPARE_NE(name, x, y) do { send_string(name, CMD_NAME); simu_comp((x), (y), sizeof(x), false); } while (0)

enum {
	CMD_TIMER_START = 1,
	CMD_TIMER_STOP = 2,
	CMD_NAME = 3,
	CMD_UINT = 4,
    CMD_STR = 5,
};

void simu_exit();
void send_string(const char* str, uint8_t cmd);
void send_data(const void* data, uint8_t size);
void simu_comp(const void* ptr1, const void* ptr2, uint16_t size, uint8_t exp);

static inline void timer_start(const char* name) { send_string(name, CMD_NAME); CMD = CMD_TIMER_START; }
static inline void timer_stop() { CMD = CMD_TIMER_STOP; }
static inline void value_u8(const char* name, uint8_t value) { send_string(name, CMD_NAME); DAT = value; CMD = CMD_UINT; }
static inline void value_u16(const char* name, uint16_t value) { send_string(name, CMD_NAME); send_data(&value, 2); CMD = CMD_UINT; }
static inline void value_u32(const char* name, uint32_t value) { send_string(name, CMD_NAME); send_data(&value, 4); CMD = CMD_UINT; }
static inline void value_str(const char* name, const char* value) { send_string(name, CMD_NAME); send_string(value, CMD_STR); }

#endif
