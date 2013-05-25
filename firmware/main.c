#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>

#include "compat.h"
#include "common_defines.h"
#include "avr64_ports.h"
#include "lc_ports.h"
#include "output.h"
#include "ds1307.h"
#include "i2cmaster.h"
#include "uart.h"
#include "bitlash/src/bitlash.h"

#define MINIMUM_FREE_RAM 200


extern DS1307_BUFFER_STRUCT DS1307_BUF;

void main(void) {

    initialize();
    uart1_puts("Init\n\r");

    _delay_ms(10);
    
    char buf[128];

    //runBitlash();
    
    // test für dafo
    
    //while (1) {}
    
    while (1) {
        for (uint8_t j = 1; j < NUM_OUTPUTS + 1; j++) {
                activateOutput(j);
                
                //_delay_us(100);
            }
        send_to_output();
        _delay_ms(1000);
        
        
        for (uint8_t j = NUM_OUTPUTS; j > 0; j--) {
                deactivateOutput(j);
                //_delay_us(100);
            }
        
        send_to_output();
        _delay_ms(1000);
    }
    while (1) {
        for (uint8_t i = 1; i < NUM_OUTPUTS + 1; i++) {
            for (uint8_t j = 1; j < NUM_OUTPUTS + 1; j++) {
                deactivateOutput(j);
            }

            activateOutput(i);
            send_to_output();

            sprintf(buf, "Enable Output %d\n\r", i);
            uart1_puts(buf);
            ds1307_read();
            sprintf(buf, "Datum: %02d.%02d.%04d   ", DS1307_BUF.day, DS1307_BUF.month, DS1307_BUF.year + 2000); // Ausgabe Tag,Monat,Jahr
            uart1_puts(buf);
            sprintf(buf, "Zeit:  %02d:%02d:%02d\r\n", DS1307_BUF.hour, DS1307_BUF.minute, DS1307_BUF.second); // Ausgabe Stunde, Minute, Sekunde
            uart1_puts(buf);
            uart1_puts("\r\n");
            _delay_ms(1000);
        }
        _delay_ms(2000);

    }
}

void initialize () {
    uart1_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    sei();
    initOutputShiftRegister();
        i2c_init();
}
unsigned long pins;

void sp(const char *str) { while (*str) spb(*str++); }
void pinMode(byte pin, byte mode) { ; }
int digitalRead(byte pin) { return ((pins & (1<<pin)) != 0); }
void digitalWrite(byte pin, byte value) {
        if (value) pins |= 1<<pin;
        else pins &= ~(1<<pin);
}

// Stuff below is for bitslash
void spb (char c) {
    uart1_putc(c);
}

int serialRead (void) {
    return uart1_getc();
}

int analogRead(byte pin) { return 0; }
void analogWrite(byte pin, int value) { ; }
int pulseIn(int pin, int mode, int duration) { return 0; }
void speol(void) { spb(13); spb(10); }
unsigned long millis(void) {
    return 0;
}
int serialAvailable (void) {
    return 0;
}

void printHex(unumvar n) { printIntegerInBase(n, 16, 0, '0'); }
void delayMicroseconds(unsigned int us) {
    _delay_us(1);
}

void beginSerial ();
void beginSerial(unsigned long baud) { ; }
void eewrite(int i, byte j) {
    
}
byte eeread(int k) {
    
}
extern char virtual_eeprom[];
void eeinit(void);
void printIntegerInBase(unumvar n, uint8_t base, numvar width, byte pad) {
        char buf[8 * sizeof(numvar)];           // stack for the digits
        char *ptr = buf;
        if (n == 0) {
                *ptr++ = 0;
        }
        else while (n > 0) {
                *ptr++ = n % base;
                n /= base;
        }

        // pad to width with leading zeroes
        if (width) {
                width -= (ptr-buf);
                while (width-- > 0) spb(pad);
        }

          while (--ptr >= buf) {
                if (*ptr < 10) spb(*ptr + '0');
                else spb(*ptr - 10 + 'A');
        }

}

void printInteger(numvar n, numvar width, byte pad) {
        if (n < 0) {
                spb('-');
                n = -n;
                --width;
        }
        printIntegerInBase(n, 10, width, pad);
}
void printBinary(unumvar n) { printIntegerInBase(n, 2, 0, '0'); }
void delay (int a) {
    
}

void setBaud (int j) {
    
}
