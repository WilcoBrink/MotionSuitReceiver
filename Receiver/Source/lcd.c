#include "lcd.h"
#include "lpc214x.h"


void lcd_wait(){
      int loop=10000;  //more than enough
      //busy loop
      while(loop--);
}

void lcd_out_data4(unsigned char val){
     IOCLR0 |= (LCD_DATA);
     IOSET0 |= (val<<9);
}

void lcd_write_nibbles(unsigned char val){

     //higher-order byte
     lcd_en_set();
     lcd_out_data4((val>>4)&0x0F);
     lcd_en_clr();

     lcd_wait();

     //lower-order byte
     lcd_en_set();
     lcd_out_data4((val)&0x0F);
     lcd_en_clr();
     
     lcd_wait();
}

void lcd_write_control(unsigned char val){
     lcd_rs_clr();
     lcd_write_nibbles(val);
      lcd_wait();
}

void lcd_init(){
       
       IODIR0 |=(LCD_D4|LCD_D5|LCD_D6|LCD_D7|LCD_EN|LCD_RS);
       
       lcd_rs_clr();
       lcd_en_clr();
       //wait VDD raise > 4.5V
       lcd_wait(); 
       
       //dummy inst 
       lcd_write_nibbles(0x30);
       lcd_write_nibbles(0x30);
       lcd_write_nibbles(0x30);

       lcd_en_set();
       lcd_out_data4(0x2);
       lcd_en_clr();
       lcd_wait();

       lcd_write_nibbles(0x28);
       
       //LCD ON
       lcd_write_nibbles(0x0E);

       //Clear Display
       lcd_write_nibbles(0x01);

       //Entry mode
       lcd_write_nibbles(0x06);

}

void lcd_putchar(unsigned char c){

       lcd_rs_set();
       lcd_write_nibbles(c);
}

void lcd_print(char* str)
 {
      int i;
      int lcd_k=0,p=0;
      int cleardisp=1;
     {
      //limit 1 line display for prints
        if (cleardisp){
		    lcd_clear();      
		    lcd_cursor_home();
		    }
        for (i=0;i<16 && str[i]!=0;i++){
          if (lcd_k==8) { 
                for (p=0; p<32; p++){
                   lcd_cursor_right();
                  lcd_wait();
                  } }
                        
          lcd_k++;    
          lcd_putchar(str[i]);
          
        }
	}
}	

void lcd_clear(){
         lcd_write_control(0x01);
		 }
         
void lcd_cursor_home(){
	   lcd_write_control(0x02);
	   }
	   

void lcd_display_on(){
    lcd_write_control(0x0E);
    }
    
void lcd_display_off(){   
	lcd_write_control(0x08);
	}

void lcd_cursor_blink(){  
	lcd_write_control(0x0F);
	}
	
void lcd_cursor_on() {
    lcd_write_control(0x0E);
    }
    
void lcd_cursor_off() {  
	 lcd_write_control(0x0C);
	 }

void lcd_cursor_left(){   
	lcd_write_control(0x10);
	}
	
void lcd_cursor_right(){
  lcd_write_control(0x14);
  }
  
void lcd_display_sleft(){
 lcd_write_control(0x18);
 }
 
void lcd_display_sright(){
 lcd_write_control(0x1C);
 }
