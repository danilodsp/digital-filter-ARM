#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"

// AIN0/PE7.
// - ADC0 peripheral
// - GPIO Port E peripheral (for AIN0 pin)
// - AIN0 - PE3
//
// UART
// - UART0 peripheral
// - GPIO Port A peripheral (for UART0 pins)
// - UART0RX - PA0
// - UART0TX - PA1

uint32_t pui32ADC0Value[8];
uint32_t pui2[1];
uint32_t value0;
uint32_t value1;
uint32_t value2;
uint32_t value3;
uint32_t value4;
uint32_t value5;
uint32_t value6;
uint32_t value7;

//uint32_t divider = 0;
//uint32_t ulADC0Period = 0;
//uint8_t val = 0x00;

void
InitUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // osc interno 16MHz
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTStdioConfig(0, 115200, 16000000);
//    byte = UARTCharGet(UART0_BASE);
}

int
main(void)
{
    // 20 MHz (200 MHz / 10) PLL
	// 40 MHz (200 MHz / 5) PLL
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    unsigned long ulSysPeriod = SysCtlClockGet();
/*    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));*/

    InitUART();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_INT_PIN_0);

    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_DIR_MODE_OUT);

    //ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_TIMER, 0); // uint32_t ui32Base, uint32_t ui32SequenceNum, uint32_t ui32Trigger, uint32_t ui32Priority
    //ADCSequenceConfigure(ADC1_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PLL | ADC_CLOCK_RATE_FULL, 1);

    ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V); // maior precisão
    //ADCReferenceSet(ADC1_BASE, ADC_REF_EXT_3V);

    //ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0); // PE3
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1); // PE2
    //ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1); // PE2
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH2); // PE1
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH3); // PE0
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH4); // PD3
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH5); // PD2
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH6); // PD1
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH7 | ADC_CTL_IE | ADC_CTL_END); // PD0

    //ADCSequenceEnable(ADC0_BASE, 3);
    ADCSequenceEnable(ADC0_BASE, 0);
    //ADCSequenceEnable(ADC1_BASE, 3);

    //ADCIntClear(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 0);
    ADCIntEnable(ADC0_BASE, 0);

    //ADCProcessorTrigger(ADC0_BASE, 0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 1000);
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerControlTrigger(TIMER0_BASE, TIMER_A, true);

    IntEnable(INT_ADC0SS0_TM4C123);
    IntMasterEnable();

    while(1)
    {
 /*       //ADCProcessorTrigger(ADC0_BASE, 3);
    	ADCProcessorTrigger(ADC0_BASE, 0);
    	//ADCProcessorTrigger(ADC1_BASE, 3);

        while(!ADCIntStatus(ADC0_BASE, 0, false))
        {
        }

        //while(!ADCIntStatus(ADC1_BASE, 3, false))
        //        {
        //        }

        //ADCIntClear(ADC0_BASE, 0);
        ADCIntClear(ADC0_BASE, 0);
        //ADCIntClear(ADC1_BASE, 3);

        //ADCSequenceDataGet(ADC0_BASE, 0, pui32ADC0Value);
        ADCSequenceDataGet(ADC0_BASE, 0, pui32ADC0Value);
        //ADCSequenceDataGet(ADC1_BASE, 3, pui2);

        value0 = pui32ADC0Value[0]>>4;
        value1 = pui32ADC0Value[1]>>4;
        value2 = pui32ADC0Value[2]>>4;
        value3 = pui32ADC0Value[3]>>4;
        value4 = pui32ADC0Value[4]>>4;
        value5 = pui32ADC0Value[5]>>4;
        value6 = pui32ADC0Value[6]>>4;
        value7 = pui32ADC0Value[7]>>4;

        //UARTprintf("%d", value);
        //UARTprintf("%c", (char) 0xAA);
        //UARTCharPut(UART0_BASE, ((char) 0xFF));
        UARTCharPut(UART0_BASE, (char) value0);
        UARTCharPut(UART0_BASE, (char) value1);
        UARTCharPut(UART0_BASE, (char) value2);
        UARTCharPut(UART0_BASE, (char) value3);
        UARTCharPut(UART0_BASE, (char) value4);
        UARTCharPut(UART0_BASE, (char) value5);
        UARTCharPut(UART0_BASE, (char) value6);
        UARTCharPut(UART0_BASE, (char) value7);

        // delay (ciclos) = 3 * parameter = 250ms
        //SysCtlDelay(SysCtlClockGet() / 12);
        SysCtlDelay(1);*/
    }
}

interrupt void IntADC0(void)
{
	ADCIntClear(ADC0_BASE, 0);
	ADCSequenceDataGet(ADC0_BASE, 0, pui32ADC0Value);

//	val = ~val;
//	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, val);
//	ulADC0Period = ADCClockConfigGet(ADC0_BASE, &divider);

    value0 = pui32ADC0Value[0]>>4;
    value1 = pui32ADC0Value[1]>>4;
    value2 = pui32ADC0Value[2]>>4;
    value3 = pui32ADC0Value[3]>>4;
    value4 = pui32ADC0Value[4]>>4;
    value5 = pui32ADC0Value[5]>>4;
    value6 = pui32ADC0Value[6]>>4;
    value7 = pui32ADC0Value[7]>>4;

    UARTCharPut(UART0_BASE, (char) value0);
    UARTCharPut(UART0_BASE, (char) value1);
    UARTCharPut(UART0_BASE, (char) value2);
    UARTCharPut(UART0_BASE, (char) value3);
    UARTCharPut(UART0_BASE, (char) value4);
    UARTCharPut(UART0_BASE, (char) value5);
    UARTCharPut(UART0_BASE, (char) value6);
    UARTCharPut(UART0_BASE, (char) value7);
}
