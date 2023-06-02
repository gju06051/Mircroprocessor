#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "hal_data.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "time.h"

// Setting Function
void Initial_Setting();
void R_GPIO_Setting();
void IRQ_Initial_Setting();
void IRQ_Enable();
void IRQ_Disable();
void AGT0_Setting();
void AGT1_Setting();
void R_GPT_Setting();
void R_GPT3_Setting();
void R_GPT0_Setting();
void R_ADC0_Setting();
void R_ADC1_Setting();
void R_DAC0_Setting();
void R_SCI_UART_Setting();
void R_CAN_Setting();

// Operation Function
void LED_Reset();

void FND_Reset();
void FND_Display_Data(uint8_t digit, uint8_t data);
void FND_Print_Data(uint8_t *string);

void Rotate_DC();
void Rotate_Servo();

void Potentiometer_Read();
// void Thermal_Sensor_Read();      not use
void Cds_Sensor_Read();
void ADC_Read_and_Convert(void);

void Sound_Play1();
void Sound_Play2();

void user_uart_write(const char *msg, uint32_t const bytes);
void user_uart_read(uint32_t data, uint8_t idx);
void SET_MSG_DF();

void Select_Type();
void Control_Group();
void Monitor_Tact_Switch();
void Monitor_Variable_Resistor();
void Monitor_CDS();
void Security_Group();
void Key_Check1();
void Key_Check2();
void Get_Admin();
void Get_Root();
int ASCII_Check(char *check_buff);

void CAN_Frame_Setting(uint32_t id, uint8_t dlc, uint8_t *data);
void R_CAN_TX_CALL();

#define SEGMENT_INDEX 8
#define DIGIT_INDEX 4
#define LED_INDEX 4

// #define DUTY_RATE_UNIT 5         not use

#define ANGLE_UNIT 30
#define MAX_SERVO_DEGREE 180
#define AGT_SERVO_PERIOD (MAX_SERVO_DEGREE / ANGLE_UNIT)
#define IO_PCNTR1_MASK 0x00010003
#define STX 0x02
#define ETX 0x03

// #define CARRIAGE_RETURN 0x0D     not use
// #define LINE_FEED 0x0A           not use

#define STANDARD_CAN_FRAME 8

typedef enum
{
    CDS_light = 0,
    CDS_dark = 1,
} CDS_t;

static const bsp_io_port_pin_t led_pin[4] = {
    BSP_IO_PORT_10_PIN_08,
    BSP_IO_PORT_10_PIN_09,
    BSP_IO_PORT_10_PIN_10,
    BSP_IO_PORT_11_PIN_00};

static const bsp_io_port_pin_t segment_digit[4] = {
    BSP_IO_PORT_03_PIN_05,
    BSP_IO_PORT_03_PIN_06,
    BSP_IO_PORT_03_PIN_07,
    BSP_IO_PORT_03_PIN_08};
static const bsp_io_port_pin_t segment_led_pin[8] = {
    BSP_IO_PORT_06_PIN_14,
    BSP_IO_PORT_06_PIN_04,
    BSP_IO_PORT_06_PIN_05,
    BSP_IO_PORT_06_PIN_06,
    BSP_IO_PORT_06_PIN_07,
    BSP_IO_PORT_06_PIN_11,
    BSP_IO_PORT_06_PIN_12,
    BSP_IO_PORT_06_PIN_13};

typedef struct _operation_state
{
    volatile uint8_t task_mode_11;
    volatile uint8_t task_mode_12;
    volatile uint8_t task_mode_13;
    volatile uint8_t task_mode_14;
} operation_state;

typedef const enum _irq_number {
    EXTERNAL_INTERRUPT_11 = 11,
    EXTERNAL_INTERRUPT_12,
    EXTERNAL_INTERRUPT_13,
    EXTERNAL_INTERRUPT_14
} irq_number;

struct MSG_PACKET
{
    union
    {
        volatile uint8_t DA_MF_B; // access with byte

        struct // access with bitfield
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

            struct // access with bitfield
            {
                volatile uint8_t cmd : 3;
                volatile uint8_t group_num : 3;
                volatile uint8_t msg_type : 2;
            } MT_GN_C_b;
        }; // message type, group number, command

        union
        {
            volatile uint8_t DL_DFH_B; // access with byte

            struct // access with bitfield
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

    volatile uint32_t DF; // data field value
};

// typedef enum _servo_state
// {
//     SERVO_ROTATE_RIGHT = 0,
//     SERVO_ROTATE_LEFT
// } servo_state;

#endif /* LIBRARY_H_ */