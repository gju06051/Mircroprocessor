#include "Library.h"

volatile operation_state state;
volatile const uint8_t state_mask = 0x01;

// LED
volatile uint8_t LED_PA08_Enable_Level = BSP_IO_LEVEL_HIGH;
volatile uint8_t LED_PA09_Enable_Level = BSP_IO_LEVEL_HIGH;
volatile uint8_t LED_PA10_Enable_Level = BSP_IO_LEVEL_HIGH;
volatile uint8_t LED_PB00_Enable_Level = BSP_IO_LEVEL_HIGH;

// FND
extern const uint8_t fnd_num[16];
extern uint8_t print_data[4];

// DC Motor
volatile const uint32_t Timer_Period_2ms = 0x3A980U;
volatile uint8_t dutyRate = 50;                                // 50%
volatile uint8_t L293_CH0_Enable_Level = BSP_IO_LEVEL_LOW;     // Off
volatile uint8_t L293_CH0_Direction_Level = BSP_IO_LEVEL_HIGH; // CCW
bsp_io_port_pin_t L293_CH0_Enable = BSP_IO_PORT_09_PIN_00;
bsp_io_port_pin_t L293_CH0_Direction = BSP_IO_PORT_09_PIN_01;
const char dc_on_key[4] = {0x30, 0x30, 0x30, 0x30};
const char dc_off_key[4] = {0x30, 0x30, 0x30, 0x30};
const char dc_cw_key[4] = {0x30, 0x30, 0x43, 0x57};
const char dc_ccw_key[4] = {0x30, 0x43, 0x43, 0x57};

// Servo Motor
volatile const uint32_t Timer_Period_20ms = 0x249F00U;
volatile uint8_t degree = 0; // 0 degree
const double SERVO_MINIMUM_DUTY = 0.03;
const double SERVO_MAXIMUM_DUTY = 0.12;
const double SERVO_EACH_DEGREE = (SERVO_MAXIMUM_DUTY - SERVO_MINIMUM_DUTY) / 180;
const char servo_0_key[4] = {0x30, 0x30, 0x30, 0x30};
const char servo_90_key[4] = {0x30, 0x30, 0x30, 0x30};
const char servo_180_key[4] = {0x30, 0x30, 0x30, 0x30};
const char servo_p10_key[4] = {0x30, 0x50, 0x31, 0x30};
const char servo_n10_key[4] = {0x30, 0x4D, 0x31, 0x30};

// ADC
volatile uint8_t adc_mode = 0; // 0: off, 1 : potentiometer, 3 : cds
volatile CDS_t brightness;
const float ADC_CONST = (float)(3.3 / 4096);
volatile uint16_t potentiometer_mV = 0, potentiometer_Ra = 0, potentiometer_Rb = 0;
// uint16_t temperature_mV;     // not use
// int16_t temperature;         // not use
volatile uint16_t cds_data;

// DAC
volatile uint8_t sound_play1_buff = 0U;
volatile uint8_t sound_play2_buff = 0U;
volatile uint16_t sound_value;
extern unsigned char rawData1[192480];
extern unsigned char rawData2[190176];
const char dac_sound1_key[4] = {0x30, 0x30, 0x30, 0x30};
const char dac_sound2_key[4] = {0x30, 0x30, 0x30, 0x30};

// UART
volatile char tx_data_buff[6]; // fill data for transmit by uart bus
struct MSG_PACKET msg_rx;
struct MSG_PACKET msg_tx;
extern uint8_t uart_idx;

// Timer
volatile uint32_t agt0_threshold = 1000U; // init is 1000[ms]
volatile uint32_t agt1_threshold = 0U;    // init is 0[ms] - need to init val

// CAN
can_frame_t g_can_tx_frame;

const uint32_t can_id = 0x41;
const uint8_t data_field_can[] = "ML_FINAL";

// Security
extern volatile uint32_t agt1_counter;
volatile uint8_t root = 0U;
volatile uint8_t write_protection = 1U;
extern volatile uint8_t write_protection_unlock_stage;
const char write_protection_key1[4] = {0x30, 0x39, 0x35, 0x39};
const char write_protection_key2[4] = {0x30, 0x45, 0x39, 0x44};
const char priv_admin_key[4] = {0x30, 0x46, 0x30, 0x46};
const char priv_root_key[4] = {0x46, 0x30, 0x46, 0x30};

