//*****************************************************************************
//
// Filter ADC
//
// Filtering with 1000 samples, 20 sections, 120 coefficients, using ADC as the input signal
// by Danilo Pena

#define N 1000
#define T 120
#define SECTIONS 20
#define DELAY 2

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "utils/uartstdio.h"
#include <math.h>

unsigned char bytes[4*T];

float vf[T];

float sinal[N];
//float aux[N];
float x[N];
//float y[N];
uint32_t value;
uint8_t y;

float a0[SECTIONS];
float a1[SECTIONS];
float a2[SECTIONS];
float b0[SECTIONS];
float b1[SECTIONS];
float b2[SECTIONS];
float w[SECTIONS][3] = {0};

int
main(void)
{
	int n=0, m=0, i=0, s=0;
	float value_adc;
	float value32;
	float aux;
	uint32_t pui32ADC0Value[1];

	/*SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_16MHZ);*/
	//	SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
	//			SYSCTL_XTAL_16MHZ);
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	//ADC
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
			ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 3);
	ADCIntClear(ADC0_BASE, 3);

	do
	{
		bytes[n] = UARTCharGet(UART0_BASE);
		n++;
	}
	while(n<(4*T));

	memcpy(vf, bytes, sizeof(unsigned char)*4*T);

	memcpy(a0, vf, sizeof(float)*SECTIONS);
	memcpy(a1, vf+SECTIONS, sizeof(float)*SECTIONS);
	memcpy(a2, vf+2*SECTIONS, sizeof(float)*SECTIONS);
	memcpy(b0, vf+3*SECTIONS, sizeof(float)*SECTIONS);
	memcpy(b1, vf+4*SECTIONS, sizeof(float)*SECTIONS);
	memcpy(b2, vf+5*SECTIONS, sizeof(float)*SECTIONS);

	value = 0;
	y=0;

	do
	{
		ADCProcessorTrigger(ADC0_BASE, 3);
		while(!ADCIntStatus(ADC0_BASE, 3, false)){}

		ADCIntClear(ADC0_BASE, 3);
		ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);

		//UARTprintf("AIN0 = %4d\r", pui32ADC0Value[0]);

		//		SysCtlDelay(SysCtlClockGet() / 12);

		//value32 = (float) pui32ADC0Value[0];
		value_adc = pui32ADC0Value[0];
		value32 = value_adc;

		//value = (pui32ADC0Value[0]>>4);
		//value++;

		// Filtro IIR
		for(s=0; s<SECTIONS; s++){
			w[s][2] = a0[s]*value32 - a1[s]*w[s][1] - a2[s]*w[s][0];
			value32 = b0[s]*w[s][2] + b1[s]*w[s][1] + b2[s]*w[s][0];
			w[s][0] = w[s][1];
			w[s][1] = w[s][2];
		}
		value = floor(((value32/8000.0) + 0.5)*255);
		y = (value & 0xFF);
//		y = (pui32ADC0Value[0]>>4);

		GPIOPinWrite(GPIO_PORTB_BASE, (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7), y);

		//m++;
	}
	while(1);

	//
	//	memcpy(x, sinal, sizeof(float)*N);
	//
	//	// Filtro direct-form II SOS
	//	memcpy(aux, x, sizeof(unsigned char)*4*N);
	//
	//	for(i=2; i<(N+2); i++){
	//		for(s=0; s<SECTIONS; s++){
	//			w[s][2] = a0[s]*aux[i-2] - a1[s]*w[s][1] - a2[s]*w[s][0];
	//			aux[i-2] = b0[s]*w[s][2] + b1[s]*w[s][1] + b2[s]*w[s][0];
	//			w[s][0] = w[s][1];
	//			w[s][1] = w[s][2];
	//		}
	//		y[i-2] = aux[i-2];
	//	}
	//
	//	memcpy(bytes, y, sizeof(float)*N);

	//	for(i=0;i<N;i++){
	//		//SysCtlDelay(5000);
	//		//UARTCharPut(UART0_BASE, bytes[i]);
	//		value = (int) sinal[i];
	//		value = value * 255;
	//
	//		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, value);
	//	}

	return(0);
}
