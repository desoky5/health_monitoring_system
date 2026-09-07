/*
 * ADC_int.h
 *
 *  Created on: Sep 7, 2026
 *      Author: Omar Desoky
 */

#ifndef MCAL_ADC_ADC_INT_H_
#define MCAL_ADC_ADC_INT_H_

#include "../../LIB/STD_TYPES.h"
#include "ADC_cfg.h"

#define ADC_CHANNEL_0  0U
#define ADC_CHANNEL_1  1U
#define ADC_CHANNEL_2  2U
#define ADC_CHANNEL_3  3U
#define ADC_CHANNEL_4  4U
#define ADC_CHANNEL_5  5U
#define ADC_CHANNEL_6  6U
#define ADC_CHANNEL_7  7U
#define ADC_CHANNEL_8  8U
#define ADC_CHANNEL_9  9U
#define ADC_CHANNEL_10 10U
#define ADC_CHANNEL_11 11U
#define ADC_CHANNEL_12 12U
#define ADC_CHANNEL_13 13U
#define ADC_CHANNEL_14 14U
#define ADC_CHANNEL_15 15U

typedef enum
{
	ADC_STATUS_OK = 0U,
	ADC_STATUS_TIMEOUT,
	ADC_STATUS_INVALID_CHANNEL,
	ADC_STATUS_INVALID_ARGUMENT
}ADC_Status_t;

void MADC_vInit(void);
ADC_Status_t MADC_u8Read(u16* A_pu16Value);





#endif /* MCAL_ADC_ADC_INT_H_ */
