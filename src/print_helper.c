#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "print_helper.h"
#include "../lib/andygock_avr-uart/uart.h"
#include <avr/pgmspace.h>


/* Method to print ascii table*/
void print_ascii_tbl (void)
{
    for ( char c = ' '; c <= '~'; c++) {
        uart0_putc(c);
    }

    uart0_puts_p(PSTR("\r\n"));
}
/*viide: https://stackoverflow.com/questions/3408706/hexadecimal-string-to-byte-array-in-c/23898449#23898449
 Method to convert byte to uint8_t HEX */
char *bin2hex(unsigned char *p, int len)
{
    char *hex = malloc(((2 * len) + 1));
    char *r = hex;

    while (len && p) {
        (*r) = ((*p) & 0xF0) >> 4;
        (*r) = ((*r) <= 9 ? '0' + (*r) : 'A' - 10 + (*r));
        r++;
        (*r) = ((*p) & 0x0F);
        (*r) = ((*r) <= 9 ? '0' + (*r) : 'A' - 10 + (*r));
        r++;
        p++;
        len--;
    }

    *r = '\0';
    return hex;
}

/* Method to print human table and what isn't in asci table, will be printed in hex code*/
void print_for_human ( const unsigned char *array,
                       const size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (array[i] >= ' ' && array[i] <= '~') {
            uart0_putc(array[i]);
        } else {
            uart0_puts_p(PSTR("\"0X"));
            uart0_putc((array[i] >> 4) + ((array[i] >> 4) <= 9 ? 0x30 : 0x37));
            uart0_putc((array[i] & 0x0F) + ((array[i] & 0x0F) <= 9 ? 0x30 : 0x37));
            uart0_putc('"');
        }
    }

    uart0_puts_p(PSTR("\r\n"));
}

