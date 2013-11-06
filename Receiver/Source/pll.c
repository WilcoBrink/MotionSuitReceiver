/*
    ARM IP interface

    pll.c:
          PLL-initialisatie testboard

    opmerkingen:
                -Als de PLL niet locked, blijft de processor hier hangen

*/
	// We'd like the LPC2148 to run at 60 mhz (has to be an even multiple of crystal)
	// 
	// According to the Philips LPC2148 manual:   M = cclk / Fosc	where:	M    = PLL multiplier (bits 0-4 of PLLCFG)
	//																		cclk = 60000000 hz
	//																		Fosc = 12000000 hz
	//
	// Solving:	M = 60000000 / 12000000 = 5           
	//
	//			Note: M - 1 must be entered into bits 0-4 of PLLCFG (assign 4 to these bits)
	//
	//
	// The Current Controlled Oscilator (CCO) must operate in the range 156 mhz to 320 mhz
	//
	// According to the Philips LPC2148 manual:	Fcco = cclk * 2 * P    where:	Fcco = CCO frequency 
	//																			cclk = 60000000 hz
	//																			P = PLL divisor (bits 5-6 of PLLCFG)
	//
	// Solving:	Fcco = 60000000 * 2 * P
	//			P = 2  (trial value)
	//			Fcco = 60000000 * 2 * 2
	//			Fcc0 = 240000000 hz    (good choice for P since it's within the 156 mhz to 320 mhz range)
	//
	// From Table 22 (page 34) of Philips LPC2148 manual    P = 2, PLLCFG bits 5-6 = 1  (assign 1 to these bits)
	//
	// Finally:      PLLCFG = 0  01  00100  =  0x24
	//
	// Final note: to load PLLCFG register, we must use the 0xAA followed 0x55 write sequence to the PLLFEED register
	//             this is done in the short function feed() below
	//

#include "pll.h"
#include "LPC214x.h"

void PLL_init(void)
/*
  Initialiseert de PLL met de gedefineerde snelheden
*/
{
	PLL0CON  = 0x01;							// Connect to PLL source
	PLL0CFG  = 0x24;							// Set PLL
	PLL0FEED = 0xAA;
	PLL0FEED = 0x55;
	while (!(PLL0STAT&0x00000400));  			// wacht op de PLL voor een lock
	PLL0CON  = 0x03;							// Connect to PLL source
	PLL0FEED = 0xAA;
	PLL0FEED = 0x55;
	MAMTIM = 0x03;
	MAMCR 	 = 2;//1;  							// 1 = FLASH, 2 = ROM
	VPBDIV 	 = 1;								// PCLK = CCLK


	
}
