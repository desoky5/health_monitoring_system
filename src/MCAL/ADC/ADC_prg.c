/*
 * ADC_prg.c
 *
 *  Created on: Sep 7, 2026
 *      Author: Omar Desoky
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MTH.h"
#include "../RCC/RCC_int.h"
#include "../GPIO/GPIO_int.h"
#include "ADC_int.h"
#include "ADC_prv.h"

static ADC_Status_t MADC_xConfigureSensorPin(void)
{
	GPIOx_PinConfig_t L_xPinCfg;
	u8 L_u8Channel = (u8)ADC_SENSOR_CHANNEL;

	if (L_u8Channel <= ADC_CHANNEL_7)
	{
		L_xPinCfg.Port = GPIO_PORTA;
		L_xPinCfg.Pin = L_u8Channel;
	}
	else if (L_u8Channel <= ADC_CHANNEL_9)
	{
		L_xPinCfg.Port = GPIO_PORTB;
		L_xPinCfg.Pin = L_u8Channel - ADC_CHANNEL_8;
	}
	else if (L_u8Channel <= ADC_CHANNEL_15)
	{
		L_xPinCfg.Port = GPIO_PORTC;
		L_xPinCfg.Pin = L_u8Channel - ADC_CHANNEL_10;
	}
	else
	{
		return ADC_STATUS_INVALID_CHANNEL;
	}

	L_xPinCfg.Mode = GPIO_Analog;
	L_xPinCfg.OutputType = OUTPUT_push_pull;
	L_xPinCfg.OutputSpeed = Output_low_speed;
	L_xPinCfg.PullType = GPIO_OT_NOPULL;
	L_xPinCfg.AltFunc = GPIO_AF0;

	MRCC_vEnableCLK(RCC_AHB1, L_xPinCfg.Port);
	MGPIO_vInit(&L_xPinCfg);

	return ADC_STATUS_OK;
}

static void MADC_vSetSampleTime(void)
{
	u8 L_u8Channel = (u8)ADC_SENSOR_CHANNEL;

	if (L_u8Channel <= ADC_CHANNEL_9)
	{
		ADC1->SMPR2 &= ~(0b111U << (3U * L_u8Channel));
		ADC1->SMPR2 |= ADC_CHANNEL_SAMPLE_84 << (3U * L_u8Channel);
	}
	else
	{
		L_u8Channel -= 10U;
		ADC1->SMPR1 &= ~(0b111U << (3U * L_u8Channel));
		ADC1->SMPR1 |= ADC_CHANNEL_SAMPLE_84 << (3U * L_u8Channel);
	}
}

void MADC_vInit(void)
{
	if (MADC_xConfigureSensorPin() != ADC_STATUS_OK)
	{
		return;
	}

	MRCC_vEnableCLK(RCC_APB2, ADC1_CLOCK_BIT);
	ADC_COMMON_CCR &= ~(0b11U << 16U);
	ADC1->CR1 &= ~(0b11U << ADC_CR1_RES_BIT);
	ADC1->CR2 &= ~((1U << ADC_CR2_CONT) | (0b11U << 28U) | (1U << ADC_CR2_ALIGN));
	ADC1->SQR1 &= ~(0b1111U << 20U);
	ADC1->SQR3 = ADC_SENSOR_CHANNEL;
	MADC_vSetSampleTime();
	ADC1->CR2 |= 1U << ADC_CR2_ADON;
}

ADC_Status_t MADC_u8Read(u16* A_pu16Value)
{
	u32 L_u32Timeout = ADC_CONVERSION_TIMEOUT;

	if (A_pu16Value == NULL)
	{
		return ADC_STATUS_INVALID_ARGUMENT;
	}

	ADC1->SR &= ~(1U << ADC_SR_EOC);
	ADC1->CR2 |= 1U << ADC_CR2_SWSTART;

	while ((GET_BIT(ADC1->SR, ADC_SR_EOC) == 0U) && (L_u32Timeout > 0U))
	{
		L_u32Timeout--;
	}

	if (L_u32Timeout == 0U)
	{
		return ADC_STATUS_TIMEOUT;
	}

	*A_pu16Value = (u16)ADC1->DR;
	return ADC_STATUS_OK;
}




