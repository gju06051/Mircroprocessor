#include "hal_data.h"
#include "Library.h"

// FND
const uint8_t fnd_num[16] = {0x81, 0xCF, 0x92, 0x86, 0xCC, 0xA4, 0xA0, 0x8F, 0x80, 0x84, 0x88, 0xE0, 0xB1, 0xC2, 0xB0, 0xB8};
uint8_t print_data[4] = {0x81, 0x81, 0xCF, 0x81};

// ADC
extern volatile uint8_t adc_mode;

// DAC
extern uint8_t sound_play1_buff;
extern uint8_t sound_play2_buff;

// Timer0
volatile uint32_t agt0_counter = 0U; // Timer Operation - Timer Counting
extern volatile uint32_t agt0_threshold;
volatile uint32_t count_val = 0U;    // decimal value
volatile uint8_t count_val_10s = 0U; // for change FND val
volatile uint8_t count_val_1s = 0U;  // for change FND val

// Timer1
volatile uint32_t agt1_counter = 0U; // ADC Monitoring
extern volatile uint32_t agt1_threshold;

// Uart
uint8_t uart_idx = 0U;
extern struct MSG_PACKET msg_rx;
char test_data[11] = {0};

// Write Protection
volatile uint8_t write_protection_unlock_stage = 0U;
volatile uint32_t write_protect_time = 3000U; // WP in 3[S]
extern uint8_t root;

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

// extern variable

void hal_entry(void)
{
    // Initial Setting
    Initial_Setting();

    while (true)
    {
        // FND Out
        FND_Print_Data(print_data);

        // DAC Out
        if (sound_play1_buff > 0)
        {
            Sound_Play1();
            sound_play1_buff -= 1;
        }
        if (sound_play2_buff > 0)
        {
            Sound_Play2();
            sound_play2_buff -= 1;
        }
    }

#if BSP_TZ_SECURE_BUILD
    R_BSP_NonSecureEnter();
#endif
}

// GPIO Operation
// LED3 : Write Protection State
// LED4 : Admin/Root Mode State

// External Interrupt (Switch)
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
        if (adc_mode == 1)
        {
            adc_mode = 3;
        }
        else if (adc_mode == 3)
        {
            adc_mode = 1;
        }
        print_data[2] = fnd_num[adc_mode];
        break;
    case EXTERNAL_INTERRUPT_14: // SW4 : 7-Segment Digit 1~2 Reset (to initial value "00")
        count_val = 0;
        print_data[0] = fnd_num[0];
        print_data[1] = fnd_num[0];
        break;
    }
}

// Timer Interrupt
void R_AGT0_Interrupt(timer_callback_args_t *p_args) // Counting Value, FND Digit 1~2 change
{
    FSP_PARAMETER_NOT_USED(p_args);
    agt0_counter++;

    // Counting Value
    if (agt0_counter % agt0_threshold == 0)
    {
        if (count_val < 99)
        {
            count_val++;
        }
        else
        {
            count_val = 0;
        }
    }
    count_val_10s = count_val / 10;
    count_val_1s = count_val % 10;
    print_data[0] = fnd_num[count_val_10s];
    print_data[1] = fnd_num[count_val_1s];
}
void R_AGT1_Interrupt(timer_callback_args_t *p_args) // Write Protection, ADC
{
    FSP_PARAMETER_NOT_USED(p_args);
    agt1_counter++;

    // Write Protection
    if (write_protection_unlock_stage == 1)
    {
        if (agt1_counter % write_protect_time == 0)
        {
            write_protection_unlock_stage = 0;
        }
    }

    // ADC
    if (adc_mode != 0)
    {
        if (agt1_counter % agt1_threshold == 0)
        {
            if (root == 1)
            {
                if (adc_mode == 1)
                {
                    ADC_Read_and_Convert();
                    Monitor_Variable_Resistor();
                }
                else if (adc_mode == 3)
                {
                    ADC_Read_and_Convert();
                    Monitor_CDS();
                }
            }
        }
    }
}

// Uart Interrupt
// Get MSG and Check Flag
void user_uart_callback(uart_callback_args_t *p_args)
{
    // If SDR (Serial Data Register) is charged, this event (UART_EVENT_RX_CHAR) is occurred.
    if (p_args->event == UART_EVENT_RX_CHAR)
    {
        switch (p_args->data)
        {
        case 0x02: // start of text
            // clear msg_rx
            memset(&msg_rx, 0, sizeof(msg_rx));
            uart_idx = 1;
            break;
        case 0x03: // end of text
            Select_Type();
            uart_idx = 0;
            break;
        default:
            user_uart_read(p_args->data, uart_idx);
            uart_idx++;
        }
    }
}
void can_callback(can_callback_args_t *p_args)
{
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
