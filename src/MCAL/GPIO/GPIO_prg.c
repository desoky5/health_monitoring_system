/*
 * GPIO_prg.c
 *
 *  Created on: Aug 19, 2026
 *      Author: Omar Desoky
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MTH.h"

#include "GPIO_int.h"
#include "GPIO_prv.h"

//Forbidden Pins
/*
 * PORTA : 13 , 14 , 15
 * PORTB : 3 , 4
 */
void MGPIO_vSetMode(u8 A_u8PortID, u8 A_u8PinNum, u8 A_u8Mode)
{
	if ((A_u8PortID == GPIO_PORTA) && (A_u8PinNum == GPIO_PIN13 || A_u8PinNum == GPIO_PIN14 || A_u8PinNum == GPIO_PIN15))
	{
		// forbidden pin on PORTA — handle error
	}
	else if ((A_u8PortID == GPIO_PORTB) &&
	         (A_u8PinNum == GPIO_PIN3 || A_u8PinNum == GPIO_PIN4))
	{
		// forbidden pin on PORTB — handle error
	}
	else
	{
		switch (A_u8PortID)
		{
			case GPIO_PORTA:
				GPIOA->MODER &= ~(0b11 << (2*A_u8PinNum)); // Initially clear the bits before setting the mode
				GPIOA->MODER |= (A_u8Mode << (2*A_u8PinNum)); // Set the pin sellected mode
				break;
			case GPIO_PORTB:
				GPIOB->MODER &= ~(0b11 << (2*A_u8PinNum)); // Initially clear the bits before setting the mode
				GPIOB->MODER |= (A_u8Mode << (2*A_u8PinNum));// Set the pin sellected mode
				break;
			case GPIO_PORTC:
				GPIOC->MODER &= ~(0b11 << (2*A_u8PinNum)); // Initially clear the bits before setting the mode
				GPIOC->MODER |= (A_u8Mode << (2*A_u8PinNum));// Set the pin sellected mode
				break;
			default:
				break;
		}
	}
}


void MGPIO_vSetOutputType(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8OutputType)
{
	if ((A_u8PortID == GPIO_PORTA) && (A_u8PinNum == GPIO_PIN13 || A_u8PinNum == GPIO_PIN14 || A_u8PinNum == GPIO_PIN15))
	{
		// forbidden pin on PORTA — handle error
	}
	else if ((A_u8PortID == GPIO_PORTB) &&(A_u8PinNum == GPIO_PIN3 || A_u8PinNum == GPIO_PIN4))
	{
		// forbidden pin on PORTB — handle error
	}
	else
	{
		switch (A_u8PortID)
		{
			case GPIO_PORTA:
				GPIOA->OTYPER &= ~(0b1 << (A_u8PinNum)); // Initially clear the bits before setting the mode
				GPIOA->OTYPER |= (A_u8OutputType << (A_u8PinNum)); // Set the pin Output Type
				break;
			case GPIO_PORTB:
				GPIOB->OTYPER &= ~(0b1 << (A_u8PinNum)); // Initially clear the bits before setting the mode
				GPIOB->OTYPER |= (A_u8OutputType << (A_u8PinNum)); // Set the pin Output Type
				break;
			case GPIO_PORTC:
				GPIOC->OTYPER &= ~(0b1 << (A_u8PinNum)); // Initially clear the bits before setting the mode
				GPIOC->OTYPER |= (A_u8OutputType << (A_u8PinNum)); // Set the pin Output Type
				break;
			default:
					break;

		}
	}
//	if(A_u8OutputType == OUTPUT_push_pull)
//	{
//		switch (A_u8PortID) {
//			case GPIO_PORTA:
//				CLR_BIT(GPIOA->OTYPER,A_u8PinNum);
//				break;
//			case GPIO_PORTB:
//				CLR_BIT(GPIOB->OTYPER,A_u8PinNum);
//				break;
//			case GPIO_PORTC:
//				CLR_BIT(GPIOC->OTYPER,A_u8PinNum);
//				break;
//		}
//	}
//	else if(A_u8OutputType == OUTPUT_open_drain)
//	{
//		switch (A_u8PortID) {
//			case GPIO_PORTA:
//				SET_BIT(GPIOA->OTYPER,A_u8PinNum);
//				break;
//			case GPIO_PORTB:
//				SET_BIT(GPIOB->OTYPER,A_u8PinNum);
//				break;
//			case GPIO_PORTC:
//				SET_BIT(GPIOC->OTYPER,A_u8PinNum);
//				break;
//
//	}
//	}
}


