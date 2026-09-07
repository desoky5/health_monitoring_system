/*
 * S2P_int.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Omar Desoky
 */

#ifndef HAL_S2P_S2P_INT_H_
#define HAL_S2P_S2P_INT_H_
typedef struct{
	u8 DataPort ;
	u8 DataPin ;
	u8 ShiftCLKPort ;
	u8 ShiftCLKPin;
	u8 LatchCLKPort;
	u8 LatchCLKPin;
}S2P_Init_t;

void HS2P_vInit(S2P_Init_t* A_xInit);
void HS2P_vSendData(S2P_Init_t* A_xInit,u32 A_u32Byte);
void HS2P_vLatchData(S2P_Init_t* A_xInit);
void HS2P_vShiftData(S2P_Init_t* A_xInit);
#define S2P_NO_OF_SHIFT_REG 2

#endif /* HAL_S2P_S2P_INT_H_ */
