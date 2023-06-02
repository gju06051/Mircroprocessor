#include "Library.h"

// Digit Counter
extern volatile uint32_t count_val;  // decimal value
volatile uint8_t count_val_10s = 0U; // for change FND val
volatile uint8_t count_val_1s = 0U;  // for change FND val

// FND
uint8_t print_data[4] = {0x81, 0x81, 0xCF, 0x81};

// DC/SERVO Motor
volatile const uint32_t Timer_Period_2ms = 0x3A980U;
volatile const uint32_t Timer_Period_20ms = 0x249F00U;
const double SERVO_MINIMUM_DUTY = 0.03;
const double SERVO_MAXIMUM_DUTY = 0.12;
const double SERVO_EACH_DEGREE = (SERVO_MAXIMUM_DUTY - SERVO_MINIMUM_DUTY) / 180;

// CAN
can_frame_t g_can_tx_frame;
const uint32_t can_id = 0x41;
const uint8_t data_field_can[] = "ML_FINAL";

//----------------------------------------Setting-------------------------------------
void Initial_Setting()
{
    R_GPIO_Setting();
    LED_Reset();
    FND_Reset();
    IRQ_Initial_Setting();
    AGT0_Setting();
    AGT1_Setting();
    R_GPT_Setting();
    R_GPT0_Setting();
    R_GPT3_Setting();
    R_ADC0_Setting();
    R_DAC0_Setting();
    R_SCI_UART_Setting();
    R_CAN_Setting();
}
void R_GPIO_Setting()
{
    R_PORT9->PCNTR1 |= IO_PCNTR1_MASK;

    R_PORT10->PCNTR1_b.PDR |= 0x0700;
    R_PORT11->PCNTR1_b.PDR |= 0x0001;

    LED_Reset();
}
void IRQ_Initial_Setting()
{
    // Interrupt ICU Setting
    R_ICU_ExternalIrqOpen(&g_external_irq11_ctrl, &g_external_irq11_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq12_ctrl, &g_external_irq12_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq13_ctrl, &g_external_irq13_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq14_ctrl, &g_external_irq14_cfg);

    IRQ_Enable();
}
void IRQ_Enable()
{
    // Interrupt Set Enable Register Setting (Enable)
    R_ICU_ExternalIrqEnable(&g_external_irq11_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq12_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq13_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq14_ctrl);
}
void IRQ_Disable()
{
    // Interrupt Set Disable Register Setting (Disable)
    R_ICU_ExternalIrqDisable(&g_external_irq11_ctrl);
    R_ICU_ExternalIrqDisable(&g_external_irq12_ctrl);
    R_ICU_ExternalIrqDisable(&g_external_irq13_ctrl);
    R_ICU_ExternalIrqDisable(&g_external_irq14_ctrl);
}
void AGT0_Setting()
{
    R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    R_AGT_Start(&g_timer0_ctrl);
}
void AGT1_Setting()
{
    R_AGT_Open(&g_timer1_ctrl, &g_timer1_cfg);
    R_AGT_Start(&g_timer1_ctrl);
}
void R_GPT_Setting()
{
    R_MSTP->MSTPCRD_b.MSTPD5 = 0U; // GPT32EHx (x=0 to 3) Module Stop State Cancel

    R_PMISC->PWPR_b.B0WI = (uint8_t)0U;  // PFSWE Bit Write Protection Disable
    R_PMISC->PWPR_b.PFSWE = (uint8_t)1U; // PmnPFS Register Write Protection Disable

    R_PFS->PORT[4].PIN[3].PmnPFS_b.PMR = 1U;  // GTIOC3A Pin Peripheral Function Enable
    R_PFS->PORT[4].PIN[3].PmnPFS_b.PSEL = 3U; // Port m/n Pin Function Select: GTIOC3A (GPT3 PWM Output)

    R_PFS->PORT[4].PIN[15].PmnPFS_b.PMR = 1U;  // GTIOC0A Pin Peripheral Function Enable
    R_PFS->PORT[4].PIN[15].PmnPFS_b.PSEL = 3U; // Port m/n Pin Function Select: GTIOC0A (GPT0 PWM Output)
}
void R_GPT3_Setting() // DC Motor PWM Generator
{
    R_GPT3->GTCR_b.MD = 0U;   // GPT32EH3 Count Mode Setting (Saw-wave PWM Mode)
    R_GPT3->GTCR_b.TPCS = 0U; // GPT32EH3 Clock Source Prescale Setting (PCLKD/1)

    R_GPT3->GTPR = Timer_Period_2ms - 1; // GPT32EH3 Counting Maximum Cycle Setting
    R_GPT3->GTCNT = 0U;                  // GPT32EH3 Counter Initial Value Setting

    R_GPT3->GTIOR_b.GTIOA = 9U; // GPT32EH3 Output Mode Setting (PWM Mode)
    R_GPT3->GTIOR_b.OAE = 1U;   // GPT32EH3 Output Enable Setting (Output Enable)
    R_GPT3->GTCCR[0] = 0U;      // GPT32EH3 Initial Duty Setting
    R_GPT3->GTCR_b.CST = 1U;    // GPT32EH3 Count Start
}
void R_GPT0_Setting() // Servo Motor PWM Generator
{
    R_GPT0->GTCR_b.MD = 0U;                                    // GPT32EH0 Count Mode Setting (Saw-wave PWM Mode)
    R_GPT0->GTCR_b.TPCS = 0U;                                  // GPT32EH0 Clock Source Prescale Setting (PCLKD/1)
    R_GPT0->GTPR = Timer_Period_20ms - 1;                      // GPT32EH0 Counting Maximum Cycle Setting
    R_GPT0->GTCNT = 0;                                         // GPT32EH0 Counter Initial Value Setting
    R_GPT0->GTIOR_b.GTIOA = 9U;                                // GPT32EH0 Output Mode Setting (PWM Mode)
    R_GPT0->GTIOR_b.OAE = 1U;                                  // GPT32EH0 Output Enable Setting (Output Enable)
    R_GPT0->GTCCR[0] = Timer_Period_20ms * SERVO_MINIMUM_DUTY; // GPT32EH0 Initial Duty Setting
    R_GPT0->GTCR_b.CST = 1U;                                   // GPT32EH0 Count Start
}
void R_ADC0_Setting() // Variable Resistor & CDS Sensor
{
    R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);            // ADC0 config open
    R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg); // ADC0 channel open
}
void R_DAC0_Setting() // Speaker
{
    R_DAC_Open(&g_dac0_ctrl, &g_dac0_cfg); // DAC0 config open
    R_DAC_Start(&g_dac0_ctrl);             // DAC0 start
}
void R_SCI_UART_Setting()
{
    R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
}
void CAN_Frame_Setting(uint32_t id, uint8_t dlc, uint8_t *data)
{
    g_can_tx_frame.id = id;
    g_can_tx_frame.id_mode = CAN_ID_MODE_STANDARD;
    g_can_tx_frame.type = CAN_FRAME_TYPE_DATA;

    g_can_tx_frame.data_length_code = dlc;

    if (strlen((const char *)data) <= 8)
        memcpy(g_can_tx_frame.data, data, dlc);
    else
        memset(g_can_tx_frame.data, 0, sizeof(uint8_t) * STANDARD_CAN_FRAME);
}
void R_CAN_Setting()
{
    R_CAN_Open(&g_can0_ctrl, &g_can0_cfg);
    CAN_Frame_Setting(can_id, STANDARD_CAN_FRAME, data_field_can);
}

