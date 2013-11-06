//lcd.h
//prototypes en define's


#define  LCD_D4  1<<9
#define  LCD_D5  1<<10
#define  LCD_D6  1<<11 
#define  LCD_D7  1<<12 
#define  LCD_EN  1<<13
#define  LCD_RS  1<<14   


#define  LCD_DATA           (LCD_D4|LCD_D5|LCD_D6|LCD_D7)





#define lcd_rs_set() IOSET0 |= LCD_RS
#define lcd_rs_clr() IOCLR0 |= LCD_RS
#define lcd_en_set() IOSET0 |= LCD_EN
#define lcd_en_clr() IOCLR0 |= LCD_EN


void lcd_wait(void);
void lcd_out_data4(unsigned char);
void lcd_write_nibbles(unsigned char);
void lcd_write_control(unsigned char);
void lcd_init(void);
void lcd_clear(void);
void lcd_cursor_home(void);
void lcd_display_on(void);
void lcd_display_off(void);
void lcd_cursor_blink(void);
void lcd_cursor_on(void);
void lcd_cursor_off(void);
void lcd_cursor_left(void);
void lcd_cursor_right(void);
void lcd_display_sleft(void);
void lcd_display_sright(void);
void lcd_putchar(unsigned char);
void lcd_print( char*);












