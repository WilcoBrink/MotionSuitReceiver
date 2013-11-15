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
/******************************************************************************\
 Defines
\******************************************************************************/
#define samples 10		// Amount of steps used in the moving average filter
#define xAxis 0
#define yAxis 1
#define zAxis 2
/******************************************************************************\
 Prototype functions
\******************************************************************************/
unsigned short average(int as);
void array_shift_right(void);
short *calibrate(void);
extern  void __enable_interrupts(void);
extern  void __disable_interrupts(void);
extern	char Received_Dataext[128];
extern 	int newData;


/******************************************************************************\
 Main
\******************************************************************************/
unsigned short tel [6][samples];		// 2D array used by MAV filter

extern int main(void)
{ 
	/*   init vars   */
	unsigned short send[12];
	int i,t;
	short *pCal;
	short Cal[6];
	/*   init libs   */
	PLL_init();
	SPI_init(0x0F);						// Initialize the SPI0 interface
	UART_init();
	lcd_init();
	MRF24J40_init(0xAABB);				// Initialize the MRF24J40 board

	MRF24J40_wake();					// Wake up the MRF24J40 board
	delay_ms(500);

	MRF24J40_receive();					// MRF24J40 ready to receive data
	lcd_print("Hij is opgestart");
	delay_ms(500);

	for(i=0;i<6;i++)					// Loop to clear the array used in the MAV filter
	{
		for (t=0;t<samples;t++){
			tel[i][t] = 0;
		}
	}

	pCal=calibrate();					// Get calibration values

	Cal[0]=*pCal;
	pCal++;
	Cal[1]=*pCal;
	pCal++;
	Cal[2]=*pCal;
	pCal++;
	Cal[3]=*pCal;
	pCal++;
	Cal[4]=*pCal;
	pCal++;
	Cal[5]=*pCal;

	while(1)
	{	
		if(newData==0xFFFF){			// newData is made 0xFFFF when the transceiver sets an interrupt
			__disable_interrupts();		// Ignore interrupts while reading sensor and sending them via UART

			//array_shift_right();		// Shift old values to  make room for the new data

			for(i=0;i<3;i++){			// Loop to read the 3 accelerometer axes
				 tel[i][0]= ((Received_Dataext[2*i]<<8) + Received_Dataext[2*i+1])-Cal[i];
			}

			for(i=3;i<6;i++){
				 tel[i][0]= ((Received_Dataext[2*i+6]<<8) + Received_Dataext[2*i+7])-Cal[i];
			}

			send[0]=tel[0][0];//average(xAxis);		// MAV filter
			send[1]=tel[1][0];//average(yAxis);
			send[2]=tel[2][0];//average(zAxis);
			send[3]=(Received_Dataext[6]<<8) + Received_Dataext[7];		// Read gyroscope data
			send[4]=(Received_Dataext[8]<<8) + Received_Dataext[9];
			send[5]=(Received_Dataext[10]<<8) + Received_Dataext[11];
			send[6]=tel[3][0];//gemiddeld(xas);
			send[7]=tel[4][0];//gemiddeld(yas);
			send[8]=tel[5][0];//gemiddeld(zas);
			send[9]=(Received_Dataext[18]<<8) + Received_Dataext[19];
			send[10]=(Received_Dataext[20]<<8) + Received_Dataext[21];
			send[11]=(Received_Dataext[22]<<8) + Received_Dataext[23];

			for (i=0;i<12;i++)			// Loop to send the 6 sensor values to the computer
			{
				UART_putint(send[i]);
				UART_put(",");			// Comma used as split token in Processing
			}

			UART_put("\n");				// New line used by Processing to recognize end of transmission
			newData=0x0000;				// Clear newData
			__enable_interrupts();		// Enable interrupts
		}
	}
	return 0;							// don't ever come near this
}

/******************************************************************************\
 void array_shift_right(void):
 Shifts all elements in the array to the right
\******************************************************************************/
void array_shift_right(void)
{
	int i;
	for (i=0;i<samples;i++){
		tel[xAxis][samples-i-1]=tel[xAxis][(samples-2)-i];
		tel[yAxis][samples-i-1]=tel[yAxis][(samples-2)-i];
		tel[zAxis][samples-i-1]=tel[zAxis][(samples-2)-i];
		tel[3][samples-i-1]=tel[3][(samples-2)-i];
		tel[4][samples-i-1]=tel[4][(samples-2)-i];
		tel[5][samples-i-1]=tel[5][(samples-2)-i];
	}
}

/******************************************************************************\
 unsigned short average(int axis):
 Calculates the average of 10 samples
\******************************************************************************/
unsigned short average(int axis)
{
	int avg,i=0;
	unsigned short value=0;
	for (i=0;i<samples;i++){
		avg=avg+tel[axis][i];
	}
	avg=avg/samples;
	value=avg;
	return value;
}

/******************************************************************************\
 short *calibrate(void):
 Reads accelerometer axes and calculates offset per axis
\******************************************************************************/
short *calibrate(void)
{
	short x,y,z,i,x2,y2,z2;
	short calibratie_waarde[6];
	short *pCalibratie;
	pCalibratie=&calibratie_waarde[0];
	int xi,yi,zi,xi2,yi2,zi2=0;

	for (i=0;i<10;i++)
	{
		while(newData!=0xFFFF);
		x = (Received_Dataext[0]<<8) + Received_Dataext[1];
		y = (Received_Dataext[2]<<8) + Received_Dataext[3];
		z = (Received_Dataext[4]<<8) + Received_Dataext[5];
		x2=(Received_Dataext[12]<<8) + Received_Dataext[13];
		y2=(Received_Dataext[14]<<8) + Received_Dataext[15];
		z2=(Received_Dataext[16]<<8) + Received_Dataext[17];
		xi = xi + x;
		yi = yi + y;
		zi = zi + z;
		xi2=xi2+x2;
		yi2=yi2+y2;
		zi2=zi2+z2;

		newData = 0;
	}

	calibratie_waarde[0]=xi/10;
	calibratie_waarde[1]=yi/10;
	calibratie_waarde[2]=zi/10;
	calibratie_waarde[2]=-(16383-calibratie_waarde[2]);
	calibratie_waarde[3]=xi2/10;
	calibratie_waarde[4]=yi2/10;
	calibratie_waarde[5]=zi2/10;
	calibratie_waarde[5]=-(16383-calibratie_waarde[5]);
	return pCalibratie;
}
