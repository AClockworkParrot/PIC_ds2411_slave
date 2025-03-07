/***********************************************************************************
 Project:          SHT1x/7x demo program (V2.4)
 Filename:         SHT1x_sample_code.c
 
 Autor:            MST
                   converted for PIC16F84 and PICC compiler
                   by Bartek Fabiszewski (www.fabiszewski.net)
 
 Copyright:        (c) Sensirion AG
 ***********************************************************************************/
// Revisions:
// V2.4     calc_sht11()    Coefficients for humidity and temperature conversion
//                          changed (for V4 sensors)
//       calc_dewpoint()    New formula for dew point calculation


#include "config.h"
#ifdef SHT1x

//----------------------------------------------------------------------------------
char sht1x_write_byte(byte value)
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge
{
    byte i, error = 0;
    TRIS_DATA = OUTPUT;
    for (i = 0x80; i > 0; i /= 2) //shift bit for masking
    {
        if (i & value)
            DATA = 1; //masking value with i , write to SENSI-BUS
        else
            DATA = 0;
        __delay_us(1); //observe setup time
        SCK = 1; //clk for SENSI-BUS
        __delay_us(5); //pulswith approx. 5 us
        SCK = 0;
        __delay_us(1); //observe hold time
    }
    TRIS_DATA = INPUT; //release DATA-line
    __delay_us(1); //observe setup time
    SCK = 1; //clk #9 for ack
    error = DATA; //check ack (DATA will be pulled down by SHT11)
    SCK = 0;
    return error; //error=1 in case of no acknowledge
}

//----------------------------------------------------------------------------------
char sht1x_read_byte(byte ack)
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
{
    byte i, val = 0;
    TRIS_DATA = INPUT; //release DATA-line
    for (i = 0x80; i > 0; i /= 2) //shift bit for masking
    {
        SCK = 1; //clk for SENSI-BUS
        if (DATA)
            val = (val | i); //read bit
        SCK = 0;
    }
    TRIS_DATA = OUTPUT;
    DATA = !ack; //in case of "ack == 1" pull down DATA-Line
    __delay_us(1); //observe setup time
    SCK = 1; //clk #9 for ack
    __delay_us(5); //pulswith approx. 5 us
    SCK = 0;
    __delay_us(1); //observe hold time
    TRIS_DATA = INPUT; //release DATA-line
    return val;
}

//----------------------------------------------------------------------------------
void sht1x_transstart(void)
//----------------------------------------------------------------------------------
// generates a transmission start
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{
    TRIS_DATA = OUTPUT;
    TRIS_SCK = OUTPUT;
    DATA = 1;
    SCK = 0; //Initial state
    __delay_us(1);
    SCK = 1;
    __delay_us(1);
    DATA = 0;
    __delay_us(1);
    SCK = 0;
    __delay_us(5);
    SCK = 1;
    __delay_us(1);
    DATA = 1;
    __delay_us(1);
    SCK = 0;
}

//----------------------------------------------------------------------------------
void sht1x_connectionreset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{
    byte i;
    TRIS_DATA = OUTPUT;
    DATA = 1;
    SCK = 0; //Initial state
    for (i = 0; i < 9; i++) //9 SCK cycles
    {
        SCK = 1;
        SCK = 0;
    }
    sht1x_transstart(); //transmission start
}

//----------------------------------------------------------------------------------
char sht1x_softreset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset
{
    byte error = 0;
    sht1x_connectionreset(); //reset communication
    error += sht1x_write_byte(RESET); //send RESET-command to sensor
    return error; //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char sht1x_read_statusreg(byte *p_value, byte *p_checksum)
//----------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
{
    byte error = 0;
    sht1x_transstart(); //transmission start
    error = sht1x_write_byte(STATUS_REG_R); //send command to sensor
    *p_value = sht1x_read_byte(ACK); //read status register (8-bit)
    *p_checksum = sht1x_read_byte(noACK); //read checksum (8-bit)
    return error; //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char sht1x_write_statusreg(byte *p_value)
//----------------------------------------------------------------------------------
// writes the status register with checksum (8-bit)
{
    byte error = 0;
    sht1x_transstart(); //transmission start
    error += sht1x_write_byte(STATUS_REG_W);//send command to sensor
    error += sht1x_write_byte(*p_value); //send value of status register
    return error; //error >= 1 in case of no response form the sensor
}
#endif

