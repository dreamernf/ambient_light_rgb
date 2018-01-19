#include "ws2812b.h"
#include "uart.h"
#include "functions.h"
#include "delay.h"
#include "nrf24.h"

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

#define NUM_LEDS    1

uint8_t number_color = 0;


RGB_t leds[NUM_LEDS];

uint32_t i,j,k;

// Buffer to store a payload of maximum width
uint8_t nRF24_payload[32];

// Pipe number
nRF24_RXResult pipe;

// Length of received payload
uint8_t payload_length = 5;


void set_color(struct RGB_COLOR_TYPE color)
{

	leds[0].r=color.R;
	leds[0].g=color.G;
	leds[0].b=color.B;

}



int main() {

    SetSysClockTo72();
    Delay_Init();
    UART_Init(115200);
	init_spi();
	init_nrf24l01();
    ws2812b_Init();

    while (!ws2812b_IsReady()); // wait
    UART_SendStr("WS2812B READY is OK.\r\n");

	leds[0].r=127;
	leds[0].g=0;
	leds[0].b=127;
	ws2812b_SendRGB(leds, NUM_LEDS);
	Delay_ms(1500);

    // The main loop
    while (1) {
    	if (nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
    		// Get a payload from the transceiver
    		pipe = nRF24_ReadPayload(nRF24_payload, &payload_length);

    		// Clear all pending IRQ flags
			nRF24_ClearIRQFlags();

			// Print a payload contents to UART
			UART_SendStr("RCV PIPE#");
			UART_SendInt(pipe);
			UART_SendStr(" PAYLOAD:>");
			UART_SendBufHex((char *)nRF24_payload, payload_length);
			UART_SendStr("<\r\n");

			number_color = nRF24_payload[0];


    		switch (number_color) {
    		case 1: set_color(Red);break;
    		case 2: set_color(Green); break;
    		case 3: set_color(Blue); break;
    		case 4: set_color(Yellow); break;
    		case 5: set_color(Orange); break;
    		case 6: set_color(Purple); break;
    		case 7: set_color(White); break;
    		case 8: set_color(Cyan); break;
    		}

    		ws2812b_SendRGB(leds, NUM_LEDS);
			Delay_ms(50);


    	}
    }


}
