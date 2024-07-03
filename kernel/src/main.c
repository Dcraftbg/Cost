#include "../../config.h"
#include "serial.h"
#include <limine.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
void _start() {
    serial_init();
    serial_printstr("Hello, This is a simple template for making an OS in C for x86_64!\n");
    // Loop forever :|
    for(;;) {
        asm volatile("hlt");
    }
}
