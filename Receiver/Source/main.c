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
short *calibratie(void);
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
	short *pCal;
	short Cal[3];
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

	pCal=calibratie();

	Cal[0]=*pCal;
	pCal++;
	Cal[1]=*pCal;
	pCal++;
	Cal[2]=*pCal;

	while(1)
	{	
		if(nieuwe_data==0xFFFF){				// wordt 0xFFFF bij een interrupt van de transceiver
			__disable_interrupts();

			//opschuiven();

			for(i=0;i<3;i++){
				 tel[i][0]= ((Received_Dataext[2*i]<<8) + Received_Dataext[2*i+1])-Cal[i];
			}
			zend[0]=tel[0][0];//gemiddeld(xas);
			zend[1]=tel[1][0];//gemiddeld(yas);
			zend[2]=tel[2][0];//gemiddeld(zas);
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

short *calibratie()
{
	short x,y,z,i;
	short calibratie_waarde[3];
	short *pCalibratie;
	pCalibratie=&calibratie_waarde[0];
	int xi,yi,zi=0;

	for (i=0;i<10;i++)
	{
		while(nieuwe_data!=0xFFFF);
	x=(Received_Dataext[0]<<8) + Received_Dataext[1];
	y=(Received_Dataext[2]<<8) + Received_Dataext[3];
	z=(Received_Dataext[4]<<8) + Received_Dataext[5];
	xi=xi+x;
	yi=yi+y;
	zi=zi+z;
		nieuwe_data=0;
	}

	calibratie_waarde[0]=xi/10;
	calibratie_waarde[1]=yi/10;
	calibratie_waarde[2]=zi/10;
	calibratie_waarde[2]=calibratie_waarde[2]-16383;// 16383 is de waarde van de zwaartekracht

	return pCalibratie;
}