// test
volatile uint8_t error_count = 0U;
volatile uint8_t correct_count = 0U;
volatile uint8_t test = 0U;

//----------------------------------------Setting-------------------------------------
void Initial_Setting()
{
    state.task_mode_11 = 0;
    state.task_mode_12 = 0;
    state.task_mode_13 = 0;
    state.task_mode_14 = 0;

    // srand((unsigned)time(NULL)); need for rand

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
// void R_ADC1_Setting()
//
//    R_ADC_Open(&g_adc1_ctrl, &g_adc1_cfg);
//    R_ADC_ScanCfg(&g_adc1_ctrl, &g_adc1_channel_cfg);
//
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

void R_CAN_TX_CALL()
{
    R_CAN_Write(&g_can0_ctrl, 0, &g_can_tx_frame);
}
//----------------------------------------Func----------------------------------------
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
void Rotate_DC()
{
    R_GPT3->GTCCR[0] = Timer_Period_2ms * dutyRate / 100;
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
    if (ch2_adc_result >= 400) // 400 is threshold value
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

    if (adc_mode == 1)
    {
        Potentiometer_Read();
    }
    // Thermal_Sensor_Read();
    else if (adc_mode == 3)
    {
        Cds_Sensor_Read();
    }
}
void Sound_Play1()
{
    for (uint32_t i = 0; i < sizeof(rawData1); i += 2)
    {
        sound_value = (uint16_t)(rawData1[i] | (rawData1[i + 1] << 8));
        R_DAC_Write(&g_dac0_ctrl, sound_value);
        R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MICROSECONDS);
    }
}
void Sound_Play2()
{
    for (uint32_t i = 0; i < sizeof(rawData2); i += 2)
    {
        sound_value = (uint16_t)(rawData2[i] | (rawData2[i + 1] << 8));
        R_DAC_Write(&g_dac0_ctrl, sound_value);
        R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MICROSECONDS);
    }
}
void user_uart_write(const char *msg, uint32_t const bytes)
{
    char *string = (char *)calloc(1, sizeof(char) * (bytes));

    strcat(string, msg);

    R_SCI_UART_Write(&g_uart0_ctrl, (unsigned char *)string, bytes);
    free(string);
}
void user_uart_read(uint32_t data, uint8_t idx)
{
    if (uart_idx == 1)
    {
        msg_rx.DA_MF_B = data;
    }
    // Binary Format (GUI)
    if (msg_rx.DA_MF_b.msg_form == 1)
    {
        switch (idx)
        {
        case 2: // message tpye, group number, command
            msg_rx.GUI.MT_GN_C_B = data;
            break;
        case 3: // data length, data field high
            msg_rx.GUI.DL_DFH_B = data;
            break;
        case 4: // data field low
            msg_rx.GUI.DFL_B = data;
            SET_MSG_DF();
            break;
        default:
            break;
        }
    }
    // ASCII Format (CLI)
    else if (msg_rx.DA_MF_b.msg_form == 2)
    {
        switch (idx)
        {
        case 2: // message type
            msg_rx.CLI.msg_type = data;
            break;
        case 3: // group number
            msg_rx.CLI.group_num = data;
            break;
        case 4: // command
            msg_rx.CLI.cmd = data;
            break;
        case 5: // data length
            msg_rx.CLI.data_len = data;
            break;
        case 6: // data field 1
            msg_rx.CLI.data_field[3] = data;
            break;
        case 7: // data field 2
            msg_rx.CLI.data_field[2] = data;
            break;
        case 8: // data field 3
            msg_rx.CLI.data_field[1] = data;
            break;
        case 9: // data field 4
            msg_rx.CLI.data_field[0] = data;
            SET_MSG_DF();
            break;
        default:
            break;
        }
    }
    else // Not supported message format
    {
    }
}
void Control_Group()
{
    // Binary Format (GUI)
    if (msg_rx.DA_MF_b.msg_form == 1)
    {
        switch (msg_rx.GUI.MT_GN_C_b.group_num)
        {
        case 0: // LED
            if (msg_rx.GUI.MT_GN_C_b.cmd == 1)
            {
                // LED08 Toggle
                LED_PA08_Enable_Level ^= 0x01;
                R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[0], LED_PA08_Enable_Level);
            }
            else if (msg_rx.GUI.MT_GN_C_b.cmd == 2)
            {
                // LED09 Toggle
                LED_PA09_Enable_Level ^= 0x01;
                R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[1], LED_PA09_Enable_Level);
            }
            break;
        case 3: // DC Motor
            if (root == 1)
            {
                if (msg_rx.GUI.MT_GN_C_b.cmd == 1)
                {
                    // DC Motor On
                    L293_CH0_Enable_Level = BSP_IO_LEVEL_HIGH;
                    R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Enable, L293_CH0_Enable_Level);
                }
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 2)
                {
                    // DC Motor Off
                    L293_CH0_Enable_Level = BSP_IO_LEVEL_LOW;
                    R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Enable, L293_CH0_Enable_Level);
                }
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 3)
                {
                    dutyRate = msg_rx.DF; // get dutyrate from data field
                    if (dutyRate > 100)
                    {
                        dutyRate = 100; // dutyrate limit
                    }
                    Rotate_DC(); // Update DC Motor
                }
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 4)
                {
                    // DC Motor Direction CW
                    L293_CH0_Direction_Level = BSP_IO_LEVEL_LOW;
                    R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Direction, L293_CH0_Direction_Level);
                }
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 5)
                {
                    // DC Motor Direction CCW
                    L293_CH0_Direction_Level = BSP_IO_LEVEL_HIGH;
                    R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Direction, L293_CH0_Direction_Level);
                }
            }
            break;
        case 4: // Servo Motor
            if (root == 1)
            {
                if (msg_rx.GUI.MT_GN_C_b.cmd == 1)
                {
                    degree = 0;
                    Rotate_Servo(); // Update Servo
                }
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 2)
                {
                    degree = 90;
                    Rotate_Servo(); // Update Servo
                }
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 3)
                {
                    degree = 180;
                    Rotate_Servo(); // Update Servo
                }
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 4)
                {
                    degree += 10;
                    if (degree > 180)
                    {
                        degree = 180; // degree limit
                    }
                    Rotate_Servo();
                }
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 5)
                {
                    degree -= 10;
                    if (degree > 180)
                    {
                        degree = 0; // degree limit
                    }
                    Rotate_Servo();
                }
            }
            break;
        case 6: // DAC
            if (msg_rx.GUI.MT_GN_C_b.cmd == 1)
            {
                sound_play1_buff += 1;
            }
            else if (msg_rx.GUI.MT_GN_C_b.cmd == 2)
            {
                sound_play2_buff += 1;
            }
            break;
        case 7: // Timer Period
            if (root == 1)
            {
                if (msg_rx.GUI.MT_GN_C_b.cmd == 1)
                {
                    agt0_threshold = msg_rx.DF; // get threshold from data field
                    if (agt0_threshold < 100)
                    {
                        agt0_threshold = 100; // threshold limit 100[ms]
                    }
                    else if (agt0_threshold > 1000)
                    {
                        agt0_threshold = 1000; // threshold limit 1000[ms]
                    }
                }
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 2)
                {
                    agt1_threshold = msg_rx.DF; // get threshold from data field
                    if (agt0_threshold < 1000)
                    {
                        agt0_threshold = 1000; // threshold limit 1000[ms]
                    }
                    else if (agt0_threshold > 3000)
                    {
                        agt0_threshold = 3000; // threshold limit 3000[ms]
                    }
                    if (adc_mode == 0)
                    {
                        adc_mode = 1; // ADC Mode On
                        print_data[2] = fnd_num[1];
                    }
                }
            }
            break;
        default: // not supported group number
            break;
        }
    }
    // ASCII Format (CLI)
    else if (msg_rx.DA_MF_b.msg_form == 2)
    {
        switch (msg_rx.CLI.group_num)
        {
        case 0x30: // LED
            if (msg_rx.CLI.cmd == 0x31)
            {
                // LED08 Toggle
                LED_PA08_Enable_Level ^= 0x01;
                R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[0], LED_PA08_Enable_Level);
            }
            else if (msg_rx.CLI.cmd == 0x32)
            {
                // LED09 Toggle
                LED_PA09_Enable_Level ^= 0x01;
                R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[1], LED_PA09_Enable_Level);
            }
            break;
        case 0x31: // FND
            if (msg_rx.CLI.cmd == 0x31)
            { // dec
                print_data[3] = fnd_num[msg_rx.CLI.data_field[0] - 0x30];
            }
            else if (msg_rx.CLI.cmd == 0x32)
            { // hex
                if (msg_rx.CLI.data_field[0] < 0x3A)
                { // decimal range
                    print_data[3] = fnd_num[msg_rx.CLI.data_field[0] - 0x30];
                }
                else
                { // hex range
                    print_data[3] = fnd_num[msg_rx.CLI.data_field[0] - 0x37];
                }
            }
            else if (msg_rx.CLI.cmd == 0x33)
            { // bin

                print_data[3] = fnd_num[(msg_rx.CLI.data_field[3] - 0x30) * 8 + (msg_rx.CLI.data_field[2] - 0x30) * 4 + (msg_rx.CLI.data_field[1] - 0x30) * 2 + (msg_rx.CLI.data_field[0] - 0x30) * 1];
            }
            break;
        case 0x33: // DC Motor
            if (root == 1)
            {
                if (msg_rx.CLI.cmd == 0x31)
                {
                    // DC Motor On
                    if (ASCII_Check(dc_on_key))
                    {
                        L293_CH0_Enable_Level = BSP_IO_LEVEL_HIGH;
                        R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Enable, L293_CH0_Enable_Level);
                    }
                }
                else if (msg_rx.CLI.cmd == 0x32)
                {
                    // DC Motor Off
                    if (ASCII_Check(dc_off_key))
                    {
                        L293_CH0_Enable_Level = BSP_IO_LEVEL_LOW;
                        R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Enable, L293_CH0_Enable_Level);
                    }
                }
                else if (msg_rx.CLI.cmd == 0x33)
                {
                    if (msg_rx.CLI.data_field[3] == 0x30)
                    {
                        dutyRate = msg_rx.DF; // get dutyrate from data field
                        if (dutyRate > 100)
                        {
                            dutyRate = 100; // dutyrate limit
                        }
                        Rotate_DC(); // Update DC Motor
                    }
                }
                else if (msg_rx.CLI.cmd == 0x34)
                {
                    // DC Motor Direction CW
                    if (ASCII_Check(dc_cw_key))
                    {
                        L293_CH0_Direction_Level = BSP_IO_LEVEL_LOW;
                        R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Direction, L293_CH0_Direction_Level);
                    }
                }
                else if (msg_rx.CLI.cmd == 0x35)
                {
                    // DC Motor Direction CCW
                    if (ASCII_Check(dc_ccw_key))
                    {
                        L293_CH0_Direction_Level = BSP_IO_LEVEL_HIGH;
                        R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Direction, L293_CH0_Direction_Level);
                    }
                }
            }
            break;
        case 0x34: // Servo Motor
            if (root == 1)
            {
                if (msg_rx.CLI.cmd == 0x31)
                {
                    if (ASCII_Check(servo_0_key))
                    {
                        degree = 0;
                        Rotate_Servo(); // Update Servo
                    }
                }
                else if (msg_rx.CLI.cmd == 0x32)
                {
                    if (ASCII_Check(servo_90_key))
                    {
                        degree = 90;
                        Rotate_Servo(); // Update Servo
                    }
                }
                else if (msg_rx.CLI.cmd == 0x33)
                {
                    if (ASCII_Check(servo_180_key))
                    {
                        degree = 180;
                        Rotate_Servo(); // Update Servo
                    }
                }
                else if (msg_rx.CLI.cmd == 0x34)
                {
                    if (ASCII_Check(servo_p10_key))
                    {
                        degree += 10;
                        if (degree > 180)
                        {
                            degree = 180; // degree limit
                        }
                        Rotate_Servo();
                    }
                }
                else if (msg_rx.CLI.cmd == 0x35)
                {
                    if (ASCII_Check(servo_n10_key))
                    {
                        degree -= 10;
                        if (degree > 180)
                        {
                            degree = 0; // degree limit
                        }
                        Rotate_Servo();
                    }
                }
            }
            break;
        case 0x36: // DAC
            if (msg_rx.CLI.cmd == 0x31)
            {
                sound_play1_buff += 1;
            }
            else if (msg_rx.CLI.cmd == 0x32)
            {
                sound_play2_buff += 1;
            }
            break;
        case 0x37: // Timer Period
            if (root == 1)
            {
                if (msg_rx.CLI.cmd == 0x31)
                {
                    agt0_threshold = msg_rx.DF; // get threshold from data field
                    if (agt0_threshold < 100)
                    {
                        agt0_threshold = 100; // threshold limit 100[ms]
                    }
                    else if (agt0_threshold > 1000)
                    {
                        agt0_threshold = 1000; // threshold limit 1000[ms]
                    }
                }
                else if (msg_rx.CLI.cmd == 0x32)
                {
                    agt1_threshold = msg_rx.DF; // get threshold from data field
                    if (agt0_threshold < 1000)
                    {
                        agt0_threshold = 1000; // threshold limit 1000[ms]
                    }
                    else if (agt0_threshold > 3000)
                    {
                        agt0_threshold = 3000; // threshold limit 3000[ms]
                    }
                    if (adc_mode == 0)
                    {
                        adc_mode = 1; // ADC Mode On
                        print_data[2] = fnd_num[1];
                    }
                }
            }
            break;
        default: // not supported group number
            break;
        }
    }
    else // Not supported message format
    {
    }
}
void SET_MSG_DF()
{
    uint32_t DF = 0;       // return value of data field
    uint32_t bit_mask = 0; // used to mask binary data field

    if (msg_rx.DA_MF_b.msg_form == 1) // Binary Format (GUI)
    {
        DF = (msg_rx.GUI.DL_DFH_b.data_field_high << 8) | msg_rx.GUI.DFL_B;
        bit_mask = (1 << msg_rx.GUI.DL_DFH_b.data_len) - 1;
        DF &= bit_mask;
    }
    else if (msg_rx.DA_MF_b.msg_form == 2) // ASCII Format (CLI)
    {
        for (int i = 0; i < msg_rx.CLI.data_len - 0x30; i++)
        {
            DF += (msg_rx.CLI.data_field[i] - 0x30) * pow(10, i);
        }
    }
    else // Not supported message format
    {
    }
    msg_rx.DF = DF;
}

