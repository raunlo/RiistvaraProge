#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/helius_microrl/microrl.h"
#include "hmi_msg.h"
#include "print_helper.h"
#include "../lib/matejx_avr_lib/mfrc522.h"
#include "rfid.h"
#include "../lib/andy_brown_memdebug/memdebug.h"

#define NUM_ELEMS(x)        (sizeof(x) / sizeof((x)[0]))
#define UART_STATUS_MASK    0x00FF

void cli_rfid_print_all_cards(void);
void cli_print_help(const char *const *argv);
void cli_example(const char *const *argv);
void cli_print_ver(const char *const *argv);
void cli_print_ascii_tbls(const char *const *argv);
void cli_handle_number(const char *const *argv);
void cli_rfid_read(void);
void cli_print_cmd_error(void);
void cli_print_cmd_arg_error(void);
void cli_rfid_add_card(const char *const *argv);
void cli_remove_card(const char *const *argv);
void cli_mem_stat(const char *const *argv);

typedef struct cli_cmd {
    PGM_P cmd;
    PGM_P help;
    void (*func_p)();
    const uint8_t func_argc;
} cli_cmd_t;

const char help_cmd[] PROGMEM = "help";
const char help_help[] PROGMEM = "Get help";
const char example_cmd[] PROGMEM = "example";
const char example_help[] PROGMEM =
    "Prints out all provided 3 arguments Usage: example <argument> <argument> <argument>";
const char ver_cmd[] PROGMEM = "version";
const char ver_help[] PROGMEM = "Print FW version";
const char ascii_cmd[] PROGMEM = "ascii";
const char ascii_help[] PROGMEM = "Print ASCII tables";
const char number_cmd[] PROGMEM = "number";
const char number_help[] PROGMEM =
    "Print and display matching number Usage: number <decimal number>";
const  char rfid_read_cmd[] PROGMEM = "read";
const char rfid_read_help [] PROGMEM = "Reads rfid card info";
const char rfid_rm_help[] PROGMEM =
    "Removes rfid card from list: Usage <UID in HEX>";
const char rfid_rm_cmd[] PROGMEM = "rm";
const char rfid_print_all_cards_help[] PROGMEM =
    "To print all cards, which are listed";
const char rfid_print_all_cards_cmd[] PROGMEM = "print";
const char rfid_add_card_help[] PROGMEM =
    "Add card to list. Usage <UID in HEX> <Holder name>";
const char rfid_add_card_cmd[] PROGMEM = "add";

const char mem_stat_cmd[] PROGMEM = "mem";
const char mem_stat_help[] PROGMEM =
    "Print memory usage and change compared to previous call";


const cli_cmd_t cli_cmds[] = {
    {help_cmd, help_help, cli_print_help, 0},
    {ver_cmd, ver_help, cli_print_ver, 0},
    {example_cmd, example_help, cli_example, 3},
    {ascii_cmd, ascii_help, cli_print_ascii_tbls, 0},
    {number_cmd, number_help, cli_handle_number, 1},
    {rfid_read_cmd, rfid_read_help, cli_rfid_read, 0},
    {rfid_add_card_cmd, rfid_add_card_help, cli_rfid_add_card, 2},
    {rfid_print_all_cards_cmd, rfid_print_all_cards_help, cli_rfid_print_all_cards, 0},
    {rfid_rm_cmd, rfid_rm_help, cli_remove_card, 1},
    {mem_stat_cmd, mem_stat_help, cli_mem_stat, 0}
};

/* cli command for printing out all commands and their meanings*/
void cli_print_help(const char *const *argv)
{
    (void) argv;
    uart0_puts_p(PSTR(IMPLEMENTED_COMMANDS));

    for (uint8_t i = 0; i < NUM_ELEMS(cli_cmds); i++) {
        uart0_puts_p(cli_cmds[i].cmd);
        uart0_puts_p(PSTR(" : "));
        uart0_puts_p(cli_cmds[i].help);
        uart0_puts_p(PSTR(LINE_BREAK));
    }
}

/* cli metod for example command */
void cli_example(const char *const *argv)
{
    uart0_puts_p(PSTR(COMMAND_ARGUMENTS));

    for (uint8_t i = 1; i < 4; i++) {
        uart0_puts(argv[i]);
        uart0_puts_p(PSTR(LINE_BREAK));
    }
}

void cli_print_ver(const char *const *argv)
{
    (void) argv;
    uart0_puts_p(PSTR(VER_FW));
    uart0_puts_p(PSTR(VER_LIBC));
}

void cli_print_ascii_tbls(const char *const *argv)
{
    (void) argv;
    unsigned char ascii[128] = {0};

    for (unsigned char i = 0; i < sizeof(ascii); i++) {
        ascii[i] = i;
    }

    print_ascii_tbl();
    print_for_human(ascii, sizeof(ascii));
}

void cli_handle_number(const char *const *argv)
{
    lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
    (void) argv;
    int input = atoi(argv[1]);
    lcd_goto(LCD_ROW_2_START);

    for (size_t i = 0; i < strlen(argv[1]); i++) {
        if (!isdigit(argv[1][i])) {
            uart0_puts_p(PSTR(NOT_DECIMAL_CONSOLE));
            lcd_puts_P(PSTR(NOT_DECIMAL_LCD));
            return;
        }
    }

    if (input >= 0 && input < 10) {
        uart0_puts_p(PSTR(REPLY_NUMBER));
        uart0_puts_p((PGM_P) pgm_read_word(&(numbers[input])));
        uart0_puts_p(PSTR(LINE_BREAK));
        lcd_puts_P((PGM_P) pgm_read_word(&(numbers[input])));
    } else {
        uart0_puts_p(PSTR(WRONG_NUMBER));
        lcd_puts_P(PSTR(NOT_DECIMAL_LCD));
    }
}

