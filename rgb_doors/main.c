/*******************************************************
This program was created by the
CodeWizardAVR V3.10 Advanced
Automatic Program Generator
© Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com
Chip type               : ATmega328P
Program type            : Application
AVR Core Clock frequency: 16,000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

#include <mega328p.h>
#include <delay.h>
#include <libs/functions_rgb_light_doors.h>            
 


void main(void)
{
init_ports();
init_pwm ();

while (1)
      {

        PORTB.5=1; 
        delay_ms(500);
        PORTB.5=0;
        delay_ms(500);  
        
        set_color(Green,BRIGHTNESS_100);
        delay_ms(1500);
        set_color(Green,BRIGHTNESS_75);
        delay_ms(1500);
        set_color(Green,BRIGHTNESS_50);
        delay_ms(1500);
        set_color(Green,BRIGHTNESS_25);
        delay_ms(1500);
        set_color(Green,BRIGHTNESS_10);
        delay_ms(1500);
        set_color(Green,BRIGHTNESS_5);
        delay_ms(1500); 
        set_color(Green,BRIGHTNESS_0);
        delay_ms(1500); 
        set_color(Orange,BRIGHTNESS_100);
        delay_ms(1500);
        set_color(Orange,BRIGHTNESS_75);
        delay_ms(1500);
        set_color(Orange,BRIGHTNESS_50);
        delay_ms(1500);
        set_color(Orange,BRIGHTNESS_25);
        delay_ms(1500);
        set_color(Orange,BRIGHTNESS_10);
        delay_ms(1500);
        set_color(Orange,BRIGHTNESS_5);
        delay_ms(1500); 
        set_color(Orange,BRIGHTNESS_0);
        delay_ms(1500); 
        set_color(Violet,BRIGHTNESS_100);
        delay_ms(1500);
        set_color(Violet,BRIGHTNESS_75);
        delay_ms(1500);
        set_color(Violet,BRIGHTNESS_50);
        delay_ms(1500);
        set_color(Violet,BRIGHTNESS_25);
        delay_ms(1500);
        set_color(Violet,BRIGHTNESS_10);
        delay_ms(1500);
        set_color(Violet,BRIGHTNESS_5);
        delay_ms(1500); 
        set_color(Violet,BRIGHTNESS_0);
        delay_ms(1500);             

       }
            
      
} 
      
