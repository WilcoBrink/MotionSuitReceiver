
/* ==========================================================================
   UART.c                                                            
   (c) 2012 ProjectGroep 6                                                                         
   handler for UART
   ========================================================================== */

	#include "uart.h"
	#include "LPC214x.h"
	#include "config.h"
	
	static void UART_putnum(unsigned int num, unsigned char deel,char teken);
	
	// Stelt gebruikte I/O pinnen en baudrate in.
	void UART_init(void)
	{
		// Initialize Pin Select Block for Tx and Rx
	    PINSEL0 = (PINSEL0 & ~UART0_MSK_PINSEL0) | UART0_VAL_PINSEL0;
	    // Enable FIFO's and reset them
	    U0FCR = 0x7;
	    // Set DLAB and word length set to 8bits */
	    U0LCR = 0x83;
	    // Set baudrate
	    U0DLL = (unsigned char)UART_BAUD;
	    U0DLM = UART_BAUD>>8;
	 	// add fractional prescaler data? -> (5/(5+7)) for 9600 baud, add formula 		//for calculation in 214x.h/uart.h?
	    // Clear DLAB
	    U0LCR = 0x3;
	    // Clear interrupt bits
	    U0IER = 0x00;
	}

	// Stuurt meegegeven string uit op de UART
	void UART_put(char *c)
	{
	    unsigned int FiFoCount=0;
	    signed int i=0;
	
	    // Wacht tot de buffer leeg is
	    while(!(U0LSR & (1<<6)));
	
	    // Ga door tot het eind van de string ('\0') is berijkt
	    while(c[i]) 
	    {
	        U0THR=c[i];
	        i++;
	        FiFoCount++;
	        if(FiFoCount==16) 
	        {
	            FiFoCount=0;
	            /* Buffer vol. Wacht tot ie leeg is */
	            while(!(U0LSR & (1<<5)));
	        }
	    }
	}

	// Stuurt meegegeven karakter uit op de UART
	void UART_putchar(char c)
	{
	    // Wacht tot de buffer leeg is
	    while(!(U0LSR & (1<<6)));
	
	    // Karakter doorsturen
	    U0THR=c;
	}
	
	// Stuurt meegegeven waarde binair uit op de UART
	void UART_putbin(unsigned char num)
	{
	    unsigned char c[9];
	    unsigned char waarde = 128;
	    unsigned int i=0;
	
	    // omzetten naar binair 
	    while(i<8) 
	    {
	        if(num >= waarde) 
	        {
	            num = num - waarde;
	            c[i]='1';
	        }
	        else 
	            c[i]='0';
	
	        waarde >>= 1;
	        i++;
	    }
	
	    // string afsluiten
	    c[8]='\0';
	
	    // en doorsturen 
	    UART_put((char*)c);
	}
	
	
	// Stuurt meegegeven getal uit op de UART
	void UART_putint(int num)
	{
		unsigned int i;
		char teken=0;
		if (num<0)
		{
			i=~num+1;
			teken=1;
		}
		else {
			i=num;
		}

	    UART_putnum(i, 10,teken);
	}
		
	// Stuurt meegegeven waarde hexdecimaal uit op de UART
	void UART_puthex(unsigned int num)
	{
	    // Is niet echt nodig, maar staat wel interessant.... 
	    #if 0
	    UART_put("0x");
	    #endif
	    UART_putnum(num, 16,0);
	}
	
	// Ontvang ? karakter via de UART
	signed char UART_get(void)
	{
	    // Kijk of er wat in de buffer staat
	    if (U0LSR & 0x01) // return karakter
	        return U0RBR;
	    return -1;
	}
	
	// Ontvang ? karakter via de UART (unsigned char om met hex-waardes te kunnen werken)
	unsigned char UART_get_unsigned(void)
	{
	    // Kijk of er wat in de buffer staat
	    if (U0LSR & 0x01) // return karakter
	        return U0RBR;
	    return -1;
	}
	
	// Stuurt meegegeven getal uit op de UART in het aangegeven getallenstelsel
	static void UART_putnum(unsigned int num, unsigned char deel,char teken)
	{
	    static unsigned char chars[17] = "0123456789ABCDEF";
	    unsigned int rest;
	    signed char c[16];
	    signed int i=15;
	    // Zet de integer om naar een string
	    if(num==0) 
	    {
	        c[i]='0';
	        i--;
	    }


	    else 
	    {

	    	while(num>0)
	        {

	            rest=num%deel;
	            num/=deel;
	            c[i]=chars[rest];
	            i--;
	
	            if(i==0) // it ends here
	                num=0;
	        }

        	if (teken==1){
        		c[i]='-';
        		i--;
        	}
	    }
	
	    // Wacht tot de buffer leeg is
	    while(!(U0LSR & (1<<6)));
	
	    // Stuur de string uit
	    while(i<15) 
	    {
	        i++;
	        U0THR=c[i];
	    }
	}
