#include "Library.h"

// LED
volatile uint8_t LED_PA08_Enable_Level = BSP_IO_LEVEL_HIGH;
volatile uint8_t LED_PA09_Enable_Level = BSP_IO_LEVEL_HIGH;

// FND
extern uint8_t print_data[4];

// DC Motor
extern volatile const uint32_t Timer_Period_2ms;
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
extern volatile const uint32_t Timer_Period_20ms;
extern const double SERVO_MINIMUM_DUTY;
extern const double SERVO_EACH_DEGREE;
volatile uint8_t degree = 0; // 0 degree
const char servo_0_key[4] = {0x30, 0x30, 0x30, 0x30};
const char servo_90_key[4] = {0x30, 0x30, 0x30, 0x30};
const char servo_180_key[4] = {0x30, 0x30, 0x30, 0x30};
const char servo_p10_key[4] = {0x30, 0x50, 0x31, 0x30};
const char servo_n10_key[4] = {0x30, 0x4D, 0x31, 0x30};

// DAC
volatile uint8_t sound_play1_buff = 0U;
volatile uint8_t sound_play2_buff = 0U;
volatile uint16_t sound_value;
extern unsigned char rawData1[192480];
extern unsigned char rawData2[190176];
const char dac_sound1_key[4] = {0x30, 0x30, 0x30, 0x30};
const char dac_sound2_key[4] = {0x30, 0x30, 0x30, 0x30};

// Timer
volatile uint32_t agt0_threshold = 1000U; // init is 1000[ms]
volatile uint32_t agt1_threshold = 0U;    // init is 0[ms] - need to init val
extern uint8_t adc_mode;

// UART
extern struct MSG_PACKET msg_rx;

// Secure
extern uint8_t root;

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
                LED_PA08_Enable_Level ^= 0x01; // LED08 Toggle
                R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[0], LED_PA08_Enable_Level);
            }
            else if (msg_rx.GUI.MT_GN_C_b.cmd == 2)
            {
                LED_PA09_Enable_Level ^= 0x01; // LED09 Toggle
                R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[1], LED_PA09_Enable_Level);
            }
            break;
        case 3: // DC Motor
            if (root == 1)
            {
                if (msg_rx.GUI.MT_GN_C_b.cmd == 1)
                {

                    L293_CH0_Enable_Level = BSP_IO_LEVEL_HIGH; // DC Motor On
                    R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Enable, L293_CH0_Enable_Level);
                }
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 2)
                {
                    L293_CH0_Enable_Level = BSP_IO_LEVEL_LOW; // DC Motor Off
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
                    L293_CH0_Direction_Level = BSP_IO_LEVEL_LOW; // Direction CW
                    R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Direction, L293_CH0_Direction_Level);
                }
                else if (msg_rx.GUI.MT_GN_C_b.cmd == 5)
                {
                    L293_CH0_Direction_Level = BSP_IO_LEVEL_HIGH; //  Direction CCW
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
                sound_play1_buff += 1; // consumed in hal_entry
            }
            else if (msg_rx.GUI.MT_GN_C_b.cmd == 2)
            {
                sound_play2_buff += 1; // consumed in hal_entry
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
                LED_PA08_Enable_Level ^= 0x01; // LED08 Toggle
                R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[0], LED_PA08_Enable_Level);
            }
            else if (msg_rx.CLI.cmd == 0x32)
            {
                LED_PA09_Enable_Level ^= 0x01; // LED09 Toggle
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
                    if (ASCII_Check(dc_on_key))
                    {
                        L293_CH0_Enable_Level = BSP_IO_LEVEL_HIGH; // DC Motor On
                        R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Enable, L293_CH0_Enable_Level);
                    }
                }
                else if (msg_rx.CLI.cmd == 0x32)
                {
                    if (ASCII_Check(dc_off_key))
                    {
                        L293_CH0_Enable_Level = BSP_IO_LEVEL_LOW; // DC Motor Off
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
                    if (ASCII_Check(dc_cw_key))
                    {
                        L293_CH0_Direction_Level = BSP_IO_LEVEL_LOW; // Direction CW
                        R_IOPORT_PinWrite(&g_ioport_ctrl, L293_CH0_Direction, L293_CH0_Direction_Level);
                    }
                }
                else if (msg_rx.CLI.cmd == 0x35)
                {
                    if (ASCII_Check(dc_ccw_key))
                    {
                        L293_CH0_Direction_Level = BSP_IO_LEVEL_HIGH; // Direction CCW
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
                sound_play1_buff += 1; // consumed in hal_entry()
            }
            else if (msg_rx.CLI.cmd == 0x32)
            {
                sound_play2_buff += 1; // consumed in hal_entry()
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

void Rotate_DC()
{
    R_GPT3->GTCCR[0] = Timer_Period_2ms * dutyRate / 100;
}
void Rotate_Servo()
{
    double temp_calc = (SERVO_MINIMUM_DUTY + SERVO_EACH_DEGREE * (float)degree);
    R_GPT0->GTCCR[0] = (uint32_t)(Timer_Period_20ms * temp_calc);
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