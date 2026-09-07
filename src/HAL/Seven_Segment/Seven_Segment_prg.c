/*
 * Seven_Segment_prg.c
 *
 *  Created on: Aug 22, 2026
 *      Author: Omar Desoky
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MTH.h"
#include "Seven_Segment_int.h"
#include "../../MCAL/GPIO/GPIO_int.h"


// The seven segment map array

u8 SevenSeg_au8Digits[10] = {
    0b0111111,
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111  // 9
};

// Display a number on the seven segment
void HSEVENSEG_vDisplay(u8 A_u8Num)
{
    u8 i;
    for (i = 2; i < 10; i++)
     {
    	//Turn on All seven segment LEDs before displaying new number
           MGPIO_vSetPinVal(GPIO_PORTA, i, GPIO_LOW);
     }
    for (i = 2; i < 10; i++)
    {
    	// Access the number to be displayed using it as array index and display the binary values on the leds in order
        MGPIO_vSetPinVal(GPIO_PORTA, i, GET_BIT(SevenSeg_au8Digits[A_u8Num], i-2));
    }
}