void MGPIO_vSetOutputSpeed(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8OutputSpeed)
{
	if ((A_u8PortID == GPIO_PORTA) && (A_u8PinNum == GPIO_PIN13 || A_u8PinNum == GPIO_PIN14 || A_u8PinNum == GPIO_PIN15))
	{
		// forbidden pin on PORTA — handle error
	}
	else if ((A_u8PortID == GPIO_PORTB) &&(A_u8PinNum == GPIO_PIN3 || A_u8PinNum == GPIO_PIN4))
	{
		// forbidden pin on PORTB — handle error
	}
	else
	{
		switch (A_u8PortID)
		{
			case GPIO_PORTA:
				GPIOA->OSPEEDR &= ~(0b11 << (2*A_u8PinNum)); // Initially clear the bits before setting the mode
				GPIOA->OSPEEDR |= (A_u8OutputSpeed << (2*A_u8PinNum)); // Set the pin Output Type
				break;
			case GPIO_PORTB:
				GPIOB->OSPEEDR &= ~(0b11 << (2*A_u8PinNum)); // Initially clear the bits before setting the mode
				GPIOB->OSPEEDR |= (A_u8OutputSpeed << (2*A_u8PinNum)); // Set the pin Output Type
				break;
			case GPIO_PORTC:
				GPIOC->OSPEEDR &= ~(0b11 << (2*A_u8PinNum)); // Initially clear the bits before setting the mode
				GPIOC->OSPEEDR |= (A_u8OutputSpeed << (2*A_u8PinNum)); // Set the pin Output Type
				break;
			default:
							break;

		}
	}
}


void MGPIO_vSetPullType(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8PullType) // u8 type : GPIO_OT_NOPULL  , GPIO_OT_PULLUP ,GPIO_OT_PULLDOWN
{
	if ((A_u8PortID == GPIO_PORTA) && (A_u8PinNum == GPIO_PIN13 || A_u8PinNum == GPIO_PIN14 || A_u8PinNum == GPIO_PIN15))
		{
			// forbidden pin on PORTA — handle error
		}
		else if ((A_u8PortID == GPIO_PORTB) &&(A_u8PinNum == GPIO_PIN3 || A_u8PinNum == GPIO_PIN4))
		{
			// forbidden pin on PORTB — handle error
		}
		else
		{
			switch (A_u8PortID)
			{
				case GPIO_PORTA:
					GPIOA->PUPDR &= ~(0b11 << (2*A_u8PinNum)); // Initially clear the bits before setting the mode
					GPIOA->PUPDR |= (A_u8PullType << (2*A_u8PinNum)); // Set the pin Output Type
					break;
				case GPIO_PORTB:
					GPIOB->PUPDR &= ~(0b11 << (2*A_u8PinNum)); // Initially clear the bits before setting the mode
					GPIOB->PUPDR |= (A_u8PullType << (2*A_u8PinNum)); // Set the pin Output Type
					break;
				case GPIO_PORTC:
					GPIOC->PUPDR &= ~(0b11 << (2*A_u8PinNum)); // Initially clear the bits before setting the mode
					GPIOC->PUPDR |= (A_u8PullType << (2*A_u8PinNum)); // Set the pin Output Type
					break;
				default:
								break;
			}
		}
}

