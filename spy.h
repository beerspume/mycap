#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>

#ifndef JOYFUL_SPY
#define JOYFUL_SPY

void spy_printf(const char *fmt, ...);
char* spy_S2T(time_t second);

#endif