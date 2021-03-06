#include <mega328p.h>

struct RGB_COLOR_TYPE
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

struct RGB_COLOR_TYPE Red =          {255, 0, 0};
struct RGB_COLOR_TYPE Orange =       {255, 128, 0};
struct RGB_COLOR_TYPE Yellow =       {252, 255, 0};
struct RGB_COLOR_TYPE Green =        {0, 255, 0};
struct RGB_COLOR_TYPE GreenLime =    {191,255,  0};
struct RGB_COLOR_TYPE LightGreen =   {128,255,  0};
struct RGB_COLOR_TYPE Blue =         {0, 0, 255};
struct RGB_COLOR_TYPE NavyBlue =     {0, 0, 255};
struct RGB_COLOR_TYPE Violet =       {200, 0, 255};
struct RGB_COLOR_TYPE White =        {255, 255, 255};
struct RGB_COLOR_TYPE Cyan =         {  0,255,255};
struct RGB_COLOR_TYPE LightBlue =    {  0,128,255};
struct RGB_COLOR_TYPE Purple =       {128,  0,255};
struct RGB_COLOR_TYPE Black =        {0, 0, 0};


#define RED_CH              OCR0B //Определяем канал OCR1AL для Красного цвета
#define GREEN_CH            OCR0A //Определяем канал OCR1BL для Зеленого цвета
#define BLUE_CH             OCR1AL   //Определяем канал OCR2 для Синего цвета

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
/*Яркость (Brightness)
— самое простое преобразование.
При:
Shift=0 светодиод погашен
Shift=255 светодиод горит базовым цветом.
Все промежуточные значения Shift – это затемнение базового цвета.
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

void init_ports(void)
{
 // Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=(1<<CLKPCE);
CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port B initialization
DDRB=(1<<DDB5) | (1<<BLUE_DDR);
DDRD=(1<<GREEN_DDR) | (1<<RED_DDR);
}

//��������� ������������� ���
void init_pwm (void)
{

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

}