void LED_Reset()
{
    uint8_t idx = 0;

    /* LED Initialization */
    for (idx = 0; idx < LED_INDEX; idx++)
        R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[idx], BSP_IO_LEVEL_HIGH);
}
void FND_Reset()
{
    uint8_t idx = 0;

    /* 7-Segment Initialization */
    for (idx = 0; idx < SEGMENT_INDEX; idx++)
    {
        if (idx < DIGIT_INDEX)
            R_IOPORT_PinWrite(&g_ioport_ctrl, segment_digit[idx], BSP_IO_LEVEL_LOW);
        R_IOPORT_PinWrite(&g_ioport_ctrl, segment_led_pin[idx], BSP_IO_LEVEL_HIGH);
    }
}
void FND_Display_Data(uint8_t digit, uint8_t data)
{
    uint8_t key = 0, idx = 0;

    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MICROSECONDS);
    FND_Reset();

    /* 7-Segment Digit Selection */
    R_IOPORT_PinWrite(&g_ioport_ctrl, segment_digit[digit], BSP_IO_LEVEL_HIGH);

    /* 7-Segment LED Pin Setting */
    for (idx = 0; idx < SEGMENT_INDEX; idx++)
    {
        key = (uint8_t)(data >> (7 - idx) & 0x01);
        R_IOPORT_PinWrite(&g_ioport_ctrl, segment_led_pin[idx], key);
    }
}
void FND_Print_Data(uint8_t *string)
{
    uint8_t idx = 0;

    if (sizeof(string) != DIGIT_INDEX)
        return;

    for (idx = 0; idx < DIGIT_INDEX; idx++)
        FND_Display_Data(idx, string[idx]);
}

void Count_Init()
{
    count_val = 0;              // Init count val to zero
    print_data[0] = fnd_num[0]; // change FND digit
    print_data[1] = fnd_num[0]; // change FND digit
}

void FND_Count_Update()
{
    count_val_10s = count_val / 10;         // Counting Value 10s digit
    count_val_1s = count_val % 10;          // Counting Value 1s digit
    print_data[0] = fnd_num[count_val_10s]; // Change Digit0
    print_data[1] = fnd_num[count_val_1s];  // Change Digit1
}

void R_CAN_TX_CALL()
{
    R_CAN_Write(&g_can0_ctrl, 0, &g_can_tx_frame);
}