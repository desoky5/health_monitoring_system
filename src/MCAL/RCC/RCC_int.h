/*
 * RCC_init.h
 *
 *  Created on: Aug 18, 2026
 *      Author: Omar Desoky
 */

#ifndef MCAL_RCC_RCC_INT_H_
#define MCAL_RCC_RCC_INT_H_

//Buses
#define RCC_AHB1 	0
#define RCC_AHB2 	1
#define RCC_APB1 	2
#define RCC_APB2	3

//Peripherals

#define RCC_GPIOA 		0
#define RCC_GPIOB 		1
#define RCC_GPIOC 		2

void MRCC_vInit(void);

void MRCC_vEnableCLK(u8 A_u8BusID, u8 A_u8PeripheralID);

void MRCC_vDisableCLK(u8 A_u8BusID, u8 A_u8PeripheralID);


#endif /* MCAL_RCC_RCC_INT_H_ */
