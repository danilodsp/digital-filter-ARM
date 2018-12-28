//*****************************************************************************
//
// Filter 2
//
// Filtering with 100 samples via UART (115200), with the moving average filter (1/4)
// by Danilo Pena

#define N 100

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include <math.h>

unsigned char bytes[400];

float x[N];
float y[N] = {0};

int
main(void)
{
	int n=0, i=0, j=0;

	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	//UARTStdioConfig(0, 115200, 16000000);

	do
	{
		bytes[n] = UARTCharGet(UART0_BASE);

		n++;
	}
	while(n<400);

	memcpy(x, bytes, sizeof(unsigned char)*4*N);

	for(j=0;j<100;j++){
		if(n > 3){
			for(i=0;i<4;i++){
				y[j] = y[j] + x[j - i]*(0.25);
			}
		}
	}

	memcpy(bytes, y, sizeof(float)*N);

	for(i=0;i<n;i++){
		//SysCtlDelay(5000);
		UARTCharPut(UART0_BASE, bytes[i]);
	}

	return(0);
}
