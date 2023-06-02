#include "Library.h"

// Uart
volatile char tx_data_buff[6]; // fill data for transmit by uart bus
struct MSG_PACKET msg_rx;
struct MSG_PACKET msg_tx;

// Secure
extern volatile uint8_t write_protection;

// Fill Structure msg_rx with received uart data
void Message_Decode(uint32_t data, uint8_t idx)
{
    if (idx == 1)
    {
        msg_rx.DA_MF_B = data;
    }
    else
    {
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
                Get_Message_Data_Field_Dec();
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
                Get_Message_Data_Field_Dec();
                break;
            default:
                break;
            }
        }
        else // Not supported message format
        {
        }
    }
}

// Fill DF in structure msg_rx with decimal value of data field
void Get_Message_Data_Field_Dec()
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
    msg_rx.DF = DF; // used in several functions(ex. DC motor, Timer Period Setting)
}

// Select Message Group by msg_rx message type
void Select_Type()
{
    if (msg_rx.DA_MF_b.dev_addr != 18) // check ID
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

// Check ASCII type message data field base on data length of message
// ex. Rx data field = {1, 2, 3, 4}
// check buff = {4, 3, 2, 1}
int ASCII_Check(char *check_buff)
{
    for (int i = 0; i < msg_rx.CLI.data_len - 0x30; i++)
    { // assign data field to array occurs with invert idx

        if (msg_rx.CLI.data_field[i] != check_buff[3 - i])
        {
            return 0;
        }
    }
    return 1;
}

// UART write data encode for transfer
void Message_Encode()
{
    // set tx_data_buff
    tx_data_buff[0] = STX; // STX
    tx_data_buff[1] = msg_tx.DA_MF_B;
    tx_data_buff[2] = msg_tx.GUI.MT_GN_C_B;
    tx_data_buff[3] = msg_tx.GUI.DL_DFH_B;
    tx_data_buff[4] = msg_tx.GUI.DFL_B;
    tx_data_buff[5] = ETX; // ETX
}

// Uart write function for transfer
void user_uart_write(const char *msg, uint32_t const bytes)
{
    char *string = (char *)calloc(1, sizeof(char) * (bytes));

    strcat(string, msg);

    R_SCI_UART_Write(&g_uart0_ctrl, (unsigned char *)string, bytes);
    free(string);
}