void MGPIO_vSetPinVal(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8Value)
{
	if ((A_u8PortID == GPIO_PORTA) && (A_u8PinNum == GPIO_PIN13 || A_u8PinNum == GPIO_PIN14 || A_u8PinNum == GPIO_PIN15))
			{
				// forbidden pin on PORTA — handle error
			}
			else if ((A_u8PortID == GPIO_PORTB) &&(A_u8PinNum == GPIO_PIN3 || A_u8PinNum == GPIO_PIN4))
			{
				// forbidden pin on PORTB — handle error
			}
			else
			{
				if(A_u8Value == GPIO_LOW)
				{
						switch (A_u8PortID) {
							case GPIO_PORTA:
								CLR_BIT(GPIOA->ODR,A_u8PinNum);
								break;
							case GPIO_PORTB:
								CLR_BIT(GPIOB->ODR,A_u8PinNum);
								break;
							case GPIO_PORTC:
								CLR_BIT(GPIOC->ODR,A_u8PinNum);
								break;
						}
					}
					else if(A_u8Value == GPIO_HIGH)
					{
						switch (A_u8PortID) {
							case GPIO_PORTA:
								SET_BIT(GPIOA->ODR,A_u8PinNum);
								break;
							case GPIO_PORTB:
								SET_BIT(GPIOB->ODR,A_u8PinNum);
								break;
							case GPIO_PORTC:
								SET_BIT(GPIOC->ODR,A_u8PinNum);
								break;
							default:
											break;

					}
					}
			}
}

u8 MGPIO_u8GetPinVal(u8 A_u8PortID, u8 A_u8PinNum)
{
	u8 L_u8Value = 0;

	if ((A_u8PortID == GPIO_PORTA) &&
	    (A_u8PinNum == GPIO_PIN13 || A_u8PinNum == GPIO_PIN14 || A_u8PinNum == GPIO_PIN15))
	{
		// forbidden pin on PORTA — handle error
	}
	else if ((A_u8PortID == GPIO_PORTB) &&
	         (A_u8PinNum == GPIO_PIN3 || A_u8PinNum == GPIO_PIN4))
	{
		// forbidden pin on PORTB — handle error
	}
	else
	{
		switch (A_u8PortID)
		{
			case GPIO_PORTA:
				L_u8Value = GET_BIT(GPIOA->IDR, A_u8PinNum);
				break;
			case GPIO_PORTB:
				L_u8Value = GET_BIT(GPIOB->IDR, A_u8PinNum);
				break;
			case GPIO_PORTC:
				L_u8Value = GET_BIT(GPIOC->IDR, A_u8PinNum);
				break;
			default:
				break;
		}
	}

	return L_u8Value;
}

void MGPIO_vTogPinVal(u8 A_u8PortID ,u8 A_u8PinNum)
{

	if ((A_u8PortID == GPIO_PORTA) &&(A_u8PinNum == GPIO_PIN13 || A_u8PinNum == GPIO_PIN14 || A_u8PinNum == GPIO_PIN15))
	{
		// forbidden pin on PORTA — handle error
	}
	else if ((A_u8PortID == GPIO_PORTB) &&(A_u8PinNum == GPIO_PIN3 || A_u8PinNum == GPIO_PIN4))
	{
		// forbidden pin on PORTB — handle error
	}
	else
	{
		switch (A_u8PortID)
		{
			case GPIO_PORTA:
				TOG_BIT(GPIOA->ODR, A_u8PinNum);
				break;
			case GPIO_PORTB:
				TOG_BIT(GPIOB->ODR, A_u8PinNum);
				break;
			case GPIO_PORTC:
				TOG_BIT(GPIOC->ODR, A_u8PinNum);
				break;
			default:
				break;
		}
	}
}

