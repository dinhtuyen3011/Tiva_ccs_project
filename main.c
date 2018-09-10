#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"

#include <driverlib/timer.h>
#include <driverlib/interrupt.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <driverlib/adc.h>
#include <driverlib/gpio.h>
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"

uint32_t Data[2];
char str[100];

float Voltage1,Voltage2;

void ConfigureUART(void);

void config_adc(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0 |GPIO_PIN_1 );
	ADCSequenceConfigure(ADC0_BASE,0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1 |ADC_CTL_IE |ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 0);

}
void config_timer(void)
{
	uint32_t ui32Period;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);

	TimerEnable(TIMER0_BASE,TIMER_A);
	ui32Period =SysCtlClockGet()/2;
	TimerLoadSet(TIMER0_BASE,TIMER_A,ui32Period-1);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);
}
void UART0_IntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, ui32Status);
}
void ConfigureUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
    UARTStdioConfig(0, 115200, 16000000);
    IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	IntMasterEnable();
}

//chuong trinh ngat
void Timer0IntHandler(void)
{

	TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);

	ADCIntClear(ADC0_BASE,0);
	ADCProcessorTrigger(ADC0_BASE, 0); //Thuc nhien trigger adc
	while (!ADCIntStatus(ADC0_BASE,0, false))	//Neu trang thai khoi tao cua ADC ok thi thuc hien buoc tiep theo
	{
	}
	ADCSequenceDataGet(ADC0_BASE,0,(uint32_t *)&Data);
	Voltage1 =(Data[0])*3.3/4095;
	Voltage2 =(Data[1])*3.3/4095;



	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
	}
	//UARTprintf("Hello\r");

	float x=1.2;
	sprintf(str,"%.2f",x);
	UARTprintf("%s\r",str);
	memset(str, 0, sizeof(str));
}
int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);	//Cau hinh 3 LED la output, o day chi su dung led 2 thoi

	config_adc();
	config_timer();
	ConfigureUART();
	while(1)
	{

	}

}

