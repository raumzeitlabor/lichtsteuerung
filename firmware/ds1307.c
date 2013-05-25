#include <i2cmaster.h>
#include <avr/delay.h>
#include "ds1307.h"
unsigned char rtc_bcd_to_hex(unsigned char v);




DS1307_BUFFER_STRUCT DS1307_BUF;

     void ds1307_schreiben(void)
{



                i2c_start_wait(SLAVE_ADDRESS+I2C_WRITE);       // set device address and write mode


                i2c_write(0x00); // Startadresse
                _delay_ms(1);
                i2c_write(0x20); // Sekunden
                _delay_ms(1);
                i2c_write(0x41); // Minuten
                _delay_ms(1);
                i2c_write(0x01); // Stunden
                _delay_ms(1);
                i2c_write(0x02); // Tag  in der Woche
                _delay_ms(1);
                i2c_write(0x21); // Tag
                _delay_ms(1);
                i2c_write(0x05); // Monat
                _delay_ms(1);
                i2c_write(0x13); // Jahr
                _delay_ms(1);
                i2c_write(0x90); // Jahr
                _delay_ms(1);

                i2c_stop();      // Bus stop




}
   
void ds1307_read (void)
{

        i2c_start_wait(SLAVE_ADDRESS+I2C_WRITE);       // set device address and write mode
        i2c_write(0x00);  
        i2c_stop();
                                           // Bus stop

    i2c_rep_start(SLAVE_ADDRESS+I2C_READ);       // set device address and write mode


        DS1307_BUF.second  = rtc_bcd_to_hex(i2c_readAck());       //sek
        DS1307_BUF.minute   = rtc_bcd_to_hex(i2c_readAck());       //min
        DS1307_BUF.hour   = rtc_bcd_to_hex(i2c_readAck());       //std
        DS1307_BUF.weekday    = rtc_bcd_to_hex(i2c_readAck());       //tag in der Woche
        DS1307_BUF.day      = rtc_bcd_to_hex(i2c_readAck());       //tag
        DS1307_BUF.month    = rtc_bcd_to_hex(i2c_readAck());       //monat
        DS1307_BUF.year     = rtc_bcd_to_hex(i2c_readNak());       //jahr



return;
}

unsigned char rtc_bcd_to_hex(unsigned char v)// rtc_bcd_to_hex: wandelt BCD zu HEX Zahl um
{
  unsigned char n;

  n=v;
  n=n & 0xF0;
  n=n/16;
  v =v & 0x0F;
  n=n*10+v;
  return(n);
}
