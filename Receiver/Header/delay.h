/*
    ARM IP interface

    delay.h:
            vertragings functies testboard

*/

#ifndef DELAY_H
#define DELAY_H

#define DELAY_US    5/2
#define DELAY_MS    5*1200
#define DELAY_S     5*1200000

/*******************************************************************************
  Volgende code is niet bedoeld om aan te passen
*/

#include "LPC214x.h"

void delay_us(volatile unsigned int time);
void delay_ms(volatile unsigned int time);
void delay_s(volatile unsigned int time);

#endif /* DELAY_H */
