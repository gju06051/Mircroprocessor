#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "grlib/grlib.h"
#include "drivers/cfal96x64x16.h"
#include "utils/uartstdio.h"
#include "driverlib/gpio.h"

extern unsigned int num_1();
extern unsigned int Switch_Input();
extern unsigned int Switch_Init();
extern unsigned int LED_Init();
extern unsigned int Interrupt_Init();

int main(void)
{
	ROM_FPULazyStackingEnable();
	unsigned char num = '0';

	Switch_Init();
	LED_Init();
	Interrupt_Init();
	while (1)
	{
		do
		{
			Switch_Input();
			num = num_1();
		} while (num == 'A');

		SysCtlDelay(8000000 / 3);
	} // end while
} // end main
