#include "Library.h"

volatile operation_state state;
volatile uint8_t state_mask = 0x01;
volatile servo_state rotate_mode;

extern volatile const uint32_t Timer_Period_20ms;
extern volatile const uint32_t Timer_Period_2ms;

volatile uint8_t degree = 0;

const double SERVO_MINIMUM_DUTY = 0.03;
const double SERVO_MAXIMUM_DUTY = 0.12;
const double SERVO_EACH_DEGREE = (SERVO_MAXIMUM_DUTY - SERVO_MINIMUM_DUTY) / 180;

CDS_t brightness;
const float ADC_CONST = (float)(3.3 / 4096);
uint16_t value;

uint16_t potentiometer_mV = 0, potentiometer_Ra = 0, potentiometer_Rb = 0;
uint16_t temperature_mV;
int16_t temperature;
uint16_t cds_data;

char data[MESSAGE_MAX_SIZE] = {0};
char tail[2] = {CARRIAGE_RETURN, LINE_FEED};
volatile uint8_t uart_idx = 0;

extern unsigned char rawData[155616];

void Initial_Setting()
{
    state.task_mode_11 = 0;
    state.task_mode_12 = 0;
    state.task_mode_13 = 0;
    state.task_mode_14 = 0;

    srand((unsigned)time(NULL));

    LED_Reset();
    FND_Reset();
    IRQ_Initial_Setting();
    AGT0_Setting();
    AGT1_Setting();
    R_GPT_Setting();
    R_GPT3_Setting();
    R_ADC0_Setting();
    R_DAC0_Setting();
    R_SCI_UART_Setting();
}

///// ///// ///// ///// Setting ///// ///// ///// /////
void IRQ_Initial_Setting()
{
    // Interrupt ICU Setting
    R_ICU_ExternalIrqOpen(&g_external_irq11_ctrl, &g_external_irq11_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq12_ctrl, &g_external_irq12_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq13_ctrl, &g_external_irq13_cfg);
    // R_ICU_ExternalIrqOpen(&g_external_irq14_ctrl, &g_external_irq14_cfg);

    IRQ_Enable();
}
void IRQ_Enable()
{
    // Interrupt Set Enable Register Setting (Enable)
    R_ICU_ExternalIrqEnable(&g_external_irq11_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq12_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq13_ctrl);
    // R_ICU_ExternalIrqEnable(&g_external_irq14_ctrl);
}
void IRQ_Disable()
{
    // Interrupt Set Disable Register Setting (Disable)
    R_ICU_ExternalIrqDisable(&g_external_irq11_ctrl);
    R_ICU_ExternalIrqDisable(&g_external_irq12_ctrl);
    R_ICU_ExternalIrqDisable(&g_external_irq13_ctrl);
    // R_ICU_ExternalIrqDisable(&g_external_irq14_ctrl);
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

// void R_GPT0_Setting() // Servo Motor PWM Generator
// {
//     R_GPT0->GTCR_b.MD = 0U;   // GPT32EH0 Count Mode Setting (Saw-wave PWM Mode)
//     R_GPT0->GTCR_b.TPCS = 0U; // GPT32EH0 Clock Source Prescale Setting (PCLKD/1)
//
//     R_GPT0->GTPR = Timer_Period_20ms - 1; // GPT32EH0 Counting Maximum Cycle Setting
//     R_GPT0->GTCNT = 0;                    // GPT32EH0 Counter Initial Value Setting
//
//     R_GPT0->GTIOR_b.GTIOA = 9U;                                // GPT32EH0 Output Mode Setting (PWM Mode)
//     R_GPT0->GTIOR_b.OAE = 1U;                                  // GPT32EH0 Output Enable Setting (Output Enable)
//     R_GPT0->GTCCR[0] = Timer_Period_20ms * SERVO_MINIMUM_DUTY; // GPT32EH0 Initial Duty Setting
//     R_GPT0->GTCR_b.CST = 1U;                                   // GPT32EH0 Count Start
// }

void R_ADC0_Setting()
{
    R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
}

// void R_ADC1_Setting()
//
//    R_ADC_Open(&g_adc1_ctrl, &g_adc1_cfg);
//    R_ADC_ScanCfg(&g_adc1_ctrl, &g_adc1_channel_cfg);
//

void R_DAC0_Setting()
{
    R_DAC_Open(&g_dac0_ctrl, &g_dac0_cfg);
    R_DAC_Start(&g_dac0_ctrl);
}

void R_SCI_UART_Setting()
{
    R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
}

///// ///// ///// ///// Function ///// ///// ///// /////
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

void Rotate_Servo()
{
    double temp_calc = (SERVO_MINIMUM_DUTY + SERVO_EACH_DEGREE * (float)degree);

    R_GPT0->GTCCR[0] = (uint32_t)(Timer_Period_20ms * temp_calc);
}

void Potentiometer_Read()
{
    uint16_t ch0_adc_result;

    R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_0, &ch0_adc_result);
    potentiometer_mV = (uint16_t)(ch0_adc_result * ADC_CONST * 1000);
    potentiometer_Rb = (uint16_t)(potentiometer_mV * 3.0303);
    potentiometer_Ra = (uint16_t)(10000 - potentiometer_Rb);
}
// void Thermal_Sensor_Read()
// {
//     uint16_t ch1_adc_result;
//
//     R_ADC_Read(&g_adc1_ctrl, ADC_CHANNEL_1, &ch1_adc_result);
//     temperature_mV = (uint16_t)(ch1_adc_result * ADC_CONST * 1000);
//
//     if (temperature_mV >= 500)
//         temperature = (int16_t)((temperature_mV - 500) / 10);
//     else
//         temperature = (int16_t)((500 - temperature_mV) / 10);
// }
void Cds_Sensor_Read()
{
    uint16_t ch2_adc_result;

    R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_2, &ch2_adc_result);
    if (ch2_adc_result >= 1000)
    {
        cds_data = ch2_adc_result;
        brightness = CDS_dark;
    }
    else
    {
        cds_data = ch2_adc_result;
        brightness = CDS_light;
    }
}

void ADC_Read_and_Convert(void)
{
    adc_status_t status;

    R_ADC_ScanStart(&g_adc0_ctrl);
    // R_ADC_ScanStart(&g_adc1_ctrl);
    status.state = ADC_STATE_SCAN_IN_PROGRESS;
    while (ADC_STATE_SCAN_IN_PROGRESS == status.state)
    {
        R_ADC_StatusGet(&g_adc0_ctrl, &status);
        // R_ADC_StatusGet(&g_adc1_ctrl, &status);
    }

    Potentiometer_Read();
    // Thermal_Sensor_Read();
    Cds_Sensor_Read();
}

void Sound_Play()
{
    for (uint32_t i = 0; i < sizeof(rawData); i += 2)
    {
        value = (uint16_t)(rawData[i] | (rawData[i + 1] << 8));
        R_DAC_Write(&g_dac0_ctrl, value);
        R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MICROSECONDS);
    }
}

void user_uart_write(const char *msg, uint32_t const bytes)
{
    char *string = (char *)calloc(1, sizeof(char) * MESSAGE_MAX_SIZE);

    strcpy(string, msg);
    strcat(string, tail);

    R_SCI_UART_Write(&g_uart0_ctrl, (unsigned char *)string, bytes + 2);
    free(string);
}