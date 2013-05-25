#ifndef DS1307_H
#define	DS1307_H

typedef struct {                        
        uint8_t control_0;              //C0            0x00
        uint8_t second;                //sek           0x08
        uint8_t minute;                 //min
        uint8_t hour;                 //std
        uint8_t month;                  //monat
        uint8_t day     ;                       //tag
        uint8_t weekday;                  //w_tag
        uint8_t year;                   //jahr          0x0E
} DS1307_BUFFER_STRUCT;

#define SLAVE_ADDRESS 0xD0

void ds1307_read (void);

#endif	/* DS1307_H */

