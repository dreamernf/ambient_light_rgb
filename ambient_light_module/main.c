#include "functions.h"
#include "ws2812b.h"
#include "uart.h"
#include "delay.h"
#include "nrf24.h"
#include "stm32_ub_led.h"

#include <stdio.h>


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

#define  count_pwm_steps    128

const int16_t RGB_PWM[count_pwm_steps] = {
		0 , 0 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 3 , 3 , 4 , 4 , 5 , 6,
		6 , 7 , 8 , 9 , 9 , 10 , 11 , 12 , 13 , 14 , 15 , 16 , 17 , 18 , 19 , 21,
		22 , 23 , 24 , 25 , 27 , 28 , 30 , 31 , 32 , 34 , 35 , 37 , 38 , 40 , 42 , 43,
		45 , 46 , 48 , 50 , 52 , 53 , 55 , 57 , 59 , 61 , 63 , 65 , 67 , 69 , 71 , 73,
		75 , 77 , 79 , 81 , 83 , 86 , 88 , 90 , 92 , 95 , 97 , 99 , 102 , 104 , 106 , 109,
		111 , 114 , 116 , 119 , 122 , 124 , 127 , 129 , 132 , 135 , 137 , 140 , 143 , 146 , 149 , 151,
		154 , 157 , 160 , 163 , 166 , 169 , 172 , 175 , 178 , 181 , 184 , 187 , 190 , 194 , 197 , 200,
		203 , 207 , 210 , 213 , 216 , 220 , 223 , 227 , 230 , 233 , 237 , 240 , 244 , 247 , 251 , 255
		};


#define  NUM_LEDS   7
#define  PERIOD   256


uint8_t number_color = 0;
uint8_t bright_rgb = 0;
uint8_t bright_rgb_nrfl = 0;
uint8_t bright_rgb_step = 0;


uint8_t number_color_tmp = 0;
uint8_t bright_rgb_tmp = 0;

//char buffer[30];


RGB_t leds[NUM_LEDS];

uint16_t i,j,k;

// Buffer to store a payload of maximum width
//uint8_t nRF24_payload[32];
uint8_t nRF24_payload[32];

// Pipe number
nRF24_RXResult pipe;

// Length of received payload
uint8_t payload_length = 5;

void set_color(struct RGB_COLOR_TYPE color, uint8_t brightness)
{

	leds[0].r=(color.R*brightness)/(PERIOD-1);
	leds[0].g=(color.G*brightness)/(PERIOD-1);
	leds[0].b=(color.B*brightness)/(PERIOD-1);

	leds[1].r=(color.R*brightness)/(PERIOD-1);
	leds[1].g=(color.G*brightness)/(PERIOD-1);
	leds[1].b=(color.B*brightness)/(PERIOD-1);

	leds[2].r=(color.R*brightness)/(PERIOD-1);
	leds[2].g=(color.G*brightness)/(PERIOD-1);
	leds[2].b=(color.B*brightness)/(PERIOD-1);

/*

	if (brightness == 0)
	{

		    leds[3].r=(color.R*brightness)/(PERIOD-1);
			leds[3].g=(color.G*brightness)/(PERIOD-1);
			leds[3].b=(color.B*brightness)/(PERIOD-1);
	}
	else
		{

		leds[3].r=color.R;
		leds[3].g=color.G;
		leds[3].b=color.B;

		}

	*/

	leds[3].r=(color.R*brightness)/(PERIOD-1);
	leds[3].g=(color.G*brightness)/(PERIOD-1);
	leds[3].b=(color.B*brightness)/(PERIOD-1);


	leds[4].r=(color.R*brightness)/(PERIOD-1);
	leds[4].g=(color.G*brightness)/(PERIOD-1);
	leds[4].b=(color.B*brightness)/(PERIOD-1);

	leds[5].r=(color.R*brightness)/(PERIOD-1);
	leds[5].g=(color.G*brightness)/(PERIOD-1);
	leds[5].b=(color.B*brightness)/(PERIOD-1);

	if (brightness == 0)
	{

		    leds[6].r=(color.R*brightness)/(PERIOD-1);
			leds[6].g=(color.G*brightness)/(PERIOD-1);
			leds[6].b=(color.B*brightness)/(PERIOD-1);
	}
	else
		{

		leds[6].r=color.R;
		leds[6].g=color.G;
		leds[6].b=color.B;

		}



}




int main() {

    SetSysClockTo72();
    Delay_Init();

    UB_Led_Init();
    UB_Led_Off(LED_DEBUG);

    UB_DigIn_Init();  // ������������� �������� ������

	ws2812b_Init();
	Delay_ms(50);

    while (!ws2812b_IsReady()); // wait


    init_spi();
    Delay_ms(50);

    init_nrf24l01();
    Delay_ms(50);

    set_color(Black,RGB_PWM[127]);
   	ws2812b_SendRGB(leds, NUM_LEDS);
   	Delay_ms(500);

    // The main loop
    while (1) {

        	if (nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY)
        	 {


        		        UB_Led_Off(LED_DEBUG);

        		        // Get a payload from the transceiver
						pipe = nRF24_ReadPayload(nRF24_payload, &payload_length);

						// Clear all pending IRQ flags
						nRF24_ClearIRQFlags();

						//����� �����
						number_color = 	nRF24_payload[0];

						//����� �������
						bright_rgb = nRF24_payload[1];

						//����������� �������� �������
						bright_rgb_step = nRF24_payload[2];

						UB_Led_On(LED_DEBUG);


						bright_rgb = set_brightness_slave(bright_rgb_step,bright_rgb);

						switch (number_color) {
						case 1: set_color(Red,RGB_PWM[bright_rgb]);break;
						case 2: set_color(Green,RGB_PWM[bright_rgb]); break;
						case 3: set_color(Blue,RGB_PWM[bright_rgb]); break;
						case 4: set_color(Orange,RGB_PWM[bright_rgb]); break;
						case 5: set_color(Yellow,RGB_PWM[bright_rgb]); break;
						case 6: set_color(GreenLime,RGB_PWM[bright_rgb]); break;
						case 7: set_color(LightGreen,RGB_PWM[bright_rgb]); break;
						case 8: set_color(NavyBlue,RGB_PWM[bright_rgb]); break;
						case 9: set_color(Violet,RGB_PWM[bright_rgb]); break;
						case 10: set_color(White,RGB_PWM[bright_rgb]); break;
						case 11: set_color(Cyan,RGB_PWM[bright_rgb]); break;
						case 12: set_color(LightGreen,RGB_PWM[bright_rgb]); break;
						case 13: set_color(Purple,RGB_PWM[bright_rgb]); break;
						case 14: set_color(Black,RGB_PWM[bright_rgb]); break;
						}

						ws2812b_SendRGB(leds, NUM_LEDS);

						Delay_ms(15);

        	  }

    }

}
