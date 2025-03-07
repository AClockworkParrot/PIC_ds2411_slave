/*
 *  owslave
 *
 *  1-wire slave emulator for PIC16F84A
 *  10 MHz
 *  implemented commands: 0x33 send rom
 *                        0x44 start conversion (to initiate sensor measurment)
 *                        0xBE read scratchpad (to fetch conversion results)
 *  added support for sht1x, sht7x and sht2x (i2c) sensors
 *  created: April 2011
 *  author: Bartek Fabiszewski (www.fabiszewski.net)
 *  compiler: PICC PRO
 */
#include "config.h"

__CONFIG(CP_OFF & PWRTE_ON & WDTE_ON & FOSC_HS);


__EEPROM_DATA(0xBF, 0x42, 0x41, 0x52, 0x54, 0x45, 0x4B, 0xFF);  // serial number sequence:
                                                       // family code 0xBF,

void Read_SN(void);

byte SN[8];     // serial number
byte ow_status; // 1-wire status: 0 - waiting for reset
// ROM_CMD - waiting for rom command
// FUNCTION_CMD - waiting for function command
byte ow_buffer; // buffer for data received on 1-wire line
byte timeout;   // timeout to go to sleep while inactive
// because of tight timings we avoid sleep during conversation

#define INIT_SEQ (ow_status = 0, ow_error = 0, ow_buffer = 0, timeout = 10)

void interrupt ISR(void)
{
    byte i;
    byte current_byte;
    
    if (ow_status == ROM_CMD) { // ROM command
        ow_buffer = OW_read_byte();
        if (ow_error) {
            goto RST;
        }
        switch (ow_buffer) {
            case 0x33: //send rom
                i = 7;
                do {
                    while (OW);
                    OW_write_byte(SN[i]);
                } while (i--);
                break;
        }
        INIT_SEQ;
    end:
        INTF = 0;
        return;
    } else if (ow_status == FUNCTION_CMD) { // Function command
        ow_buffer = OW_read_byte();
        if (ow_error) {
            goto RST;
        }
        switch (ow_buffer) {
            case 0x44: // start conversion
                // currently we have functions for sensirion sht sensors
                // but any other sensor functions could be inserted here
                // measurments from sensors should be put into scratchpad
                sensor_error = 0;
                // initiate sensor measurments and read readings into scratchpad
                sht1x_read_buf(); // measure humidity and temperature

                // in case of sensor error reset it
                if (sensor_error != 0) {
                    sht1x_connectionreset();
                }
                break;
            case 0xBE: // read scratchpad
                if (sensor_error == 0) {
                    while (OW);
                    OW_write_byte(scratchpad[0]);
                    while (OW);
                    OW_write_byte(scratchpad[1]);
                    while (OW);
                    OW_write_byte(scratchpad[2]);
                    while (OW);
                    OW_write_byte(scratchpad[3]);
                    while (OW);
                    OW_write_byte(scratchpad[4]);
                    while (OW);
                    OW_write_byte(scratchpad[5]);
                    while (OW);
                    OW_write_byte(SENSOR_TYPE); // add sensor type so that 1-wire master knows how to process data
                }
                break;
        }
        INIT_SEQ;
        INTF = 0;
        return;
    }
RST:
    if (OW_reset_pulse()) {  // if reset detected
        __delay_us(30);
        OW_presence_pulse(); // generate presence pulse
        INIT_SEQ;
        ow_status = ROM_CMD; // and wait for rom command
    }else{
        INIT_SEQ; // else reset all settings
    }
    INTF = 0;
    return;
}

void main(void) {
    TRISA = 0xFF; //all inputs
    TRISB = 0xFF;
    PSA = 1;    // prescaler assigned to WDT
    PS0 = 1; PS1 = 1; PS2 = 1; //prescale = 128, WDT period = 2.3 s
    Read_SN();  // read serial number from eeprom
    INTEDG = 0; // external interrupt on falling edge
    INTE = 1;   // enable external interrupts
    GIE = 1;    // enable global interrupts
    
    INIT_SEQ;
    
    while (TRUE) {
        while (timeout) { // go to sleep after 1 second of inactivity
            CLRWDT();
            __delay_ms(100); // 0.1 s * 10 = 1 s
            timeout--;
        }
        CLRWDT();
        SLEEP();
        NOP();
    }
}

void Read_SN(void) {
    byte address = 0;
    do {
        SN[7-address] = EEPROM_READ(address);
    } while (++address < 7);
    SN[7-address] = CalcCRC(7, SN);
}


