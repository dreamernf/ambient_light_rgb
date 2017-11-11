/*******************************************************
This program was created by the
CodeWizardAVR V3.10 Advanced
Automatic Program Generator
� Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
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


struct RGB_COLOR_TYPE
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

struct RGB_COLOR_TYPE Red =      {255, 0, 0};
struct RGB_COLOR_TYPE Orange =   {252, 179, 0};
struct RGB_COLOR_TYPE Yellow =   {252, 255, 0};
struct RGB_COLOR_TYPE Green =    {0, 255, 0};
struct RGB_COLOR_TYPE Blue =     {0, 226, 251};
struct RGB_COLOR_TYPE NavyBlue = {0, 0, 255};
struct RGB_COLOR_TYPE Violet =   {200, 0, 255};
struct RGB_COLOR_TYPE White =    {255, 255, 255};
struct RGB_COLOR_TYPE Black =    {0, 0, 0};

#define RED_CH              OCR0B //���������� ����� OCR1AL ��� �������� �����
#define GREEN_CH            OCR0A //���������� ����� OCR1BL ��� �������� �����
#define BLUE_CH             OCR1AL   //���������� ����� OCR2 ��� ������ �����

#define  BRIGHTNESS_100     255
#define  BRIGHTNESS_75      192
#define  BRIGHTNESS_50      127
#define  BRIGHTNESS_25      64
#define  BRIGHTNESS_10      32
#define  BRIGHTNESS_5       16
#define  BRIGHTNESS_0       0
#define  LIGHT_OFF          0

#define RED_DDR   DDD5 
#define GREEN_DDR DDD6  
#define BLUE_DDR  DDB1

void set_color(struct RGB_COLOR_TYPE color, unsigned char brightness)
{
/*������� (Brightness)
� ����� ������� ��������������.
���:
Shift=0 ��������� �������
Shift=255 ��������� ����� ������� ������.
��� ������������� �������� Shift � ��� ���������� �������� �����.
R_shift = (R_base * Shift) / 256
G_shift = (G_base * Shift) / 256
B_shift = (B_base * Shift) / 256
*/
if (brightness==BRIGHTNESS_100)
 {
   RED_CH = color.R;
   GREEN_CH = color.G;
   BLUE_CH = color.B;
} 
  else
    {
     RED_CH = (color.R*brightness)/256;
     GREEN_CH = (color.G*brightness)/256;
     BLUE_CH = (color.B*brightness)/256;         
    }

}

 
//��������� ������������� ���
void init_pwm (void)
{
// Input/Output Ports initialization
// Port B initialization
DDRB=(1<<DDB5) | (1<<BLUE_DDR);
DDRD=(1<<GREEN_DDR) | (1<<RED_DDR);


// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 16000,000 kHz
// Mode: Phase correct PWM top=0xFF
// OC0A output: Non-Inverted PWM
// OC0B output: Non-Inverted PWM
// Timer Period: 0,031875 ms
// Output Pulse(s):
// OC0A Period: 0,031875 ms Width: 0 us
// OC0B Period: 0,031875 ms Width: 0 us
TCCR0A=(1<<COM0A1) | (0<<COM0A0) | (1<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (1<<WGM00);
TCCR0B=(0<<WGM02) | (0<<CS02) | (0<<CS01) | (1<<CS00);
TCNT0=0x00;

GREEN_CH=LIGHT_OFF;
RED_CH=LIGHT_OFF;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 16000,000 kHz
// Mode: Ph. correct PWM top=0x00FF
// OC1A output: Non-Inverted PWM
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer Period: 0,031875 ms
// Output Pulse(s):
// OC1A Period: 0,031875 ms Width: 0 us
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(1<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (1<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;

BLUE_CH=LIGHT_OFF;

OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 0 Interrupt(s) initialization
//TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (0<<TOIE0);
// Timer/Counter 1 Interrupt(s) initialization
//TIMSK1=(0<<ICIE1) | (0<<OCIE1B) | (0<<OCIE1A) | (0<<TOIE1);
}

void main(void)
{
// Declare your local variables here
unsigned char shift;

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=(1<<CLKPCE);
CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

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
      
