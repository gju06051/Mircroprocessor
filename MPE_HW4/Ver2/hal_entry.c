#include "hal_data.h"
#include "Library.h"

// hal_entry
extern uint8_t print_data[4];    // FND print data
extern uint8_t sound_play1_buff; // DAC Sound1 buff count
extern uint8_t sound_play2_buff; // DAC Sound2 buff count

// R_AGT0_Interrupt - Timer0
volatile uint32_t agt0_counter = 0U;
volatile uint32_t count_val = 0U;        // Initial count value is 0
extern volatile uint32_t agt0_threshold; // Timer0 timer period - 1ms per each

// R_AGT1_Interrupt - Timer1
volatile uint32_t agt1_counter = 0U;
const uint32_t wp_unlock_stage_time = 3000U; // WP in 3[S] - 1ms per each
extern volatile uint8_t write_protection_unlock_stage = 0U;
extern volatile uint32_t agt1_threshold; // Timer1 timer period - 1ms per each
extern volatile uint8_t adc_mode;        // Mode 0 : not active, Mode 1 : VR, Mode 3 : CDS
extern volatile uint8_t root;            // root permission

// Uart Interrupt
uint8_t uart_idx = 0U;
extern struct MSG_PACKET msg_rx;

void hal_entry(void)
{
    // Initial Setting
    Initial_Setting(); // Setting all peripheral devices for project

    while (true)
    {
        // FND Out
        FND_Print_Data(print_data);

        // DAC Out
        if (sound_play1_buff > 0)
        {
            Sound_Play1();         // Play Sound1
            sound_play1_buff -= 1; // buffer count down
        }
        if (sound_play2_buff > 0)
        {
            Sound_Play2();         // Play Sound2
            sound_play2_buff -= 1; // buffer count down
        }
    }

#if BSP_TZ_SECURE_BUILD
    R_BSP_NonSecureEnter();
#endif
}

// External Interrupt (Switch 1, 2, 3, 4)
void R_External_Interrupt(external_irq_callback_args_t *p_args)
{
    switch (p_args->channel)
    {
    case EXTERNAL_INTERRUPT_11: // SW1 : Button Click Counting + Transmit Click to Uart Bus
        Monitor_Tact_Switch();
        break;
    case EXTERNAL_INTERRUPT_12: // SW2 : CAN Message Transmitting
        R_CAN_TX_CALL();
        break;
    case EXTERNAL_INTERRUPT_13: // SW3 : ADC Mode Changing + FND Digit3 Changing
        ADC_Mode_Change();
        break;
    case EXTERNAL_INTERRUPT_14: // SW4 : 7-Segment Digit 1~2 Reset (to initial value "00")
        Count_Init();
        break;
    }
}

// Timer Interrupt (AGT0) - Period : 1ms
void R_AGT0_Interrupt(timer_callback_args_t *p_args) // Counting Value, FND Digit 1~2 change
{
    FSP_PARAMETER_NOT_USED(p_args);
    agt0_counter++;

    // Check Counting Value
    if (agt0_counter % agt0_threshold == 0) // If threshold is reached
    {
        if (count_val < 99)
        {
            count_val++; // Counting Value + 1
        }
        else
        {
            count_val = 0; // Counting Value Reset if reached 100
        }
    }

    // FND Digit 1~2 change
    FND_Count_Update();
}

// Timer Interrupt (AGT1) - Period : 1ms
void R_AGT1_Interrupt(timer_callback_args_t *p_args) // Write Protection, ADC
{
    FSP_PARAMETER_NOT_USED(p_args);
    agt1_counter++;

    // Write Protection Unlock Timer 3[s]
    if (write_protection_unlock_stage == 1)
    {
        if (agt1_counter % wp_unlock_stage_time == 0) // Check 3[s]
        {
            write_protection_unlock_stage = 0; // 3[s] is over, unlock stage cancel
        }
    }

    // ADC Montioring
    if (adc_mode != 0) // ADC Mode 0 means not have any timer period
    {                  // ADC timer period is initialized first to use
        if (root == 1) // check root permission
        {
            if (agt1_counter % agt1_threshold == 0) // If threshold is reached
            {
                ADC_Read_and_Convert();
                if (adc_mode == 1) // ADC Mode 1 : Variable Resistor
                {
                    Monitor_Variable_Resistor();
                }
                else if (adc_mode == 3) // ADC Mode 3 : CDS Sensor
                {
                    Monitor_CDS();
                }
            }
        }
    }
}

// Uart Interrupt - Receive MSG Packet and Decode
void user_uart_callback(uart_callback_args_t *p_args)
{
    // If SDR (Serial Data Register) is charged, this event (UART_EVENT_RX_CHAR) is occurred.
    if (p_args->event == UART_EVENT_RX_CHAR)
    {
        switch (p_args->data) // Check received data (STX, ETX, Data)
        {
        case STX: // start of text - clear message for new message receiv
            memset(&msg_rx, 0, sizeof(msg_rx));
            uart_idx = 1;
            break;
        case ETX: // end of text - Operating by message
            Select_Type();
            uart_idx = 0;
            break;
        default: // MSG Packet Data decoding (STX ~ ETX)
            Message_Decode(p_args->data, uart_idx);
            uart_idx++;
        }
    }
}

void can_callback(can_callback_args_t *p_args)
{
    // empty callback function for CAN receive - not used
}

void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0
        R_FACI_LP->DFLCTL = 1U;
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable();

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable()
{
}
#endif