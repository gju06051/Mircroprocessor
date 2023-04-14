#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

uint32_t agt_counter = 0U;

void hal_entry(void)
{
    /* TODO: add your own code here */
    R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    R_AGT_Start(&g_timer0_ctrl);
    R_AGT_Stop(&g_timer0_ctrl);
    R_AGT_Close(&g_timer0_ctrl);

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

void R_AGT0_Interrupt(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    // R_AGT0->AGTCR_b.TUNDF = (uint8_t)0U;
    if (agt_counter >= 6000)
        agt_counter = 0;
    agt_counter++;
}

void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable()
{
}
#endif
