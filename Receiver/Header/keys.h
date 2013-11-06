/******************************************************************************/
/*  Dit is een onderdeel van het GNU armboard V3 Hogeschool Utrecht		      */
/*  Apptec Systems nov 2006						                       		  */
/******************************************************************************/
/*                                                                     		  */
/*  keys.c: Functions to read data from the keyboard controller 			  */
/*  Philips LPC2141/42/44/46/48                                        		  */
/*																		      */
/******************************************************************************/

#ifndef _KEYS_H
#define _KEYS_H


/******************************************************************************\
 Defines
\******************************************************************************/
#define DataReadyKeys()		(IOPIN1 & key_req)

// Key map
#define key_1	0x0010

#define key_2	0x0020
#define key_3	0x0040
#define key_4	0x0080
#define key_5	0x0001
#define key_6	0x0002
#define key_7	0x0004
#define key_8	0x0008
#define key_9	0x0100
#define key_0	0x0400

#define key_s	0x0200	// Star
#define key_h	0x0800	// Hash
#define key_a	0x1000
#define key_b	0x2000
#define key_c	0x4000
#define key_d	0x8000

#define key_dat 1<<21
#define key_clk 1<<22
#define key_req	1<<25



/******************************************************************************\
 protypes
\******************************************************************************/
void InitKeys(void);
unsigned int ReadKeys(void);

#endif
