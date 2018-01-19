#include <avr/pgmspace.h>
#include <stdio.h>
#include <time.h>
#include <util/atomic.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "hmi_msg.h"
#include "print_helper.h"
#include <avr/interrupt.h>
#include "../lib/helius_microrl/microrl.h"
#include "cl_microrl.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/matejx_avr_lib/mfrc522.h"
#include "rfid.h"

#define BLINK_DELAY_MS  2000
#define LED_GREEN       PORTA2 // Arduino Mega digital pin 24
#define LED_BLUE        PORTA4 // Arduino Mega digital pin 26
#define DOOR_OPEN PORTA |= _BV(PORTA0)
#define DOOR_CLOSE PORTA &= ~_BV(PORTA0)

#define UART_BAUD           9600
#define UART_STATUS_MASK    0x00FF

typedef enum {
    door_opening,
    door_open,
    door_closing,
    door_closed
} door_state_t;

volatile uint16_t system_time;
door_state_t door = door_closed;

static microrl_t rl;
static microrl_t *prl = &rl;
static inline void heartbeat(void)
{
    static time_t prev_time;
    char ascii_buf[11] = {0x00};
    time_t now = time(NULL);

    if (prev_time != now) {
        //Print uptime to uart1
        ltoa(now, ascii_buf, 10);
        uart1_puts_p(PSTR("Uptime: "));
        uart1_puts(ascii_buf);
        uart1_puts_p(PSTR(" s.\r\n"));
        PORTA ^= _BV(LED_GREEN);
        PORTA ^= _BV(LED_BLUE);
        prev_time = now;
    }
}

static inline void init_led(void)
{
    DDRA |= _BV(LED_GREEN);
    DDRA |= _BV(LED_BLUE);
    DDRA |= _BV(PORTA0);
    DDRB |= _BV(DDB7);
    PORTB &= ~_BV(PORTB7);
}


static inline void init_con_uart1(void)
{
    uart1_puts_p(PSTR(VER_LIBC));
    uart1_puts_p(PSTR(VER_FW));
}


static inline void print_sys_time(void)
{
    char iso_time[20] = {0x00};
    struct tm now_tm;
    time_t now = time(NULL);
    gmtime_r(&now, &now_tm);
    isotime_r(&now_tm, iso_time);
    uart1_puts(iso_time);
    uart1_puts_p(PSTR("\r\n"));
}
static inline void init_sys_timer(void)
{
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= _BV(WGM12); // Turn on CTC (Clear Timer on Compare)
    TCCR1B |= _BV(CS12); // fCPU/256
    OCR1A = 62549; // Note that it is actually two registers OCR5AH and OCR5AL
    TIMSK1 |= _BV(OCIE1A); // Output Compare A Match Interrupt Enable
}

static inline void name()
{
    uart0_puts_p(PSTR(STUDENT_NAME_CONSOLE));
    uart0_puts_p(PSTR("\r\n"));
    lcd_clrscr();
    lcd_puts_P(PSTR(STUDENT_NAME_LCD));
}

static inline void start_cli(void)
{
    // Call init with ptr to microrl instance and print callback
    microrl_init (prl, uart0_puts);
    // Set callback for execute
    microrl_set_execute_callback (prl, cli_execute);
}

static inline uint16_t current_time(void)
{
    uint16_t cur_time = 0;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        cur_time = system_time;
    }
    return cur_time;
}

static inline void startup()
{
    init_led();
    uart1_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart0_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    init_con_uart1();
    init_sys_timer();
    lcd_init();
    lcd_home();
    name();
    start_cli();
    sei();
}

static inline void init_rfid_reader(void)
{
    MFRC522_init();
    PCD_Init();
}
/* handles door opening */
static inline void handle_door(void)
{
    Uid uid;
    Uid *uid_ptr = &uid;
    uint16_t time = current_time();
    static uint16_t door_open_time;
    static uint16_t msg_open_time;
    char  *name;
    static char * logged_user;
    byte bufferATQA[10];
    byte bufferSize[10];

    if (PICC_IsNewCardPresent()) {
        PICC_ReadCardSerial(&uid);
        name = bin2hex(uid_ptr->uidByte, uid_ptr->size);
        char *find_user = find(name);
        logged_user = find_user;

        if (find_user != NULL) {
            lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
            lcd_goto(LCD_ROW_2_START);
            lcd_puts(logged_user);
            DOOR_OPEN;
            door = door_opening;
        } else {
            lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
            lcd_goto(LCD_ROW_2_START);
            lcd_puts_P(PSTR(access_denied));
            door = door_opening;
            DOOR_CLOSE;
        }

        free(name);
        PICC_WakeupA(bufferATQA, bufferSize);
    }

    if (time - msg_open_time > 5 && logged_user != NULL ) {
        lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
        DOOR_CLOSE;
        logged_user = NULL;
    }

    switch (door) {
    case door_opening:
        door = door_open;
        msg_open_time = time;
        door_open_time = time;
        break;

    case door_open:
        if (time - door_open_time > 3) {
            door = door_closing;
            logged_user = NULL;
        }

        break;

    case door_closing:
        break;

    case door_closed:
        break;
    }
}

void main(void)
{
    startup();
    print_sys_time();
    init_rfid_reader();

    while (1) {
        heartbeat();
        microrl_insert_char(prl, (uart0_getc() & UART_STATUS_MASK));
        handle_door();
    }
}

ISR(TIMER1_COMPA_vect)
{
    system_tick();
    system_time++;
}

