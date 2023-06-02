#include "emWin_thread.h"
#include "GUI.h"
#include "C:\Users\edenlee\e2_studio\workspace\LCD_example1\example\Source\CustomCode\Application.h"
#include "C:\Users\wsws0\e2_studio\workspace\LCD_example1\example\Source\Generated\Resource.h"
#include "C:\Users\wsws0\e2_studio\workspace\LCD_example1\example\Source\Generated\ID_SCREEN_00.h"

void APPW_X_FS_Init(void);

void APPW_X_FS_Init(void){

}

/* emWin_thread entry function */
/* pvParameters contains TaskHandle_t */

int posx = 0;
int cnt = 0;

TEXT_Handle create;
char print[11] = "Hello World";
char buf;
int positionset(int value);

void emWin_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    R_ICU_ExternalIrqOpen(&g_external_irq11_ctrl, &g_external_irq11_cfg);
    R_ICU_ExternalIrqEnable(&g_external_irq11_ctrl);

    /* TODO: add your own code here */
    MainTask();

    while (1)
    {
        vTaskDelay (1);
    }
}

int positionset(int value)
{
    int result = 0;
    if(value != 5)
    {
        result = 25;
    }
    else
    {
        result = 30;
    }
    return result;
}

void Callback_IRQ11(external_irq_callback_args_t *p_args){
    FSP_PARAMETER_NOT_USED(p_args);
    posx += positionset(cnt);
    buf = print[cnt];
    create = TEXT_Create(60 + posx, 0, 480, 272, 0x801, WM_CF_SHOW, &buf, TEXT_CF_LEFT | TEXT_CF_VCENTER);
    TEXT_SetFont(create, GUI_FONT_32B_1);
    TEXT_SetTextColor(create, GUI_WHITE);
    cnt += 1;
}





