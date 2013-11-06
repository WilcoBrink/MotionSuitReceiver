/******************************************************************************
* Filename		: 	Vic.c
* Description	: 	This file hold the functions used by the vectored interrupt controller
******************************************************************************/



/******************************************************************************
* Function		: 	VicSetup
* Description	: 	Link interrupt handel to source and vector
* Arguments		: 	-intHandler, this is the interrupt handler which will be called 
*								when the corresponding interrupt vector is activated.
*					-intType, defines if the interrupt is a FIQ(1) or IRQ(0)
					-intSource, this number holds the source of the interrupt.
*					-intVector, this is the interruptvector, which the handler will be linked to.
* Return		:	1 = ok, 0 =error
******************************************************************************/
#include "vic.h"
#include "LPC214x.h"

char VicSetup(unsigned int intHandler ,char intType, char intSource, char intVector)
/*instellen van de Vector Interupt Controller*/	
{
	
    
	if(intType)
	{
				VICIntSelect |=  (1<<intSource);		/*FIQ*/	
	}
	else
	{
				VICIntSelect &= ~(1<<intSource);               /*IRQ*/
	}
    
    switch(intVector)
    {
		
   		case 0:
				    VICVectCntl0  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr0  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;
  
  		case 1:
				    VICVectCntl1  = 0x20 | intSource;                       /* enable interrrupt slot and link to  source */
				    VICVectAddr1  = (unsigned)intHandler;     				/* link interrupt-afhandeler */
		break;
		
  		case 2:
				    VICVectCntl2  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr2  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;		
		
		case 3:
				    VICVectCntl3  = 0x20 | intSource;                       /* enable interrrupt slot and link to  source */
				    VICVectAddr3  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;
		
		case 4:
				    VICVectCntl4  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr4  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;

  		case 5:
				    VICVectCntl5  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr5  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;
		
  		case 6:
				    VICVectCntl6  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr6  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;
				
		case 7:
				    VICVectCntl7  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr7  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;

  		case 8:
				    VICVectCntl8  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr8  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;

  		case 9:
				    VICVectCntl9  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr9  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;
		
  		case 10:
				    VICVectCntl10  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr10  = (unsigned)intHandler;     				/* link interrupt-afhandeler */
		break;		

  		case 11:
				    VICVectCntl11  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr11  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;

  		case 12:
				    VICVectCntl12  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr12  = (unsigned)intHandler;     				/* link interrupt-afhandeler */
		break;

  		case 13:
				    VICVectCntl13  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr13  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;

  		case 14:
				    VICVectCntl14  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr14  = (unsigned)intHandler;     				 /* link interrupt-afhandeler */
		break;

  		case 15:
				    VICVectCntl15  = 0x20 | intSource;                        /* enable interrrupt slot and link to  source */
				    VICVectAddr15  = (unsigned)intHandler;     				  /* link interrupt-afhandeler */
		break;
		
		default :
			return 0;	/*ERROR*/
		break;
	}
    
    VICIntEnable  |= 1<<intSource;                  	//enable the interrupt source
    VICVectAddr   = 0xFF;                           	//execute
        
	return 1;
}

void DisableVectorInt(char Vector)
{
	VICIntEnClr |=(1 << Vector);
}
	

void EnableVectorInt(char Vector)
{
	VICIntEnable |=(1 << Vector);
}
