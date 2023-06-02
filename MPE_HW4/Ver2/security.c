#include "Library.h"

// LED
volatile uint8_t LED_PA10_Enable_Level = BSP_IO_LEVEL_HIGH;
volatile uint8_t LED_PB00_Enable_Level = BSP_IO_LEVEL_HIGH;

// Write Protection
extern volatile uint32_t agt1_counter;
volatile uint8_t write_protection = 1U;
extern volatile uint8_t write_protection_unlock_stage;
const char write_protection_key1[4] = {0x30, 0x39, 0x35, 0x39};
const char write_protection_key2[4] = {0x30, 0x45, 0x39, 0x44};

// Privilege
volatile uint8_t root = 0U;
const char priv_admin_key[4] = {0x30, 0x46, 0x30, 0x46};
const char priv_root_key[4] = {0x46, 0x30, 0x46, 0x30};

// UART
extern struct MSG_PACKET msg_rx;

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
        if (ASCII_Check(priv_root_key)) // root priv
        {
            root = 1; // enable root
            LED_PB00_Enable_Level = BSP_IO_LEVEL_LOW;
            R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin[3], LED_PB00_Enable_Level);
        }
    }
}