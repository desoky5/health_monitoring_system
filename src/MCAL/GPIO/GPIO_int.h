/*
   * GPIO_int.h
 *
 *  Created on: Aug 19, 2026
 *      Author: Omar Desoky
 */

#ifndef MCAL_GPIO_GPIO_INT_H_
#define MCAL_GPIO_GPIO_INT_H_
typedef struct{
	u8 Port ;
	u8 Pin  ;
	u8 Mode ;
	u8 OutputType;
	u8 OutputSpeed;
	u8 PullType;
	u8 AltFunc;
}GPIOx_PinConfig_t;

#define GPIO_LOW 0
#define GPIO_HIGH 1
// GPIO PORTS macros
#define GPIO_PORTA 0
#define GPIO_PORTB 1
#define GPIO_PORTC 2

// GPIO PINS macros
#define GPIO_PIN0 0
#define GPIO_PIN1 1
#define GPIO_PIN2 2
#define GPIO_PIN3 3
#define GPIO_PIN4 4
#define GPIO_PIN5 5
#define GPIO_PIN6 6
#define GPIO_PIN7 7
#define GPIO_PIN8 8
#define GPIO_PIN9 9
#define GPIO_PIN10 10
#define GPIO_PIN11 11
#define GPIO_PIN12 12
#define GPIO_PIN13 13
#define GPIO_PIN14 14
#define GPIO_PIN15 15

//Set Alternate Functions
#define GPIO_AF0 0b0000
#define GPIO_AF1 0b0001
#define GPIO_AF2 0b0010
#define GPIO_AF3 0b0011
#define GPIO_AF4 0b0100
#define GPIO_AF5 0b0101
#define GPIO_AF6 0b0110
#define GPIO_AF7 0b0111
#define GPIO_AF8 0b1000
#define GPIO_AF9 0b1001
#define GPIO_AF10 0b1010
#define GPIO_AF11 0b1011
#define GPIO_AF12 0b1100
#define GPIO_AF13 0b1101
#define GPIO_AF14 0b1110
#define GPIO_AF15 0b1111
//GPIO mode Macrs


#define GPIO_Input   0b00
#define GPIO_Output  0b01
#define GPIO_ALF     0b10
#define GPIO_Analog  0b11


//OUTPUT type Modes Macros

#define OUTPUT_push_pull   0
#define OUTPUT_open_drain  1


#define	GPIO_OT_NOPULL 	 0b00
#define	GPIO_OT_PULLUP   0b01
#define	GPIO_OT_PULLDOWN 0b10

//Output Speed Modes
#define Output_low_speed 		0b00
#define Output_medium_speed     0b01
#define Output_high_speed       0b10
#define Output_very_high_speed  0b11



typedef enum {
   LAF0 = 0b0000,
   LAF1 = 0b0001,
   LAF2 = 0b0010,
   LAF3 = 0b0011,
   LAF4 = 0b0100,
   LAF5 = 0b0101,
   LAF6 = 0b0110,
   LAF7 = 0b0111,
   LAF8 = 0b1000,
   LAF9 = 0b1001,
   LAF10 = 0b1010,
   LAF11 = 0b1011,
   LAF12 = 0b1100,
   LAF13 = 0b1101,
   LAF14 = 0b1110,
   LAF15 = 0b1111,
}AFL;

typedef enum {
   HAF0 = 0b0000,
   HAF1 = 0b0001,
   HAF2 = 0b0010,
   HAF3 = 0b0011,
   HAF4 = 0b0100,
   HAF5 = 0b0101,
   HAF6 = 0b0110,
   HAF7 = 0b0111,
   HAF8 = 0b1000,
   HAF9 = 0b1001,
   HAF10 = 0b1010,
   HAF11 = 0b1011,
   HAF12 = 0b1100,
   HAF13 = 0b1101,
   HAF14 = 0b1110,
   HAF15 = 0b1111,
}AFH;

void MGPIO_vInit(GPIOx_PinConfig_t* A_xPinCfg);

void MGPIO_vSetMode(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8Mode);

void MGPIO_vSetOutputType(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8OutputType);

void MGPIO_vSetOutputSpeed(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8OutputSpeed);

void MGPIO_vSetPullType(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8PullType);

void MGPIO_vSetPinVal(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8Value);

void MGPIO_vSetPinVaAtomic(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8Value);

u8 MGPIO_u8GetPinVal(u8 A_u8PortID ,u8 A_u8PinNum);

void MGPIO_vTogPinVal(u8 A_u8PortID ,u8 A_u8PinNum);

void MGPIO_vSetAlt(u8 A_u8PortID ,u8 A_u8PinNum,u8 A_u8AFx);




#endif /* MCAL_GPIO_GPIO_INT_H_ */
