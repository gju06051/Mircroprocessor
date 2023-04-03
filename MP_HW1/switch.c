//*****************************************************************************
//
// hello.c - Simple hello world example.
//
// Copyright (c) 2011-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 1.1 of the DK-TM4C123G Firmware Package.
//
//*****************************************************************************

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
extern unsigned int num_2();
extern unsigned int num_3();
extern unsigned int num_4();
extern unsigned int Switch_Input();
extern unsigned int Switch_Init();
//extern void ConfigureUART(void);
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Hello World (hello)</h1>
//!
//! A very simple ``hello world'' example.  It simply displays ``Hello World!''
//! on the display and is a starting point for more complicated applications.
//! This example uses calls to the TivaWare Graphics Library graphics
//! primitives functions to update the display.  For a similar example using
//! widgets, please see ``hello_widget''.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}
void stringDraw(tContext sCont){
    GrStringDraw(&sCont, "3", 1, 5, 0, 0);
    GrStringDraw(&sCont, "2", 1, 5, 10, 0);
    GrStringDraw(&sCont, "1", 1, 5, 20, 0);
    GrStringDraw(&sCont, "0", 1, 5, 30, 0);
}


unsigned char intTochar(int input){
	unsigned char output;
	switch(input){
	case 0:
		output = '0';
	case 1:
		output = '1';
	case 2:
		output = '2';
	case 3:
		output = '3';
	default:
		break;
	}
	return output;
}
//*****************************************************************************
//
// Print "Hello World!" to the display.
//
//*****************************************************************************
int main(void)
{
   //led();
    tContext sContext;
    ROM_FPULazyStackingEnable();
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);
    ConfigureUART();

    UARTprintf("Hello, world!\n");

    CFAL96x64x16Init();

    GrContextInit(&sContext, &g_sCFAL96x64x16);

    GrContextForegroundSet(&sContext, ClrWhite);

    GrContextFontSet(&sContext, g_psFontCm12);

//*****************************************************************************************
//*****************************************************************************************

       unsigned char n[4] = {'0', '0', '0', '0'};
       unsigned char num = '0';

        tRectangle tRect[4][2];

        Switch_Init();

        int i = 0;
 while(1){
	 do{
		 Switch_Input();
	     num = num_1();
	 }while(num == 'A');

     int j = 0;
     if (num=='B'){
    	 if(n[i]=='9')
    		 n[i]='0';
    	 else
    		 n[i]++;
     } else if (num=='C'){
    	 if(n[i]=='0')
    		 n[i]='9';
    	 else
    		 n[i]--;

     }

     GrContextForegroundSet(&sContext, ClrBlack);
     stringDraw(sContext);
     GrStringDraw(&sContext,intTochar(i), 1, 10, 52, 0);

     GrContextForegroundSet(&sContext, ClrWhite);
     stringDraw(sContext);
     GrStringDraw(&sContext,intTochar(i), 1, 10, 52, 0);

     for(j=0;j<4;j++){
    	 tRect[j][0].i16XMax=95;
    	 tRect[j][0].i16XMin=15;
    	 tRect[j][0].i16YMax=10*j + 9;
    	 tRect[j][0].i16YMin=10 * j + 1;

    	 tRect[j][1].i16XMax=15+ 80 * (n[j]-48)/9;
    	 tRect[j][1].i16XMin=tRect[j][0].i16XMin;
    	 tRect[j][1].i16YMax=tRect[j][0].i16YMax;
    	 tRect[j][1].i16YMin=tRect[j][0].i16YMin;

    	 GrContextForegroundSet(&sContext, ClrBlack);
         GrRectFill(&sContext, &tRect[j][0]);
         GrContextForegroundSet(&sContext, ClrWhite);
         GrRectDraw(&sContext, &tRect[j][0]);
         GrRectFill(&sContext, &tRect[j][1]);
     }
     GrStringDraw(&sContext, n, 4, 35, 52, -1);
     SysCtlDelay(8000000/3);
}   //end while
//*****************************************************************************************
//*****************************************************************************************
//    GrFlush(&sContext);


}      //end main


