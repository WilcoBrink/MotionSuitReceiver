/* ========================================================================== */
/*                                                                            */
/*   spi.c                                                             		  */
/*   (c) 2011 Udo van Heteren                                                 */
/*                                                                            */
/*   SPI-1 (SSP) Aansturing                               					  */
/*                                                                            */
/* ========================================================================== */

#ifndef SPI_H
#define SPI_H
#define SCK  	17
#define MISO	18
#define MOSI  	19
#define SSEL	20


#define FIFOSIZE	16

/* Er staan in spi.c twee functies om data naar de SPI te sturen; een met
   foutcontrole, en een zonder. Volgende define op '0', en je krijgt de
   functie zonder foutcontrole.
   De spi-interface zal niet vaak fouten genereren, maar als het verkeerd
   gaat en er wordt niet op fouten gecontroleerd (SPI_ERR_TEST = 0), dan
   blijft de software hangen.
   Het is dus aan te bevelen deze op '1' te houden. */
#define SPI_ERR_TEST    0

/*******************************************************************************
  Volgende code is niet bedoeld om aan te passen
*/

//#include <config.h>
#include "LPC214x.h"

void SPI_init(unsigned char config);
void SPI_set_8_bit(void);
void SPI_set_12_bit(void);
void SPI_set_16_bit(void);
void SPI_set_low_speed(void);
void SPI_set_high_speed(void);
unsigned short SPI_put(unsigned short data);
unsigned short SPI_get(unsigned short data);

#endif /* SPI_H */
