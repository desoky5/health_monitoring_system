/*
 * LEDMATRIX_int.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Omar Desoky
 */

#ifndef HAL_LEDMATRIX_LEDMATRIX_INT_H_
#define HAL_LEDMATRIX_LEDMATRIX_INT_H_

#include "../../HAL/S2P/S2P_int.h"


void HLEDMATRIX_vInit(GPIOx_PinConfig_t *A_xRows,u8 A_u8RowsNo,GPIOx_PinConfig_t *A_xCols,u8 A_u8ColsNo);
void HLEDMATRIX_vDisplay(u8 A_u8Frame[],u32 A_u32FrameDelay);

void HLEDMATRIX_vDisplayFrameS2P(u8 A_u8Frame[], u32 A_u32FrameDelay, u8 A_u8ColNo, S2P_Init_t* S2P_Init);


#endif /* HAL_LEDMATRIX_LEDMATRIX_INT_H_ */
