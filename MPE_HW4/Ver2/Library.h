#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "hal_data.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "time.h"

// Init Setting
void Initial_Setting();     // Initialize all periphearl setting
void IRQ_Initial_Setting(); // Initialize IRQ Setting
void IRQ_Enable();          // Enable IRQ
void IRQ_Disable();         // Disable IRQ
void LED_Reset();           // Reset LED
void FND_Reset();           // Reset FND
void AGT1_Setting();        // Timer1 for ADC
void R_ADC0_Setting();      // ADC Setting
void R_GPIO_Setting();      // GPIO Setting for GPT Output
void R_GPT_Setting();       // GPT Setting for PWM Output
void R_GPT3_Setting();      // GPT3 Setting for DC Motor
void R_GPT0_Setting();      // GPT0 Setting for Servo Motor
void R_DAC0_Setting();      // DAC0 Setting
void AGT0_Setting();        // Timer0 for Digit Counter
void R_SCI_UART_Setting();  // UART Setting
void R_CAN_Setting();       // CAN Setting

// FND
void FND_Display_Data(uint8_t digit, uint8_t data); // FND Display Data
void FND_Print_Data(uint8_t *string);               // FND Print Data
void Count_Init();                                  // Initialize Counting Value
void FND_Count_Update();                            // Update FND digit 1,2

// CAN
void CAN_Frame_Setting(uint32_t id, uint8_t dlc, uint8_t *data); // CAN Frame Setting
void R_CAN_TX_CALL();                                            // CAN Transmit Call

// Uart - uart.c
void Message_Decode(uint32_t data, uint8_t idx);             // UART Message Decode
void Get_Message_Data_Field_Dec();                           // Get Message DF Decimal value
void Select_Type();                                          // Check UART Message Type
int ASCII_Check(char *check_buff);                           // Check Data Field ASCII value
void Message_Encode();                                       // UART Message Encode
void user_uart_write(const char *msg, uint32_t const bytes); // UART Transmit

// Control - control.c
void Control_Group(); // Control Group
void Rotate_DC();     // Update DC Motor Speed by dutyRate
void Rotate_Servo();  // Update Servo Motor Angle by degree
void Sound_Play1();   // Play Sound 1
void Sound_Play2();   // Play Sound 2

// Montior - monitor.c
void Monitor_Tact_Switch();       // Monitor Tact Switch
void Monitor_Variable_Resistor(); // Monitor Variable Resistor
void Monitor_CDS();               // Monitor CDS Sensor
void Potentiometer_Read();        // Read Potentiometer Value
void Cds_Sensor_Read();           // Read Cds Sensor Value
void ADC_Read_and_Convert(void);  // ADC Read and Convert to Digital Value
void ADC_Mode_Change();           // ADC Mode Change

// Security - security.c
void Security_Group(); // Security Group
void Key_Check1();     // Check Key 1 of Write Protection Unlock
void Key_Check2();     // Check Unlock Time(3[s]) and Key 2 of Write Protection
void Get_Admin();      // Get Admin Permission
void Get_Root();       // Get Root Permission

// Define
#define SEGMENT_INDEX 8
#define DIGIT_INDEX 4
#define LED_INDEX 4

#define IO_PCNTR1_MASK 0x00010003
#define ANGLE_UNIT 30
#define MAX_SERVO_DEGREE 180
#define AGT_SERVO_PERIOD (MAX_SERVO_DEGREE / ANGLE_UNIT) // degree base value

#define STX 0x02 // Start of Text
#define ETX 0x03 // End of Text

#define STANDARD_CAN_FRAME 8 // CAN message size

// Some Global Variable
typedef enum
{
    CDS_light = 0,
    CDS_dark = 1,
} CDS_t;

static const bsp_io_port_pin_t led_pin[4] = {
    BSP_IO_PORT_10_PIN_08, // LED0
    BSP_IO_PORT_10_PIN_09, // LED1
    BSP_IO_PORT_10_PIN_10, // LED2
    BSP_IO_PORT_11_PIN_00} // LED3
