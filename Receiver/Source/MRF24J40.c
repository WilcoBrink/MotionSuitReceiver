/* ========================================================================== */
/*                                                                            */
/*   MRF24J40.c                                                               */
/*   (c) 2011 Udo van Heteren                                                 */
/*                                                                            */
/*   MRF24J40 MiWi Driver													  */
/*                                                                            */
/* ========================================================================== */

#include "spi.h"
#include "MRF24J40.h"
#include "LPC214x.h"
#include "delay.h"
#include "vic.h"
#include "lcd.h"

extern  void __enable_interrupts();
extern  void __disable_interrupts();
extern	char Received_Dataext[128]="Testert";
extern	int nieuwe_data = 0x0000;

int	busy = 0;
int sequence_number = 0;
unsigned int adres2;
char errors = '0';



MRF24_DATA_PKG MRF24_Rx_Data;									// Structure for RX Data

void MRF24J40_init(unsigned int adres)
{
	// INIT I/O
	__disable_interrupts();
	PINSEL1 |=   2<<(INT-16)*2;									// INT = EINT3
	PINSEL0 &= ~(3<<RESET_MRF*2);								// RESET = GPIO
	PINSEL1 &= ~(3<<(CS_MRF24-16)*2);							// CS = GPIO
	PINSEL1 &= ~(3<<(WAKE_24-16)*2);							// WAKE = GPIO
	IODIR0  |=   1<<RESET_MRF;									// RESET = output
	IODIR0  |=   1<<CS_MRF24;									// CS = output
	IODIR0  |=   1<<WAKE_24;									// CS = output
    IOSET0	 =   1<<RESET_MRF;									// set RESET, RESET is inverted
    IOSET0   =   1<<CS_MRF24;									// set CS, CS is inverted
    IOCLR0   =   1<<WAKE_24;									// clr WAKE
	
	// VIC SETUP
	EXTMODE |= (1<<3);											// EINT3 edge sensitive
	EXTPOLAR &=~ (1<<3);										// EINT3 falling edge sensitive 
	VicSetup((unsigned)MRF24J40_interrupt,IRQ,EINT3_INT_SLOT,MRF24_PRIOR);
	
    // INIT MRF24J40
    adres2 = adres;
    SPI_set_16_bit();											// SPI 16 bits
	MRF24_Write_Short(SOFTRST | RESET_MRF24);					// Perform a software reset
	MRF24_Write_Short(PACON2 | FIFOEN_24 | TXONST);				// Initialise FIFO and configure Transmitter
	MRF24_Write_Short(TXSTBL | RFSTBL);							// Initialise SIFS
	MRF24_Write_Long(RFCON0 | CH_16);							// Channel select : Channel 16
	MRF24_Write_Long(RFCON1 | VCOOPT);							// Configure VCO
	MRF24_Write_Long(RFCON2 | PLLEN);							// Enable PLL
	MRF24_Write_Long(RFCON6 | TXFIL | CLKRECVR);				// Configure TX fliter and Clock Recovery
	MRF24_Write_Long(RFCON7 | SLPCLKDIV);						// Select 100 kHz sleep clock
	MRF24_Write_Long(RFCON8 | RFVCO);							// Configure VCO Control
	MRF24_Write_Long(SLPCON1 | CLKOUTEN | SLPCLKSEL);			// Disable CLKOUT pin, configure Sleep Clock Divisor
	MRF24_Write_Short(BBREG2 | CCA | CCACSTH);					// Set Clear-Channel-Assessment to Energy-Detection
	MRF24_Write_Short(CCAEDTH | CCAED);							// Set Energy-Detection treshold
	MRF24_Write_Short(BBREG6 | RSSIMODE);						// Caculate RSSI per packet, store in RXFIFO
	MRF24_Write_Short(TXPEND | MLIFS);							// Spacing between frames
	MRF24_Write_Short(TXTIME | TURNTIME);						// Spacing between sending and receiving
	MRF24_Write_Short(TXSTBL | RFSTBL);							// VCO stabilization time and frame spacing
	MRF24_Write_Long(SLPCON0 | INTEDGE | SLPCLKEN);				// Interrupt on falling edge, Sleep clock enabled
	MRF24_Write_Short(INTCON | INTDISABLE);						// All interrupts disabled
	MRF24_Write_Long(RFCON3 | TXPOWER);							// Transmitter max power
	MRF24_Write_Short(RFCTL | RXTXRST);							// Reset transmitter and receiver
	MRF24_Write_Short(RFCTL | RXTXSET);							// Transmitter and receiver off
	delay_us(192);
	EXTINT |= MRF24_EINT;										// reset interrupts
	__enable_interrupts();
}

void MRF24J40_send_byte(char data, unsigned short adres)
{
	MRF24J40_send(&data, 1, adres);		
}

