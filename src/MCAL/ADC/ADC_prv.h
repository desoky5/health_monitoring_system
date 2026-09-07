/*
 * ADC_prv.h
 *
 *  Created on: Sep 7, 2026
 *      Author: Omar Desoky
 */

#ifndef MCAL_ADC_ADC_PRV_H_
#define MCAL_ADC_ADC_PRV_H_

#define ADC1_BASE_ADDR 0x40012000U
#define ADC_COMMON_BASE_ADDR 0x40012300U

typedef struct
{
	u32 SR;
	u32 CR1;
	u32 CR2;
	u32 SMPR1;
	u32 SMPR2;
	u32 JOFR1;
	u32 JOFR2;
	u32 JOFR3;
	u32 JOFR4;
	u32 HTR;
	u32 LTR;
	u32 SQR1;
	u32 SQR2;
	u32 SQR3;
	u32 JSQR;
	u32 JDR1;
	u32 JDR2;
	u32 JDR3;
	u32 JDR4;
	u32 DR;
}ADC_MemMap_t;

#define ADC1 ((volatile ADC_MemMap_t*)ADC1_BASE_ADDR)
#define ADC_COMMON_CCR (*(volatile u32*)(ADC_COMMON_BASE_ADDR + 0x04U))
#define ADC1_CLOCK_BIT 8U

#define ADC_SR_EOC 1U
#define ADC_CR2_ADON 0U
#define ADC_CR2_CONT 1U
#define ADC_CR2_ALIGN 11U
#define ADC_CR2_SWSTART 30U
#define ADC_CR1_RES_BIT 24U
#define ADC_CHANNEL_SAMPLE_84 0b100U





#endif /* MCAL_ADC_ADC_PRV_H_ */
