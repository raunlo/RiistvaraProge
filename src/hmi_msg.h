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
#define ADDED_SUCCESSFULLY "Card added to list!\r\n"

#define MEMORY_FAILED "Memory operation failed!\r\n"

#define CARD_ADDING_FAILED "This card is already added\r\n"
#define UNABLE_SELECT_CARD "Unable to select card.\r\n"
#define CARD_SELECTED "Card selected! \r\n"
#define CARD_TYPE "Card type: "
#define CARD_UID "Card UID: "
#define CARD_SIZE "  (Size is: "
#define READ_CARD_END " )"

#define LIST_EMPTY "Card list is empty!\r\n"
#define NOT_FOUND "Card was not in list!"
#define RM_successfully "Card was deleted succsessfully!\r\n"

#define PRINT_HOLDER "  Holder name: "
#define PRINT_UID_START ". UID["
#define PRINT_UID_END "]: "
#define LINE_BREAK "\r\n"

#define FALSE_INPUT "To few or too many arguments for this command\r\n\tUse <help>\r\n"
#define COMMAND_NOT_IMPLEMENTED "Command not implemented.\r\n\tUse <help> to get help.\r\n"
#define IMPLEMENTED_COMMANDS "Implemented commands:\r\n"
#define COMMAND_ARGUMENTS "Command had following arguments:\r\n"

#define access_denied "access denied"



extern PGM_P const numbers[];
#endif /* header guard end*/
