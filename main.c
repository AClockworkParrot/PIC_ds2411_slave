#include "C:\PIC_projects\ds2411_v2\main.h"
#include "config.h"

byte const m[8] = {0x01, 0xE0, 0x65, 0x72, 0x11, 0x00, 0x00, 0xA3};  // serial number sequence


// func for detect reset pulse
byte OW_reset_pulse (void)
{
    byte del_count = 50;
    while (del_count--) // this loop takes 13.6 us
    {
        delay_us(10);
        if (OW)         // 124.8 us to here since master pulls bus low
        {
            break;
        }
    }
    if (del_count > 40) {
        return 0; // too short
    }
    return 1;     // reset pulse detected
}

// 1-wire slave write bit
// must be run immediately after line is pulled down by master
void OW_write_bit (byte write_bit)
{
    if (write_bit)
    {
        //writing a bit '1'
        delay_us(55);//55                // bus is already pulled high by pullup resistor, so just wait
    }
    else
    {
        //writing a bit '0'
        drive_OW_low();                    // drive the bus low
        delay_us(17);//15
        drive_OW_high();                   // release the bus
        delay_us(35);//35
    }
}

// func for write bytes
void OW_write_byte (byte write_data)
{
    OW_write_bit(write_data & 0x01);     // sending LS-bit first
    while (OW);                          // wait for master set bus low
    write_data >>= 1;                    // shift the data byte for the next bit to send
    OW_write_bit(write_data & 0x01);
    while (OW);
    write_data >>= 1;
    OW_write_bit(write_data & 0x01);
    while (OW);
    write_data >>= 1;
    OW_write_bit(write_data & 0x01);
    while (OW);
    write_data >>= 1;
    OW_write_bit(write_data & 0x01);
    while (OW);
    write_data >>= 1;
    OW_write_bit(write_data & 0x01);
    while (OW);
    write_data >>= 1;
    OW_write_bit(write_data & 0x01);
    while (OW);
    write_data >>= 1;
    OW_write_bit(write_data & 0x01);
}

void main()
{
byte i;
    // Initialization
    //  setup_timer_0(RTCC_INTERNAL | RTCC_DIV_1); // Configure Timer 0
    //   SET_TRIS_B(0xFF); // Configure port B as input
    SETUP_WDT(WDT_2304MS); // Configure Watchdog Timer

    while (TRUE) {
        // Periodic code execution, similar to interrupts
       
        
        // Check for reset and reset 1-Wire device
        if (OW_reset_pulse()) {  // if reset pulse detected
            delay_us(30); // ����������� ��������� ��������
        for ( i = 0; i < 8; i++) {  // Loop over the array length
   
        while (OW); // Wait for 1-Wire to be idle
   
        OW_write_byte(m[i]); // Send the byte
        }
   }
        
     //   Watchdog Timer handling
     RESTART_WDT();
        delay_ms(100); // Delay 0.1 s * 10 = 1 s
    }}

