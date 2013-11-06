/*
    ARM IP interface
    uart.h: UART-driver testboard
*/

void UART_init(void);
void UART_put(char *c);
void UART_putchar(char c);
void UART_putbin(unsigned char num);
void UART_putint(unsigned int num);
void UART_puthex(unsigned int num);
signed char UART_get(void);
unsigned char UART_get_unsigned(void);