void MRF24J40_send_string(char *tekst, unsigned short adres)
{
	int Data_length_24 = 0;
	while(tekst[Data_length_24] != '\0')
		Data_length_24++;
		
	MRF24J40_send(tekst, Data_length_24, adres);
	MRF24J40_receive();
}

void MRF24J40_send(char *data, int Data_length_24, unsigned int adres)
{
	while(busy);												// sending in progress?? Wait
	busy = 1;
	__disable_interrupts();
	
	//Create dataframe
	int i, j=0;
	MRF24_Write_Short(INTCON | TXNIE);							// Enable transmit interrupt
	MRF24_Write_Long((0x000000 << 1) | HEADER_SIZE);			// Header length = 9
	MRF24_Write_Long((0x001000 << 1) | (Data_length_24 + HEADER_SIZE));	// Frame length = datalength + header length
	MRF24_Write_Long((0x002000 << 1) | FRAMECTRL1);				// FrameControl; one PAN frame, ask for Ack, no following packets, usecured, data trasfer
	MRF24_Write_Long((0x003000 << 1) | FRAMECTRL2);				// FrameControl; source, PAN and destination addresses are 2 bytes each
	MRF24_Write_Long((0x004000 << 1) | ((sequence_number++)&0xFF));// Sequence Number
	MRF24_Write_Long((0x005000 << 1) | ((adres & 0xFF00)>>8));	// Destination PAN
	MRF24_Write_Long((0x006000 << 1) | (adres & 0x00FF));		// Destination PAN
	MRF24_Write_Long((0x007000 << 1) | ((adres & 0xFF00)>>8));	// Destination address
	MRF24_Write_Long((0x008000 << 1) | (adres & 0x00FF));		// Destination address
	MRF24_Write_Long((0x009000 << 1) | ((adres2 & 0xFF00)>>8));	// Source address
	MRF24_Write_Long((0x00A000 << 1) | (adres2 & 0x00FF));		// Source address
	for(i=0x00B000;i<(0x00B000 + (Data_length_24<<12));i=i+0x1000)
	{
		MRF24_Write_Long((i << 1) | data[j]);					// Payload
		j++;
	}
	EXTINT |= MRF24_EINT;										// reset interrupts
	__enable_interrupts();
	MRF24_Write_Short(TXCON | TXNTRIG | TXNACKREQ);				// Start transmission with Ack
	// wait for interrupt (+/- 15ms)
	delay_ms(20);
}

void MRF24J40_receive(void)
{
	__disable_interrupts();
	MRF24_Write_Short(PANIDL | ((adres2 & 0xFF00)>>8));			// Set Receiver PAN address
	MRF24_Write_Short(PANIDH | (adres2 & 0x00FF));				// Set Receiver PAN address
	MRF24_Write_Short(SADRL | ((adres2 & 0xFF00)>>8));			// Set Receiver address
	MRF24_Write_Short(SADRH | (adres2 & 0x00FF));				// Set Receiver address
	MRF24_Write_Short(RXMCR | RX_MODE);							// Receiver in Normal mode: 0x00, Accept errors: 0x03, pancoordinator: 0x04
	MRF24_Write_Short(INTCON | RXIE);							// Enable Receive interrupt
	EXTINT |= MRF24_EINT;										// reset interrupts
	__enable_interrupts();
	// wait for interrupt
}

