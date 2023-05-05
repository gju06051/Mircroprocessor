#include "hal_data.h"
#include "Library.h"
#include "vocabulary.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

extern operation_state state;
extern volatile const uint8_t state_mask;

static const uint8_t number[10] = {0x81, 0xCF, 0x92, 0x86, 0xCC, 0xA4, 0xA0, 0x8F, 0x80, 0x84};

volatile uint32_t agt0_counter = 0U;
volatile uint32_t agt1_counter = 0U;

volatile uint8_t dutyRate = 0;
// extern volatile uint8_t degree;
// extern volatile servo_state rotate_mode;
volatile const uint32_t Timer_Period_2ms = 0x3A980U;
volatile const uint32_t Timer_Period_20ms = 0x249F00U;

extern uint16_t potentiometer_Rb;
extern CDS_t brightness;

extern char data[MESSAGE_MAX_SIZE];
extern volatile uint8_t uart_idx;

volatile uint8_t rand_num = 0;
volatile uint8_t quiz_num = 0;

volatile uint8_t quiz_num_10s = 0;
volatile uint8_t quiz_num_1s = 0;

volatile uint8_t ans_num = 0;
volatile uint8_t ans_num_10s = 0;
volatile uint8_t ans_num_1s = 0;

volatile uint8_t LED_PA08_Enable_Level = BSP_IO_LEVEL_HIGH;
volatile uint8_t LED_PA09_Enable_Level = BSP_IO_LEVEL_HIGH;
volatile uint8_t LED_PA10_Enable_Level = BSP_IO_LEVEL_HIGH;

uint8_t print_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

void hal_entry(void)
{
    Initial_Setting();
    while (true)
        if (state.task_mode_13 == 0x01)
        {
            quiz_num_10s = quiz_num / 10;
            quiz_num_1s = quiz_num % 10;
            ans_num_10s = ans_num / 10;
            ans_num_1s = ans_num % 10;
            print_data[0] = number[quiz_num_10s];
            print_data[1] = number[quiz_num_1s];
            print_data[2] = number[ans_num_10s];
            print_data[3] = number[ans_num_1s];
            if (quiz_num_10s == 0)
            {
                print_data[0] = 0xFF;
            }
            if (ans_num_10s == 0)
            {
                print_data[2] = 0xFF;
            }
            FND_Print_Data(print_data);
        }
        else
        {
            FND_Reset();
        }

#if BSP_TZ_SECURE_BUILD
    R_BSP_NonSecureEnter();
#endif
}

// External Interrupt
void R_External_Interrupt(external_irq_callback_args_t *p_args) // Interrupt Service Routine
{
    switch (p_args->channel)
    {
    case EXTERNAL_INTERRUPT_11: // assignment 3-1
        memset(&data, 0, sizeof(data));
        LED_PA08_Enable_Level ^= 0x01;
        R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[0], LED_PA08_Enable_Level);
        state.task_mode_11 ^= state_mask;
        break;
    case EXTERNAL_INTERRUPT_12: // assignment 3-2
        LED_PA09_Enable_Level ^= 0x01;
        R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[1], LED_PA09_Enable_Level);
        state.task_mode_12 ^= state_mask;
        break;
    case EXTERNAL_INTERRUPT_13: // assignment 3-3
        memset(&data, 0, sizeof(data));
        LED_PA10_Enable_Level ^= 0x01;
        R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[2], LED_PA10_Enable_Level);
        quiz_num = 0;
        ans_num = 0;
        FND_Reset();
        state.task_mode_13 ^= state_mask;
        break;
    }
}

// Timer Interrupt
void R_AGT0_Interrupt(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    agt0_counter++;

    if (state.task_mode_12 == 0x01)
    {
        ADC_Read_and_Convert();
        if (brightness == 1)
        {
            LED_PA09_Enable_Level ^= 0x01;
            R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[1], LED_PA09_Enable_Level);
            state.task_mode_12 ^= state_mask;
        }
        else
        {
            if (agt0_counter % 50 == 0)
            {
                dutyRate = 100 * potentiometer_Rb / 10000;
                R_GPT3->GTCCR[0] = Timer_Period_2ms * dutyRate / 100;
            }
        }
    }
    else
    {
        dutyRate = 0;
        R_GPT3->GTCCR[0] = Timer_Period_2ms * dutyRate / 100;
    }
}

void R_AGT1_Interrupt(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    agt1_counter++;

    if (state.task_mode_13 == 0x01)
    {
        if (agt1_counter % 500 == 0)
        {
            rand_num = rand() % 50;
            user_uart_write(voca[rand_num], strlen(voca[rand_num]));
            quiz_num++;
        }
    }
}

void user_uart_callback(uart_callback_args_t *p_args)
{
    // If SDR (Serial Data Register) is charged, this event (UART_EVENT_RX_CHAR) is occurred.
    if (p_args->event == UART_EVENT_RX_CHAR)
    {
        if (state.task_mode_11 == 0x01)
        {
            switch (p_args->data)
            {
            case CARRIAGE_RETURN:
                break;
            case LINE_FEED:
                user_uart_write(data, uart_idx);
                memset(&data, 0, sizeof(data));
                uart_idx = 0;
                break;
            default:
                // If Receive data Register is charged, take some data in byte.

                if ((48 <= p_args->data) & (p_args->data <= 57))
                {
                    data[uart_idx++] = (char)(p_args->data - 15);
                }
                else if (((65 <= p_args->data) & (p_args->data <= 90)) | ((97 <= p_args->data) & (p_args->data <= 122)))
                {
                    data[uart_idx++] = (char)(p_args->data - 7);
                }
                else
                {
                    data[uart_idx++] = (char)(p_args->data + 12);
                }
                break;
            }
        }
        if (state.task_mode_13 == 0x01)
        {
            // If SDR (Serial Data Register) is charged, this event (UART_EVENT_RX_CHAR) is occurred.
            switch (p_args->data)
            {
            case CARRIAGE_RETURN:
                break;
            case LINE_FEED:
                if (!strncmp((char *)&data[0], voca[rand_num], strlen(voca[rand_num])))
                {
                    Sound_Play();
                    ans_num++;
                }
                memset(&data, 0, sizeof(data));
                uart_idx = 0;
                break;
            default:
                data[uart_idx++] = (char)(p_args->data);
                break;
            }
        }
    }
}

void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
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
