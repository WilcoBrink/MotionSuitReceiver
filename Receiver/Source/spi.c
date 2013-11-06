/* ========================================================================== */
/*                                                                            */
/*   spi.c                                                 		              */
/*   (c) 2011 Udo van Heteren                                                 */
/*                                                                            */
/*   SPI-1 (SSP) Aansturing 				                                  */
/*    																		  */
/*	 pin-info:																  */
/*         P17 - SCK:  klok													  */
/*         P18 - MISO: Data-uit												  */
/*         P19 - MOSI: Data-in												  */
/*		                                                                      */
/* ========================================================================== */

#include "spi.h"

void SPI_init(unsigned char config)
{
    /* pinnen instellen */
    PINSEL1 |= 2<<((SCK-16)*2);				// set SCK
    PINSEL1 |= 2<<((MISO-16)*2);			// set MISO
    PINSEL1 |= 2<<((MOSI-16)*2);			// set MOSI
    
	SSPCR1 = 0x01;							// Enable loop-back-mode
	SSPCR0 = 0x10F;							/* initialiseer SSP, = 0001 0 0 00 1111
										    SCR			= 0001
										    CPHA 		= 0 : Clock remains low between frames
										    CPOL 		= 0 : Data is sampled on first Clock edge
										    FRF			= 00 : SPI Frame Format
										    DSS			= 1111 : 16 bits per transfer*/
	SSPCPSR = 12;							// SPI snelheid 5 MHz = 10 MHz(clock MRF49XA)/2 (MRF49XA gegeven)
	SSPCR1 = 0x02;							// enable SSP controler    
}

void SPI_set_8_bit()
{
	SSPCR1 = 0x01;							// disable SSP Controler
	SSPCR0 = 0x107;							// DSS 		= 0111 : 8 bits per transfer
	SSPCR1 = 0x02;							// enable SSP Controler
}

void SPI_set_12_bit()
{
	SSPCR1 = 0x01;							// disable SSP Controler
	SSPCR0 = 0x10B;							// DSS 		= 1101 : 12 bits per transfer
	SSPCR1 = 0x02;							// enable SSP Controler
}

void SPI_set_16_bit()
{
	SSPCR1 = 0x01;							// disable SSP Controler
	SSPCR0 = 0x10F;							// DSS		= 1111 : 16 bits per transfer
	SSPCR1 = 0x02;							// enable SSP Controler
}

void SPI_set_low_speed()
{
	SSPCR1 = 0x01;							// disable SSP Controler
    SSPCPSR = 12;							// SPI snelheid = 2.5 MHz
    SSPCR1 = 0x02;							// enable SSP Controler
}

void SPI_set_high_speed()
{
	SSPCR1 = 0x01;							// disable SSP Controler
    SSPCPSR = 6;							// SPI snelheid = 5 MHz
    SSPCR1 = 0x02;							// enable SSP Controler
}

unsigned short SPI_get(unsigned short data)	//ontvang SPI Byte
{
	int dummy;
	while(SSPSR & 0x4)						// FIFO leeg?
    {
	dummy = SSPDR;							// Lees FIFO
    }
	SSPDR = data;							// clock genereren voor MISO
	while (SSPSR & 0x10);					// klaar?	
	return (SSPDR);
}

unsigned short SPI_put(unsigned short data)	// Stuur meegegeven data over SPI, status wordt teruggegeven
{
	while (!(SSPSR & 0x2));					// check status register, geen fifo leeg?
    SSPDR = data;
	while(SSPSR & 0x10);					// Klaar?
    return 0x80;
}
