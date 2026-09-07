/*
 * ADC_cfg.h
 *
 *  Created on: Sep 7, 2026
 *      Author: Omar Desoky
 */

#ifndef MCAL_ADC_ADC_CFG_H_
#define MCAL_ADC_ADC_CFG_H_

#define ADC_SENSOR_CHANNEL 0U
#define ADC_CONVERSION_TIMEOUT 1000000UL

#if ADC_SENSOR_CHANNEL > 15U
#error ADC_SENSOR_CHANNEL must be between 0 and 15
#endif





#endif /* MCAL_ADC_ADC_CFG_H_ */
