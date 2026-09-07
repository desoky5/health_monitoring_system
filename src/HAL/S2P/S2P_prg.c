/*
 * S2P_prg.c
 *
 *  Created on: Aug 30, 2026
 *      Author: Omar Desoky
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MTH.h"

#include "../../MCAL/RCC/RCC_int.h"
#include "../../MCAL/GPIO/GPIO_int.h"
#include "S2P_int.h"


void HS2P_vInit(S2P_Init_t* A_xInit)
{
	GPIOx_PinConfig_t data = {
			.Port = A_xInit->DataPort,
			.Pin = A_xInit->DataPin,
			.Mode = GPIO_Output,
			.OutputType = OUTPUT_push_pull,
			.OutputSpeed = Output_low_speed,
			.PullType= GPIO_OT_NOPULL


	};
	GPIOx_PinConfig_t shift = {
			.Port = A_xInit->ShiftCLKPort,
			.Pin = A_xInit->ShiftCLKPin,
			.Mode = GPIO_Output,
			.OutputType = OUTPUT_push_pull,
			.OutputSpeed = Output_low_speed,
			.PullType= GPIO_OT_NOPULL


	};
	GPIOx_PinConfig_t latch = {
				.Port = A_xInit->LatchCLKPort,
				.Pin = A_xInit->LatchCLKPin,
				.Mode = GPIO_Output,
				.OutputType = OUTPUT_push_pull,
				.OutputSpeed = Output_low_speed,
				.PullType= GPIO_OT_NOPULL


		};
	MGPIO_vInit(&data);
	MGPIO_vInit(&shift);
	MGPIO_vInit(&latch);
}
void HS2P_vSendData(S2P_Init_t* A_xInit,u32 A_u32Byte)
{
	for (u8 i = 0 ; i < 8 * S2P_NO_OF_SHIFT_REG ;i++)
	{
    MGPIO_vSetPinVal(A_xInit->DataPort,A_xInit->DataPin,GET_BIT(A_u32Byte,i));
    HS2P_vShiftData(A_xInit);
    }
    HS2P_vLatchData(A_xInit);
}
void HS2P_vShiftData(S2P_Init_t* A_xInit)
{
	MGPIO_vSetPinVal(A_xInit->ShiftCLKPort,A_xInit->ShiftCLKPin,GPIO_HIGH);
	MSYSTICK_vSetDelay_ms(2);
	MGPIO_vSetPinVal(A_xInit->ShiftCLKPort,A_xInit->ShiftCLKPin,GPIO_LOW);
	MSYSTICK_vSetDelay_ms(2);

}
void HS2P_vLatchData(S2P_Init_t* A_xInit)
{
	MGPIO_vSetPinVal(A_xInit->LatchCLKPort,A_xInit->LatchCLKPin,GPIO_HIGH);
	MSYSTICK_vSetDelay_ms(2);
	MGPIO_vSetPinVal(A_xInit->LatchCLKPort,A_xInit->LatchCLKPin,GPIO_LOW);
	MSYSTICK_vSetDelay_ms(2);
}




