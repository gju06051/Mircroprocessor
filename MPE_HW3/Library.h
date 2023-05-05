#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "hal_data.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "time.h"

void Initial_Setting();
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

void LED_Reset();
void FND_Reset();
void FND_Display_Data(uint8_t digit, uint8_t data);
void FND_Print_Data(uint8_t *string);
void Rotate_Servo();
void Potentiometer_Read();
void Thermal_Sensor_Read();
void Cds_Sensor_Read();
void ADC_Read_and_Convert(void);
void Sound_Play();
void user_uart_write(const char *msg, uint32_t const bytes);

#define SEGMENT_INDEX 8
#define DIGIT_INDEX 4
#define LED_INDEX 4

#define DUTY_RATE_UNIT 5

#define ANGLE_UNIT 30
#define MAX_SERVO_DEGREE 180
#define AGT_SERVO_PERIOD (MAX_SERVO_DEGREE / ANGLE_UNIT)

#define STX 0x02
#define ETX 0x03
#define CARRIAGE_RETURN 0x0D
#define LINE_FEED 0x0A

#define MESSAGE_MAX_SIZE 50

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

typedef enum _servo_state
{
    SERVO_ROTATE_RIGHT = 0,
    SERVO_ROTATE_LEFT
} servo_state;

#endif /* LIBRARY_H_ */