/******************************************************************************/
/*  Dit is een onderdeel van het GNU armboard V3 Hogeschool Utrecht		      */
/*  Apptec Systems nov 2006						                       		  */
/******************************************************************************/
/*                                                                     		  */
/*  keys.c: Functions to read data from the keyboard controller 			  */
/*  Philips LPC2141/42/44/46/48                                        		  */
/*																		      */
/******************************************************************************/

/******************************************************************************\
 Include files
\******************************************************************************/
#include "LPC214x.h"
#include "keys.h"


/******************************************************************************\
 board definities
\******************************************************************************/


/* Function:	Initialize the keyboard controller
 *				Currently only initializes the IO ports
 */ 
void InitKeys(void)
{
	unsigned char y;
	IODIR0 |= key_clk;						// Set I/O direction
	IODIR0 &= ~key_dat;						//
	IODIR1 &= ~key_req;						//
	IOCLR0 = key_clk;
	//reset PIC. clock until key_req goes high
	while(!(IOPIN1 & key_req)) // Wait for req line to go high
	{
	 IOSET0 = key_clk;
		y = 200;
		while(y--);
	
	 IOCLR0 = key_clk;
		y = 200;
		while(y--);
	}
	
}

/* Function:	Reads keystroke from keyboard buffer
 * Returns:		Key from buffer
 */ 
unsigned int ReadKeys(void)
{
	unsigned char x, y;
	unsigned int  data = 0;
	
	// Generate start pattern
	IOSET0 = key_clk;						// Set clk (ack) line
	while(IOPIN1 & key_req);				// Wait for req line to go low
	IOCLR0 = key_clk;
	
	for(x=0;x<16;x++)						// Read data
	{
		y = 200;
		while(y--);
		IOSET0 = key_clk;					// Data is clocked out on the rising edge
		y = 200;
		while(y--);
		IOCLR0 = key_clk;					// Generate a falling edge
		
		data |= !!(IOPIN0 & key_dat) << 16;	// Data's clocked out on rising edge
											// so read in on falling edge
		data >>= 1;							// Shift bit
	}
	
	return data;
}
