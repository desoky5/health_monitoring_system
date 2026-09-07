/*
 * GPIO_prv.h
 *
 *  Created on: Aug 19, 2026
 *      Author: Omar Desoky
 */

#ifndef MCAL_GPIO_GPIO_PRV_H_
#define MCAL_GPIO_GPIO_PRV_H_
//Setting the base addresses for PORTA , PORTB , PORTC
#define GPIOA_BASE_ADDR	0x40020000U
#define GPIOB_BASE_ADDR	0x40020400U
#define GPIOC_BASE_ADDR	0x40020800U

//Setting a data type struct of GPTIO registers to access all ports based on every port base address
typedef struct
{
	u32 MODER;
	u32 OTYPER;
	u32 OSPEEDR;
	u32 PUPDR;
	u32 IDR;
	u32 ODR;
	u32 BSRR;
	u32 LCKR;
	u32 AFRL;
	u32 AFRH;
}GPIOx_MemMap_t;


#define GPIOA 				((volatile GPIOx_MemMap_t*)(GPIOA_BASE_ADDR))
#define GPIOB 				((volatile GPIOx_MemMap_t*)(GPIOB_BASE_ADDR))
#define GPIOC 				((volatile GPIOx_MemMap_t*)(GPIOC_BASE_ADDR))

#endif /* MCAL_GPIO_GPIO_PRV_H_ */