// Monitor Group
void Monitor_Tact_Switch()
{
    memset(&msg_tx, 0, sizeof(msg_tx)); // clear msg_tx

    // Device address, Message format
    msg_tx.DA_MF_b.msg_form = 1;  // Binary Format (GUI)
    msg_tx.DA_MF_b.dev_addr = 18; // Device Address

    // message type, group number, command
    msg_tx.GUI.MT_GN_C_b.msg_type = 2;  // Monitor
    msg_tx.GUI.MT_GN_C_b.group_num = 2; // Tact Switch
    msg_tx.GUI.MT_GN_C_b.cmd = 1;       // Get Tact Switch Status

    // data length, data field high
    msg_tx.GUI.DL_DFH_b.data_len = 12;                  // data length
    msg_tx.GUI.DL_DFH_b.data_field_high = (1 << 5) - 1; // data field high all set ('b1111)

    // data field low
    msg_tx.GUI.DFL_B = (1 << 13) - 1; // data field low all set ('b1111_1111_1111)

    // set tx_data_buff
    tx_data_buff[0] = 0x02; // STX
    tx_data_buff[1] = msg_tx.DA_MF_B;
    tx_data_buff[2] = msg_tx.GUI.MT_GN_C_B;
    tx_data_buff[3] = msg_tx.GUI.DL_DFH_B;
    tx_data_buff[4] = msg_tx.GUI.DFL_B;
    tx_data_buff[5] = 0x03; // ETX

    // send data
    user_uart_write(tx_data_buff, 6);
}
void Monitor_Variable_Resistor()
{
    if (root == 1)
    {
        memset(&msg_tx, 0, sizeof(msg_tx)); // clear msg_tx

        // Device address, Message format
        msg_tx.DA_MF_b.msg_form = 1;  // Binary Format (GUI)
        msg_tx.DA_MF_b.dev_addr = 18; // Device Address

        // message type, group number, command
        msg_tx.GUI.MT_GN_C_b.msg_type = 2;  // Monitor
        msg_tx.GUI.MT_GN_C_b.group_num = 5; // ADC
        msg_tx.GUI.MT_GN_C_b.cmd = 1;       // Get Variable Resistor Status

        // data length, data field high
        msg_tx.GUI.DL_DFH_b.data_len = 9;        // data length
        msg_tx.GUI.DL_DFH_b.data_field_high = 0; // data field high ('b0000)

        // data field low
        msg_tx.GUI.DFL_B = 128 + (100 * potentiometer_Rb / 10000);

        // set tx_data_buff
        tx_data_buff[0] = 0x02; // STX
        tx_data_buff[1] = msg_tx.DA_MF_B;
        tx_data_buff[2] = msg_tx.GUI.MT_GN_C_B;
        tx_data_buff[3] = msg_tx.GUI.DL_DFH_B;
        tx_data_buff[4] = msg_tx.GUI.DFL_B;
        tx_data_buff[5] = 0x03; // ETX

        // send data
        user_uart_write(tx_data_buff, 6);
    }
}
void Monitor_CDS()
{
    if (root == 1)
    {
        memset(&msg_tx, 0, sizeof(msg_tx)); // clear msg_tx

        // Device address, Message format
        msg_tx.DA_MF_b.msg_form = 1;  // Binary Format (GUI)
        msg_tx.DA_MF_b.dev_addr = 18; // Device Address

        // message type, group number, command
        msg_tx.GUI.MT_GN_C_b.msg_type = 2;  // Monitor
        msg_tx.GUI.MT_GN_C_b.group_num = 5; // ADC
        msg_tx.GUI.MT_GN_C_b.cmd = 3;       // Get CDS Status

        // data length, data field high
        msg_tx.GUI.DL_DFH_b.data_len = 9;        // data length
        msg_tx.GUI.DL_DFH_b.data_field_high = 1; // data field high all set ('b0001)

        // data field low
        if (brightness == CDS_light)
        {
            msg_tx.GUI.DFL_B = 128 + (127); // data field all set ('b1111_1111)
        }
        else if (brightness == CDS_dark)
        {
            msg_tx.GUI.DFL_B = 128; // data field set MSB ('b1000_0000)
        }

        // set tx_data_buff
        tx_data_buff[0] = 0x02; // STX
        tx_data_buff[1] = msg_tx.DA_MF_B;
        tx_data_buff[2] = msg_tx.GUI.MT_GN_C_B;
        tx_data_buff[3] = msg_tx.GUI.DL_DFH_B;
        tx_data_buff[4] = msg_tx.GUI.DFL_B;
        tx_data_buff[5] = 0x03; // ETX

        // send data
        user_uart_write(tx_data_buff, 6);
    }
}
void Select_Type()
{
    if (msg_rx.DA_MF_b.dev_addr != 18)
    {
        return;
    }
    if (msg_rx.DA_MF_b.msg_form == 1) // Binary Format (GUI)
    {
        if (msg_rx.GUI.MT_GN_C_b.msg_type == 3)
        {
            Security_Group();
        }
        else if ((write_protection == 0) & (msg_rx.GUI.MT_GN_C_b.msg_type == 1))
        {
            Control_Group();
        }
    }
    else if (msg_rx.DA_MF_b.msg_form == 2) // ASCII Format (CLI)
    {
        if (msg_rx.CLI.msg_type == 0x33)
        {
            Security_Group();
        }
        else if ((write_protection == 0) & (msg_rx.CLI.msg_type == 0x31))
        {
            Control_Group();
        }
    }
}
void Security_Group()
{
    if (msg_rx.DA_MF_b.msg_form == 1)
    {
        switch (msg_rx.GUI.MT_GN_C_b.group_num)
        {
        case 0: // Write Protection
            if (msg_rx.GUI.MT_GN_C_b.cmd == 6)
                Key_Check1();
            else if (msg_rx.GUI.MT_GN_C_b.cmd == 7)
                Key_Check2();
            break;
        case 1: // Privilege
            if (write_protection == 0)
            {
                if (msg_rx.GUI.MT_GN_C_b.cmd == 0)
                    Get_Admin();
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 7)
                    Get_Root();
            }
            break;
        default:
            break;
        }
    }
    else if (msg_rx.DA_MF_b.msg_form == 2)
    {
        switch (msg_rx.CLI.group_num)
        {
        case 0x30: // Write Protection
            if (msg_rx.CLI.cmd == 0x36)
                Key_Check1();
            else if (msg_rx.CLI.cmd == 0x37)
                Key_Check2();
            break;
        case 0x31: // Privilege
            if (write_protection == 0)
            {
                if (msg_rx.CLI.cmd == 0x30)
                    Get_Admin();
                else if (msg_rx.CLI.cmd == 0x37)
                    Get_Root();
            }
            break;
        default:
            break;
        }
    }
}

