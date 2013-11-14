/******************************************************************************
* Filename		: 	config.h
* Description	: 	This file hold the header information used by UART0
******************************************************************************/

	#define BAUDRATE    		9600		// baudrate UART
	#define	SYS_BASE_FREQ		60000000
	#define UART0_VAL_PINSEL0	(0x5<<0)	// PINSEL0 Value for UART0
	#define UART0_MSK_PINSEL0   (0xf<<0)    // PINSEL0 Mask for UART0
	#define UART1_VAL_PINSEL0	(0x5<<16)   // PINSEL0 Value for UART1
	#define UART1_MSK_PINSEL0	(0xf<<16)   // PINSEL0 Mask for UART1	
	#define UART_BAUD (unsigned short) ((SYS_BASE_FREQ/BAUDRATE)/16)
