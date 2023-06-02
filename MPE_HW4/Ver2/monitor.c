#include "Library.h"

// FND
extern uint8_t print_data[4];

// ADC
volatile uint8_t adc_mode = 0; // 0: off, 1 : potentiometer, 3 : cds
volatile CDS_t brightness;
const float ADC_CONST = (float)(3.3 / 4096);
volatile uint16_t potentiometer_mV = 0, potentiometer_Ra = 0, potentiometer_Rb = 0;
volatile uint16_t cds_data;

// UART
extern volatile uint8_t tx_data_buff[6];
extern struct MSG_PACKET msg_tx;

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

    // send data
    Message_Encode();
    user_uart_write(tx_data_buff, 6);
}

void Monitor_Variable_Resistor()
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

    // send data
    Message_Encode();
    user_uart_write(tx_data_buff, 6);
}

void Monitor_CDS()
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

    // send data
    Message_Encode();
    user_uart_write(tx_data_buff, 6);
}

void Potentiometer_Read()
{
    uint16_t ch0_adc_result;

    R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_0, &ch0_adc_result);
    potentiometer_mV = (uint16_t)(ch0_adc_result * ADC_CONST * 1000);
    potentiometer_Rb = (uint16_t)(potentiometer_mV * 3.0303);
    potentiometer_Ra = (uint16_t)(10000 - potentiometer_Rb);
}

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

void ADC_Mode_Change()
{
    if (adc_mode == 1) // ADC1 -> ADC3
    {
        adc_mode = 3;
    }
    else if (adc_mode == 3) // ADC3 -> ADC1
    {
        adc_mode = 1;
    }
    print_data[2] = fnd_num[adc_mode]; // change FND digit
}