void Key_Check1()
{
    if (msg_rx.DA_MF_b.msg_form == 1)
    {
        if (msg_rx.DF == 959)
        {
            write_protection_unlock_stage = 1;
            agt1_counter = 0;
        }
    }
    else if (msg_rx.DA_MF_b.msg_form == 2)
    {
        if (ASCII_Check(write_protection_key1))
        {
            write_protection_unlock_stage = 1;
            agt1_counter = 0;
        }
    }
}
void Key_Check2()
{
    if (msg_rx.DA_MF_b.msg_form == 1)
    {
        if ((msg_rx.DF == 0xE9D) & (write_protection_unlock_stage == 1))
        {
            write_protection = 0; // Protection Unlock
            LED_PA10_Enable_Level = BSP_IO_LEVEL_LOW;
            R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[2], LED_PA10_Enable_Level);
        }
    }
    else if (msg_rx.DA_MF_b.msg_form == 2)
    {
        if ((ASCII_Check(write_protection_key2)) & (write_protection_unlock_stage == 1))
        {
            write_protection = 0; // Protection Unlock
            LED_PA10_Enable_Level = BSP_IO_LEVEL_LOW;
            R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[2], LED_PA10_Enable_Level);
        }
    }
}

void Get_Admin()
{
    if (msg_rx.DA_MF_b.msg_form == 1)
    {
        if (msg_rx.DF == 0x555) // admin priv
        {
            root = 0; // diable root
            LED_PB00_Enable_Level = BSP_IO_LEVEL_HIGH;
            R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[3], LED_PB00_Enable_Level);
        }
    }
    else if (msg_rx.DA_MF_b.msg_form == 2)
    {
        test++;
        if (ASCII_Check(priv_admin_key)) // admin priv
        {
            root = 0; // diable root
            LED_PB00_Enable_Level = BSP_IO_LEVEL_HIGH;
            R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[3], LED_PB00_Enable_Level);
        }
    }
}
void Get_Root()
{
    if (msg_rx.DA_MF_b.msg_form == 1)
    {
        if (msg_rx.DF == 0xAAA) // root priv
        {
            root = 1; // enable root
            LED_PB00_Enable_Level = BSP_IO_LEVEL_LOW;
            R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[3], LED_PB00_Enable_Level);
        }
    }
    else if (msg_rx.DA_MF_b.msg_form == 2)
    {
        test++;
        if (ASCII_Check(priv_root_key)) // root priv
        {
            root = 1; // enable root
            LED_PB00_Enable_Level = BSP_IO_LEVEL_LOW;
            R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[3], LED_PB00_Enable_Level);
        }
    }
}

int ASCII_Check(char *check_buff)
{
    for (int i = 0; i < msg_rx.CLI.data_len - 0x30; i++)
    { // assign data field to array occurs with invert idx
        // ex. Rx data field = {1, 2, 3, 4}
        // check buff = {4, 3, 2, 1}
        if (msg_rx.CLI.data_field[i] != check_buff[3 - i])
        {
            error_count++;
            return 0;
        }
    }
    correct_count++;
    return 1;
}