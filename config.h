/*
 *    config.h
 *
 *    created: April 2011
 *    author: Bartek Fabiszewski (www.fabiszewski.net)
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define drive_OW_low() { SET_TRIS_B(0b11111110); OUTPUT_LOW(PIN_B0); }
#define drive_OW_high() { SET_TRIS_B(0b11111111);}
#define OW_presence_pulse() {\
drive_OW_low();  \
delay_us(120); \
drive_OW_high(); \
}

#define _XTAL_FREQ    8000000 // 4MHz

// leave uncommented only necessary module
// only one module may be compiled into hex due to PIC code space limits
#define SHT1x     // leave uncommented if you have sht2x sensor supporting i2c connected to pic

//#define TRUE                1
//#define FALSE               0
#define INPUT               1
#define OUTPUT              0
#define HIGH                1
#define LOW                 0

#define OW                  PIN_B0          //pin 6 for 1-wire data (this can't be changed)
#define TRIS_OW             TRISB0

#define MOD1                             //my first design used different pins

#ifdef MOD1
#define DATA                RB6          //pin 12 for sht data
#define SCK                 RB7          //pin 13 for sht clock
#define TRIS_DATA           TRISB6
#define TRIS_SCK            TRISB7

// some global vars
//byte ow_error;      // error on ow line
///byte sensor_error;  // error on sensor line
///byte scratchpad[6]; // 6-byte buffer [h-msb h-lsb h-crc t-msb t-lsb t-crc]



//#include "sht1x.h"


#endif /* CONFIG_H_ */
