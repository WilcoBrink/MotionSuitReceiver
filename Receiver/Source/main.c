/******************************************************************************\
 Include files
\******************************************************************************/
#include "LPC214x.h"
#include "pll.h"
#include "MRF24J40.h"
#include "spi.h"
#include "delay.h"
#include "lcd.h"
#include "keys.h"
#include "uart.h"

#define stap 10
#define xas 0
#define yas 1
#define zas 2

unsigned short gemiddeld(int as);
void opschuiven(void);
extern  void __enable_interrupts(void);
extern  void __disable_interrupts(void);
extern	char Received_Dataext[128];
extern 	int nieuwe_data;



/******************************************************************************\
 main
\******************************************************************************/
unsigned short tel [3][stap];



extern int main(void)
{ 
	/*   init libs   */
	unsigned short zend[6];
	int i,t;
	PLL_init();
	SPI_init(0x0F);								// SPI_init(SPI configuratie)
	UART_init();
	lcd_init();
	MRF24J40_init(0xAABB);						// MRF24J40_init(eigen adres), was 0x0004

	MRF24J40_wake();	
	delay_s(2);

	MRF24J40_receive();							// MRF24J40 receive
	lcd_print("Hij is opgestart");
	delay_s(1);

	for(i=0;i<3;i++)
	{
		for (t=0;t<stap;t++){
			tel [i][t]=0;
		}

	}

	while(1)
	{	
		if(nieuwe_data==0xFFFF){				// wordt 0xFFFF bij een interrupt van de transceiver
			__disable_interrupts();

			opschuiven();

			for(i=0;i<3;i++){
				 tel[i][0]= (Received_Dataext[2*i]<<8) + Received_Dataext[2*i+1];
			}
			zend[0]=gemiddeld(xas);
			zend[1]=gemiddeld(yas);
			zend[2]=gemiddeld(zas);
			zend[3]=(Received_Dataext[6]<<8) + Received_Dataext[7];
			zend[4]=(Received_Dataext[8]<<8) + Received_Dataext[9];
			zend[5]=(Received_Dataext[10]<<8) + Received_Dataext[11];

			for (i=0;i<6;i++)
			{
				UART_putint(zend[i]);
				UART_put(",");
			}

			UART_put("\n");
			nieuwe_data=0x0000;
			__enable_interrupts();
		}
	}
	return 0;									// don't ever come near this
}

void opschuiven(void)
{
	int i;
	for (i=0;i<stap;i++){
		tel[xas][stap-i-1]=tel[xas][(stap-2)-i];
		tel[yas][stap-i-1]=tel[yas][(stap-2)-i];
		tel[zas][stap-i-1]=tel[zas][(stap-2)-i];
	}

}

unsigned short gemiddeld(int as)
{
	int avg,i=0;
	unsigned short value=0;
	for (i=0;i<stap;i++){
		avg=avg+tel[as][i];
	}
	avg=avg/stap;
	value=avg;
	return value;
}