void cli_mem_stat(const char *const *argv)
{
    (void) argv;
    char print_buf[256] = {0x00};
    extern int __heap_start, *__brkval;
    int v;
    int space;
    static int prev_space;
    space = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    uart0_puts_p(PSTR("Heap statistics\r\n"));
    sprintf_P(print_buf, PSTR("Used: %u\r\nFree: %u\r\n"), getMemoryUsed(),
              getFreeMemory());
    uart0_puts(print_buf);
    uart0_puts_p(PSTR("\r\nSpace between stack and heap:\r\n"));
    sprintf_P(print_buf, PSTR("Current  %d\r\nPrevious %d\r\nChange   %d\r\n"),
              space, prev_space, space - prev_space);
    uart0_puts(print_buf);
    uart0_puts_p(PSTR("\r\nFreelist\r\n"));
    sprintf_P(print_buf, PSTR("Freelist size:             %u\r\n"),
              getFreeListSize());
    uart0_puts(print_buf);
    sprintf_P(print_buf, PSTR("Blocks in freelist:        %u\r\n"),
              getNumberOfBlocksInFreeList());
    uart0_puts(print_buf);
    sprintf_P(print_buf, PSTR("Largest block in freelist: %u\r\n"),
              getLargestBlockInFreeList());
    uart0_puts(print_buf);
    sprintf_P(print_buf, PSTR("Largest freelist block:    %u\r\n"),
              getLargestAvailableMemoryBlock());
    uart0_puts(print_buf);
    sprintf_P(print_buf, PSTR("Largest allocable block:   %u\r\n"),
              getLargestNonFreeListBlock());
    uart0_puts(print_buf);
    prev_space = space;
}


/* cli method to add card to list*/
void cli_rfid_add_card(const char *const *argv)
{
    card_rfid card;
    int len = strlen(argv[1]) / 2 ;
    char *user = malloc(strlen(argv[2]) + 1);
    char *array = malloc(strlen(argv[1]) + 1);
    char *size = malloc(strlen(argv[1]));

    if (!array || !user || !size) {
        uart0_puts_p(PSTR(MEMORY_FAILED));
        return;
    }

    //copy input details
    itoa(len, size, 10);
    card.size = size;
    strcpy(user, argv[2]);
    card.user = user;
    strcpy(array, argv[1]);
    card.uidArray = array;
    rfid_add_card(&card);
}

void cli_rfid_print_all_cards(void)
{
    rfid_list_all_cards();
}
void cli_rfid_read(void)
{
    Uid uid;
    Uid *uid_ptr = &uid;

    if (!PICC_IsNewCardPresent()) {
        byte bufferATQA[10];
        byte bufferSize = sizeof(bufferATQA);
        byte result;
        result = PICC_WakeupA(bufferATQA, &bufferSize);

        //check if the card has previously read
        if (result != STATUS_OK) {
            uart0_puts_p((PSTR(UNABLE_SELECT_CARD)));
            return;
        }
    }

    PICC_ReadCardSerial(uid_ptr);
    uart0_puts_p(PSTR(CARD_SELECTED));
    uart0_puts_p(PSTR(CARD_TYPE));
    uart0_puts(PICC_GetTypeName(PICC_GetType(uid_ptr->sak)));
    uart0_puts_p(PSTR(LINE_BREAK));
    uart0_puts_p(PSTR(CARD_UID));
    char *convert = bin2hex(uid_ptr->uidByte,  uid_ptr->size);
    uart0_puts(convert);
    uart0_puts_p(PSTR(CARD_SIZE));
    char *convert_size = bin2hex(&uid.size, sizeof uid_ptr->size);
    uart0_puts(convert_size);
    uart0_puts_p(PSTR(READ_CARD_END));
    uart0_puts_p(PSTR(LINE_BREAK));
    free(convert);
    free(convert_size);
}

void cli_remove_card(const char *const *argv)
{
    rfid_remove_card(argv[1], sizeof(argv[1]));
}

void cli_print_cmd_error(void)
{
    uart0_puts_p(PSTR(COMMAND_NOT_IMPLEMENTED));
}

void cli_print_cmd_arg_error(void)
{
    uart0_puts_p(
        PSTR(FALSE_INPUT));
}

int cli_execute(int argc, const char *const *argv)
{
    uart0_puts_p(PSTR("\r\n"));

    for (uint8_t i = 0; i < NUM_ELEMS(cli_cmds); i++) {
        if (!strcmp_P(argv[0], cli_cmds[i].cmd)) {
            // Test do we have correct arguments to run command
            // Function arguments count shall be defined in struct
            if ((argc - 1) != cli_cmds[i].func_argc) {
                cli_print_cmd_arg_error();
                return 0;
            }

            // Hand argv over to function via function pointer,
            // cross fingers and hope that funcion handles it properly
            cli_cmds[i].func_p (argv);
            return 0;
        }
    }

    cli_print_cmd_error();
    return 0;
}