extern void MRF24J40_interrupt(void)
{
	char error_tx_stat = 0;
	char int_stat = 0;
	int i, j;
	char Received_Data[128];

	__disable_interrupts();
	int_stat = MRF24_Read_Short(INTSTAT);
	if(int_stat & TXNIF)										// Transmit mode, TX Normal FIFO Release Interrupt
	{
		error_tx_stat = MRF24_Read_Short(TXSTAT);
		if(error_tx_stat != 0)									// Check TX MAC Status Register for errors
		{
			errors++;
			lcd_clear(); 
			lcd_putchar(errors);
			MRF24J40_error();
		}
		busy = 0;												// Sending complete
		MRF24_Write_Short(INTCON | RXIE);						// Enable Receive interrupt
	}
	
	if(int_stat & RXIF)											// Receive mode, FIFO is full
	{
		MRF24_Write_Short(BBREG1 | RXDECINV);					// Disable receiver
		i = (MRF24_Read_Long(RXFIFO));							// Read RXFIFO, first byte is frame length, i = frame length -9 is data length
		
		for(j = 0 ; j<=i ; j++)									// Read data from FIFO buffer
		{
			Received_Data[j] = MRF24_Read_Long(RXFIFO | (j << 13));	// Read RXFIFO
		}
		
		int temp;
		for(temp=0;temp<=16;temp++)					// Maak eerst de externe chararray leeg
		{
			Received_Dataext[temp]=0;
		}
		
		temp=0;
		
		
		//code on "schone" data te printen geen Header
		char data_end = Received_Data[0] - 1; // bewaar het einde van de echte data (payload)
		
		int a,b; // 2 tellers
		for (a=10;a<data_end;a++) // begin de loop op 10 om de header eruit te halen
			{
			b=a-10;
			Received_Dataext[b]=Received_Data[a]; // zet de 10de in de 1ste, sla header over
			}
		// printen naar lcd in de main
		// einde toegevoegde code
		
		//int q;
		//for(q=0;q<=100;q++)
 		//{
 		//	IOSET0 |= (1<<8);
 		//	delay_us(500);
 		//	IOCLR0 |= (1<<8);
 		//	delay_us(500);
		//}
		nieuwe_data = 0xFFFF;			// als melding dat er nieuwe data is ontvangen
		
		MRF24_Rx_Data.Frame_Length = Received_Data[0];			// Parse data into MIWI_DATA_PKG Struct
		StringCopy(MRF24_Rx_Data.MAC_Control, &Received_Data[1], 2);
		MRF24_Rx_Data.Payload_Length = Received_Data[0] - 11;
		MRF24_Rx_Data.Seq_Nr = Received_Data[3];
		StringCopy(MRF24_Rx_Data.Destination_PAN, &Received_Data[4], 2);
		StringCopy(MRF24_Rx_Data.Destination_Addr, &Received_Data[6], 2);
		StringCopy(MRF24_Rx_Data.Source_Addr, &Received_Data[8], 2);
		StringCopy(MRF24_Rx_Data.Payload, &Received_Data[10], i - 11);
		MRF24_Rx_Data.Payload[i-11] = '\0';
		StringCopy(MRF24_Rx_Data.FCS, &Received_Data[10 + (i-11)], 2);

		MRF24_Write_Short(RXFLUSH | RX_FLUSH);					// Clear RX FIFO buffer
		MRF24_Write_Short(BBREG1);								// Enable receiver
	}
	EXTINT |= MRF24_EINT;										// reset interrupts
	__enable_interrupts();
}

//////////////////////////////////////////////////////////////////////////

void MRF24J40_error(void)
{
	short error;
	error = SPI_get(0xFF);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MRF24_Write_Short(unsigned short data)						// Write short address register
{
	MRF24_Read_Short(data | 0x100);								// Send data | Write bit
}

////////////////////////////////////////////////////////////////////////////////////////////////

unsigned short MRF24_Read_Short(unsigned short data)			// Read 16 bits register
{
	unsigned short temp;
	IOCLR0 = 1<<CS_MRF24;										// Chip Select Active
    temp= SPI_get(data);										// Send data
    IOSET0 = 1<<CS_MRF24;										// CS not active
    return temp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MRF24_Write_Long(unsigned int data)						// Write long address register
{
	MRF24_Read_Long(data | 0x1000);								// Send data | Write bit
}

///////////////////////////////////////////////////////////////////////////////////////////////

unsigned short MRF24_Read_Long(unsigned int data)
{
	unsigned int temp;
	SPI_set_12_bit();											// SPI is 12 bits breed
	while(SSPSR & 0x4)											// FIFO leeg?
    {
	temp = SSPDR;												// Leeg FIFO
    }
	IOCLR0 = 1<<CS_MRF24;										// Chip Select Active
    SPI_put(((data & 0xFFF000)>>12)|0x800);						// Send MSB 12 Bits | Long bit
    while (SSPSR & 0x10);										// klaar?
    temp = SSPDR;												// Leeg FIFO
    SPI_put(data&0xFFF);										// Send LSB 12 Bits
	while (SSPSR & 0x10);										// klaar?	
    SPI_set_16_bit();											// SPI is 16 bits breed
    IOSET0 = 1<<CS_MRF24;										// CS not active
	return SSPDR;	
}

////////////////////////////////////////////////////////////////////////////////////////////

void MRF24J40_sleep(void)
{
	MRF24_Write_Short(SOFTRST | RSTPWR);						// Power management reset
	MRF24_Write_Short(SLPACK | WAKECNT);						// Put to sleep
}

////////////////////////////////////////////////////////////////////////////////////////////

void MRF24J40_wake(void)
{
	IOSET0 = 1<<WAKE_24;										// Wake MRF24J40
	delay_ms(2);												// delay for stabilisation oscillator
	IOCLR0 = 1<<WAKE_24;
}

///////////////////////////////////////////////////////////////////////////////////////////

void StringCopy(char *destination, char *source, int length)
{
	int i = 0;
	
	while(i<length)
	{
		destination[i] = source[i];
		i++;
	}
}

