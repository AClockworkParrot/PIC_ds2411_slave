/*
 *    created: April 2011
 *    author: Bartek Fabiszewski (www.fabiszewski.net)
 */
#ifndef _1wire_H
#define _1wire_H

// macros
// I turned some functions into macros to avoid losing time on jumps

#define drive_OW_low() { SET_TRIS_B(0b11111110); OUTPUT_LOW(PIN_B0); }
#define drive_OW_high() { SET_TRIS_B(0b11111111);}
#define OW_presence_pulse() {\
drive_OW_low();  \
delay_us(120); \
drive_OW_high(); \
}

#define ROM_CMD                1        // ROM Command
#define FUNCTION_CMD           2        // Function Command

// prototypes
void OW_write_bit (byte write_data);
byte OW_read_bit (void);
byte OW_reset_pulse (void);
void OW_write_byte (byte write_data);
byte OW_read_byte(void);
byte OW_match_search (byte write_bit);
byte OW_match_bits (byte read_bit);
#endif