void MGPIO_vSetAlt(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8AFx)
{
	if ((A_u8PortID == GPIO_PORTA) && (A_u8PinNum == GPIO_PIN13 || A_u8PinNum == GPIO_PIN14 || A_u8PinNum == GPIO_PIN15))
			{
				// forbidden pin on PORTA — handle error
			}
			else if ((A_u8PortID == GPIO_PORTB) &&(A_u8PinNum == GPIO_PIN3 || A_u8PinNum == GPIO_PIN4))
			{
				// forbidden pin on PORTB — handle error
			}
			else
			{
				if (A_u8PinNum<8)
					{
				switch (A_u8PortID)
				{

					case GPIO_PORTA:

							GPIOA->AFRL &= ~(0b1111 << (4*A_u8PinNum)); // Initially clear the bits before setting the mode
							GPIOA->AFRL |= (A_u8AFx << (4*A_u8PinNum));

						break;
					case GPIO_PORTB:
							GPIOB->AFRL &= ~(0b1111 << (4*A_u8PinNum)); // Initially clear the bits before setting the mode
							GPIOB->AFRL |= (A_u8AFx << (4*A_u8PinNum));
						break;
					case GPIO_PORTC:
							GPIOC->AFRL &= ~(0b1111 << (4*A_u8PinNum)); // Initially clear the bits before setting the mode
							GPIOC->AFRL |= (A_u8AFx << (4*A_u8PinNum));
						break;
					default:
						break;
				}}
						else if ((A_u8PinNum>7) &( A_u8PinNum<16))
						{
							switch(A_u8PortID)
							{
						case GPIO_PORTA:
								GPIOA->AFRH &= ~(0b1111 << (4*(A_u8PinNum-8))); // Initially clear the bits before setting the mode
								GPIOA->AFRH |= (A_u8AFx << (4*(A_u8PinNum-8)));
							break;
						case GPIO_PORTB:
								GPIOB->AFRH &= ~(0b1111 << (4*(A_u8PinNum-8))); // Initially clear the bits before setting the mode
								GPIOB->AFRH |= (A_u8AFx << (4*(A_u8PinNum-8)));
							break;
						case GPIO_PORTC:
								GPIOC->AFRH &= ~(0b1111 << (4*(A_u8PinNum-8))); // Initially clear the bits before setting the mode
								GPIOC->AFRH |= (A_u8AFx << (4*(A_u8PinNum-8)));
							break;
						default:
							break;
						}
				}
			}
}

void MGPIO_vSetPinVaAtomic(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8Value)
{
	if ((A_u8PortID == GPIO_PORTA) && (A_u8PinNum == GPIO_PIN13 || A_u8PinNum == GPIO_PIN14 || A_u8PinNum == GPIO_PIN15))
				{
					// forbidden pin on PORTA — handle error
				}
				else if ((A_u8PortID == GPIO_PORTB) &&(A_u8PinNum == GPIO_PIN3 || A_u8PinNum == GPIO_PIN4))
				{
					// forbidden pin on PORTB — handle error
				}
				else
				{
					//Bit Set
					if (A_u8Value == GPIO_HIGH)
					{
						switch (A_u8PortID)
						{
						case GPIO_PORTA:
							GPIOA->BSRR = 1<<(A_u8PinNum);
							break;
						case GPIO_PORTB:
							GPIOB->BSRR = 1<<(A_u8PinNum);
							break;
						case GPIO_PORTC:
							GPIOC->BSRR = 1<<(A_u8PinNum);
							break;
						}
						}
						else if (A_u8Value == GPIO_LOW)
						{
							switch (A_u8PortID)
								{
									case GPIO_PORTA:
										GPIOA->BSRR = 1<<(A_u8PinNum+16);
										break;
									case GPIO_PORTB:
										GPIOB->BSRR = 1<<(A_u8PinNum+16);
										break;
									case GPIO_PORTC:
										GPIOC->BSRR = 1<<(A_u8PinNum+16);
										break;
						}
					}
				}
}
void MGPIO_vInit(GPIOx_PinConfig_t* A_xPinCfg)
{
	MGPIO_vSetMode(A_xPinCfg->Port, A_xPinCfg->Pin, A_xPinCfg->Mode);
	MGPIO_vSetOutputType(A_xPinCfg->Port, A_xPinCfg->Pin, A_xPinCfg->OutputType);
	MGPIO_vSetOutputSpeed(A_xPinCfg->Port, A_xPinCfg->Pin, A_xPinCfg->OutputSpeed);
	MGPIO_vSetPullType(A_xPinCfg->Port, A_xPinCfg->Pin, A_xPinCfg->PullType);

	if (A_xPinCfg->Mode == GPIO_ALF)
	{
		MGPIO_vSetAlt(A_xPinCfg->Port, A_xPinCfg->Pin, A_xPinCfg->AltFunc);
	}
}
