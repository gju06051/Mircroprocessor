
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
extern unsigned int Switch_Input();
extern unsigned int Switch_Init();

extern unsigned int LED_Init();
extern unsigned int LED_On();
extern unsigned int LED_Off();

extern unsigned int Blink_slow();
extern unsigned int Blink_fast();
extern unsigned int UART_Init();
extern unsigned int PRINT_LEDMODE();

// extern void ConfigureUART(void);
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
void __error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// Print "Hello World!" to the display.
//
//*****************************************************************************

int main(void)
{
    ROM_FPULazyStackingEnable();

    Switch_Init();
    LED_Init();
    UART_Init();
    PRINT_LEDMODE();

    char num = '0';
    while (1)
    {
        do
        {
            Switch_Input();
            num = num_1();
        } while (num == 'A');
        if (num == 'B')
        { // A : LEDON()
            LED_On();
        }
        else if (num == 'C')
        { // B : LEDOFF()
            LED_Off();
        }
        else if (num == 'D')
        { // C : BLINKSLOW()
            Blink_slow();
        }
        else if (num == 'E')
        { // D : BLINKFAST()
            Blink_fast();
        }

        SysCtlDelay(8000000 / 3);
    } // end while
    //*****************************************************************************************
    //*****************************************************************************************
    //    GrFlush(&sContext);

} // end main
