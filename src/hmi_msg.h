#include <avr/pgmspace.h>
#ifndef _HMI_MSG_H
#define _HMI_MSG_H

#define NOT_DECIMAL_CONSOLE "Argument is not decimal\r\n"
#define NOT_DECIMAL_LCD "Not a decimal"
#define VER_FW   "Version: " FW_VERSION " built on: " __DATE__ " " __TIME__ "\r\n"
#define VER_LIBC "avr-libc version: " __AVR_LIBC_VERSION_STRING__ " avr-gcc version: " __VERSION__ "\r\n"
#define REPLY_NUMBER "You entered number "
#define WRONG_NUMBER "Please enter number between 0 and 9!\r\n"
#define STUDENT_NAME_CONSOLE "Rauno Lõhmus"
#define STUDENT_NAME_LCD "Rauno Lohmus"



extern PGM_P const numbers[];
#endif /* header guard end*/
