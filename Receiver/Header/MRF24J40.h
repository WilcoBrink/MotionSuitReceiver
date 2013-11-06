/* ========================================================================== */
/*                                                                            */
/*   MRF24J40.h                                                               */
/*   (c) 2011 Udo van Heteren                                                 */
/*                                                                            */
/*   MRF24J40 MiWi Driver													  */
/*                                                                            */
/* ========================================================================== */

#ifndef MRF24J40_H
#define MRF24J40_H

// Vic setup
#define MRF24_PRIOR			1
#define EINT3_INT_SLOT		0x11
#define MRF24_EINT			1<<3

// MRF24J40 I/O
#define RESET_MRF	15
#define WAKE_24		31
#define CS_MRF24	29
#define INT  		30

typedef struct
{
	char Frame_Length;
	char Payload_Length;
	char MAC_Control[2];
	char Seq_Nr;
	char Destination_PAN[2];
	char Destination_Addr[2];
	char Source_PAN[2];
	char Source_Addr[2];
	char Payload[128];
	char FCS[2];
} MRF24_DATA_PKG;

// Channel Selection
#define	CH_11		0x02
#define	CH_12		0x12
#define	CH_13		0x22
#define	CH_14		0x32
#define	CH_15		0x42
#define	CH_16		0x52
#define	CH_17		0x62
#define	CH_18		0x72
#define	CH_19		0x82
#define	CH_20		0x92
#define	CH_21		0xA2
#define	CH_22		0xB2
#define	CH_23		0xC2
#define	CH_24		0xD2
#define	CH_25		0xE2
#define	CH_26		0xF2

// MRF24J40 Registers
#define RXMCR		0x00 << 9			// Shift short address and add R/W bit
#define PANIDL		0x01 << 9
#define PANIDH		0x02 << 9
#define SADRL		0x03 << 9
#define SADRH		0x04 << 9
#define RXFLUSH		0x0D << 9
#define PACON2		0x18 << 9
#define TXCON		0x1B << 9
#define	TXPEND		0x21 << 9
#define WAKECON		0x22 << 9
#define TXSTAT		0x24 << 9
#define	TXTIME		0x27 << 9
#define SOFTRST 	0x2A << 9
#define TXSTBL		0x2E << 9
#define INTSTAT		0x31 << 9
#define INTCON		0x32 << 9
#define SLPACK		0x35 << 9
#define RFCTL		0x36 << 9
#define BBREG1		0x39 << 9
#define BBREG2		0x3A << 9
#define BBREG6		0x3E << 9
#define CCAEDTH		0x3F << 9
#define RFCON0		0x200 << 13			// Shift long address and add R/W bit
#define RFCON1		0x201 << 13
#define RFCON2		0x202 << 13
#define RFCON3		0x203 << 13
#define RFCON6		0x206 << 13
#define RFCON7		0x207 << 13
#define RFCON8		0x208 << 13
#define SLPCON0		0x211 << 13
#define SLPCON1		0x220 << 13
#define	RXFIFO		0x300 << 13

// MRF24J40 Configuration
#define CCA			0x8E
#define	CCACSTH		0x38
#define CCAED		0x60
#define CLKOUTEN	0x20
#define CLKRECVR	0x10
#define INTEDGE		0x00
#define INTDISABLE	0xFF
#define FIFOEN_24	0x80
#define FRAMECTRL1	0x61
#define FRAMECTRL2	0x88
#define HEADER_SIZE	0x09
#define MLIFS		0x7C
#define PLLEN		0x80
#define RESET_MRF24	0x07
#define RFOPT		0x02
#define RFSTBL		0x95
#define RFVCO		0x10
#define RSSIMODE	0x40
#define RSTPWR		0x04
#define RXDECINV	0x04
#define RXIE		0xF7
#define RXIF		0x08
#define	RX_FLUSH	0x01
#define RX_MODE		0x00
#define RXRST		0x05
#define RXTXRST		0x04
#define RXTXSET		0x00
#define SLPCLKDIV	0x01
#define SLPCLKEN	0x00
#define SLPCLKSEL	0x80
#define	TURNTIME	0x38
#define TXFIL		0x80
#define	TXNACKREQ	0x04
#define TXNIE		0xFE
#define TXNIF		0x01
#define TXNTRIG		0x01
#define TXONST		0x18
#define TXPOWER		0x00
#define TXRST		0x06
#define VCOOPT		0x01
#define WAKECNT		0x5F


// MRF24J40 Functions
void MRF24J40_init(unsigned int adres);
void Print_MRF24(MRF24_DATA_PKG *Miwi_Data);
void MRF24J40_send_byte(char data, unsigned short adres);
void MRF24J40_send_string(char tekst[32], unsigned short adres);
void MRF24J40_send(char *data, int Data_length_24, unsigned int adres);
void MRF24J40_receive(void);
void MRF24J40_interrupt(void);
void MRF24J40_error(void);
void MRF24_Write_Short(unsigned short data);
unsigned short MRF24_Read_Short(unsigned short data);
void MRF24_Write_Long(unsigned int data);
unsigned short MRF24_Read_Long(unsigned int data);
void MRF24J40_sleep(void);
void MRF24J40_wake(void);
void StringCopy(char *destination, char *source, int length);

#endif /* MRF24J40_h */