;

static const bsp_io_port_pin_t segment_digit[4] = {
    BSP_IO_PORT_03_PIN_05, // digit1
    BSP_IO_PORT_03_PIN_06, // digit2
    BSP_IO_PORT_03_PIN_07, // digit3
    BSP_IO_PORT_03_PIN_08  // digit4
};

static const bsp_io_port_pin_t segment_led_pin[8] = {
    BSP_IO_PORT_06_PIN_14, // FND LED0
    BSP_IO_PORT_06_PIN_04, // FND LED1
    BSP_IO_PORT_06_PIN_05, // FND LED2
    BSP_IO_PORT_06_PIN_06, // FND LED3
    BSP_IO_PORT_06_PIN_07, // FND LED4
    BSP_IO_PORT_06_PIN_11, // FND LED5
    BSP_IO_PORT_06_PIN_12, // FND LED6
    BSP_IO_PORT_06_PIN_13  // FND LED7
};

static const uint8_t fnd_num[16] = {
    0x81, // 0
    0xCF, // 1
    0x92, // 2
    0x86, // 3
    0xCC, // 4
    0xA4, // 5
    0xA0, // 6
    0x8F, // 7
    0x80, // 8
    0x84, // 9
    0x88, // A
    0xE0, // B
    0xB1, // C
    0xC2, // D
    0xB0, // E
    0xB8  // F
};

typedef const enum _irq_number {
    EXTERNAL_INTERRUPT_11 = 11,
    EXTERNAL_INTERRUPT_12,
    EXTERNAL_INTERRUPT_13,
    EXTERNAL_INTERRUPT_14
} irq_number;

// Message Packet Structure for ML FINAL PROJECT (except 0. STX, 5. or 10. ETX)

// 1. Data Address, Message Format - Common for Binary, ASCII

// Binary Format (GUI)
// 2. Message Type, Group Number, Command
// 3. Data Length, Data Field High
// 4. Data Field Low

// ASCII Format (CLI)
// 2. Message Type
// 3. Group Numbe
// 4. Command
// 5. Data Length
// 6 ~ 9. Data Field

// DF. Data Field Decimal Value for serveral function

// Example, If you want access
// binary format message command ->  msg_rx.GUI.MT_GN_C_b.cmd
// ascii format message type -> msg_rx.CLI.cmd

struct MSG_PACKET
{
    union
    {
        volatile uint8_t DA_MF_B; // access with byte
        struct                    // access with bitfield
        {
            volatile uint8_t msg_form : 2; // 'b01 = GUI(Binary), 'b10 = CLI(ASCII)
            volatile uint8_t dev_addr : 6; // 'd18 = 'b01_0010
        } DA_MF_b;
    }; // data address, message format

    struct // GUI - Binary Format
    {
        union
        {
            volatile uint8_t MT_GN_C_B; // access with byte
            struct                      // access with bitfield
            {
                volatile uint8_t cmd : 3;
                volatile uint8_t group_num : 3;
                volatile uint8_t msg_type : 2;
            } MT_GN_C_b;
        }; // message type, group number, command

        union
        {
            volatile uint8_t DL_DFH_B; // access with byte
            struct                     // access with bitfield
            {
                volatile uint8_t data_field_high : 4;
                volatile uint8_t data_len : 4;
            } DL_DFH_b;
        }; // data length, data field high

        volatile uint8_t DFL_B; // data field low
    } GUI;

    struct // CLI - ASCII Format
    {
        volatile uint8_t msg_type;      // message type
        volatile uint8_t group_num;     // group number
        volatile uint8_t cmd;           // command
        volatile uint8_t data_len;      // data length
        volatile uint8_t data_field[4]; // data_field array
    } CLI;

    volatile uint32_t DF; // data field decimal value
};

#endif /* LIBRARY_H_ */