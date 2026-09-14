/*
 * main.c
 *
 *  Created on: Sep 13, 2026
 *      Author: Omar Desoky
 */
#include "../LIB/STD_TYPES.h"
#include "../MCAL/RCC/RCC_int.h"
#include "../MCAL/GPIO/GPIO_int.h"
#include "../MCAL/ADC/ADC_int.h"
#include "../MCAL/USART/USART_int.h"
#include "../MCAL/SYSTICK/SYSTICK_int.h"
#include "../HAL/Heart_Rate/HEART_RATE_int.h"

#define SAMPLE_PERIOD_MS  10U
#define USART1_CLOCK_BIT  4U
#define SCB_CPACR (*(volatile u32*)0xE000ED88U)

static volatile u8 G_u8SampleTickFlag = 0U;

static void UART_PinsInit(void)
{
    GPIOx_PinConfig_t L_xTxCfg = {
        .Port = GPIO_PORTA,
        .Pin = GPIO_PIN9,
        .Mode = GPIO_ALF,
        .OutputType = OUTPUT_push_pull,
        .OutputSpeed = Output_high_speed,
        .PullType = GPIO_OT_NOPULL,
        .AltFunc = GPIO_AF7
    };
    GPIOx_PinConfig_t L_xRxCfg = {
        .Port = GPIO_PORTA,
        .Pin = GPIO_PIN10,
        .Mode = GPIO_ALF,
        .OutputType = OUTPUT_push_pull,
        .OutputSpeed = Output_high_speed,
        .PullType = GPIO_OT_PULLUP,
        .AltFunc = GPIO_AF7
    };

    MRCC_vEnableCLK(RCC_AHB1, RCC_GPIOA);
    MGPIO_vInit(&L_xTxCfg);
    MGPIO_vInit(&L_xRxCfg);
}

static void u32ToStr(u32 A_u32Value, char* A_pcBuffer)
{
    char L_cTemp[11];
    u8 L_u8Index = 0U;
    u8 L_u8Count = 0U;

    if (A_u32Value == 0U)
    {
        A_pcBuffer[0] = '0';
        A_pcBuffer[1] = '\0';
        return;
    }

    while (A_u32Value > 0U)
    {
        L_cTemp[L_u8Index++] = (char)('0' + (A_u32Value % 10U));
        A_u32Value /= 10U;
    }

    while (L_u8Index > 0U)
    {
        A_pcBuffer[L_u8Count++] = L_cTemp[--L_u8Index];
    }
    A_pcBuffer[L_u8Count] = '\0';
}

static void SysTick_10ms_Callback(void)
{
    G_u8SampleTickFlag = 1U;
}

int main(void)
{
    SCB_CPACR |= (0xFU << 20U);

    MRCC_vInit();
    UART_PinsInit();
    MRCC_vEnableCLK(RCC_APB2, USART1_CLOCK_BIT);
    MUSART_vInit();
    MUSART_vSendString("SYSTEM READY\r\n");

    MADC_vInit();

    MSYSTIC_Config_t L_xSysTickCfg = {
        .InterruptEnable = INT_ENABLE,
        .CLK_SRC = CLK_SRC_AHB_8
    };
    MSYSTICK_vInit(&L_xSysTickCfg);
    MSYSTICK_vSetIntervalMulti(SAMPLE_PERIOD_MS, SysTick_10ms_Callback);
    HHeartRate_vInit();

    u16 L_u16AdcValue;
    u16 L_u16Bpm;
    char L_acStrBuffer[12];

    while (1)
    {
        if (G_u8SampleTickFlag)
        {
            G_u8SampleTickFlag = 0U;

            if (MADC_u8Read(&L_u16AdcValue) == ADC_STATUS_OK)
            {
                HHeartRate_vProcessSample(L_u16AdcValue);

                if (HHeartRate_u8GetBpm(&L_u16Bpm))
                {
                    u32ToStr((u32)L_u16Bpm, L_acStrBuffer);
                    MUSART_vSendString("BPM: ");
                    MUSART_vSendString(L_acStrBuffer);
                    MUSART_vSendString("\r\n");
                }
            }
        }
    }
}
