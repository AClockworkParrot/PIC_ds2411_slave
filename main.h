#include <10F220.h>
#device adc=8

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES NOMCLR                   //Master Clear pin used for I/O
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NOMCPU                   //Master Clear Pull-up disabled
#FUSES IOSC8                    //INTOSC speed 8MHz

#use delay(clock=8000000)
#use rs232(baud=9600,parity=N,xmit=PIN_B0,rcv=PIN_B0,bits=8)

