/*
    ARM IP interface

    delay.c:
            vertragings functies testboard

    opmerkingen:
                -Deze timingswaardes zijn niet bepaald nauwkeurig(vooral de us 
                 niet).
                 Voor echt exacte timing zou je dit in assembly moeten doen,
                 maar als je zo precies wil, neem dan een timerinterrupt; dan 
                 weet je zeker dat je timing klopt.

*/

#include "delay.h"

void delay_us(volatile unsigned int time)
/*
  Vertraging in microseconden.
  Deze is het minst precies; de vertraging is afgerond naar boven, zodat er niet 
  met floating point wordt geprutst..
*/
{
    volatile unsigned int i;

    while(time>0) {
        for(i=0;i<DELAY_US;i++);
        time--;
    }
}

void delay_ms(volatile unsigned int time)
/*
  Vertraging in milliseconden.
*/
{
    volatile unsigned int i;

    while(time>0) {
        for(i=0;i<DELAY_MS;i++);
        time--;
    }
}

void delay_s(volatile unsigned int time)
/*
  Vertraging in seconden.
*/
{
    volatile unsigned int i;

    while(time>0) {
        for(i=0;i<DELAY_S;i++);
        time--;
    }
}
