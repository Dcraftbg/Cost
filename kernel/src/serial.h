#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "port.h"
void serial_init();
void serial_print_u8(uint8_t c);
void serial_printstr(const char* str);

