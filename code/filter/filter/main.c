//*****************************************************************************
//
// Filter 1
//

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

float x[N];
float y[N];
float xn, yn, yn2;
char tmp;
float tmp2;
char tmp3[9];
bool menos;
bool ponto;

float vf[N];
unsigned char vb[N];

void ftoa(float f,char *buf);

int
main(void)
{
	int n=0, i=0, c=0, j=0;
	menos = false;
	ponto = false;

	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	//UARTStdioConfig(0, 115200, 16000000);

	do
	{
		vb[n] = UARTCharGet(UART0_BASE);

		/*if(tmp == 32){ // espaco
			if(menos)
				x[n] = -x[n];

			menos = false;
			ponto = false;
			n++;
			x[n] = 0;
			c = 0;
		}
		if(tmp == 45){ // sinal menos
			menos = true;
			ponto = false;
			n++;
			c = 0;;
		}
		if(tmp == 46){ // ponto
			ponto = true;
			c = 0;
		}
		if(tmp == 33){
			for(j=0;j<(n+1);j++){
				ftoa(x[j], &tmp3);

				do{
					UARTCharPut(UART0_BASE, tmp3[i++]);
				}while(i<sizeof tmp3-1);
				i=0;
				UARTCharPut(UART0_BASE, 0x20);
			}
			n = 0;
		}

		if(tmp > 47){
			tmp = tmp - 48;

			if(ponto){
				tmp2 = tmp*(pow(10,c-1));
				x[n] = x[n] + tmp2;
				c--;
			}
			else{
				tmp2 = (x[n]*pow(10,c));
				x[n] = tmp2 + tmp;
				c++;
			}
		}*/



		//xn = x[n];

		/*if(n > 3){
			for(i=0;i<4;i++){
				yn = yn + x[n - i]*(0.25);
			}
		}

		yn = xn;

		if(n>0){
			yn2 = x[n-1];
		}
		else{
			yn2 = yn;
		}*/



		n++;
	}
	while(n<100);

	memcpy(vf, vb, sizeof(unsigned char)*400);

	for(i=0;i<n;i++){
		vf[i] = vf[i] + 1;
	}

	memcpy(vb, vf, sizeof(float)*100);

	for(i=0;i<n;i++){
		UARTCharPut(UART0_BASE, vb[i]);
	}
	//UARTCharPut(UART0_BASE, *vb);

	//UARTCharPut(UART0_BASE, '@');

	return(0);
}

void ftoa(float f,char *buf)
{
	int pos=0,ix,dp,num;
	if (f<0)
	{
		buf[pos++]='-';
		f = -f;
	}
	dp=0;
	while (f>=10.0)
	{
		f=f/10.0;
		dp++;
	}
	for (ix=1;ix<8;ix++)
	{
		num = f;
		f=f-num;
		if (num>9)
			buf[pos++]='#';
		else
			buf[pos++]='0'+num;
		if (dp==0) buf[pos++]='.';
		f=f*10.0;
		dp--;
	}
}
