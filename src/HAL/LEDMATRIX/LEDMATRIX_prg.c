/*
 * LEDMATRIX_prg.c
 *
 *  Created on: Aug 25, 2026
 *      Author: Omar Desoky
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MTH.h"

#include "../../MCAL/GPIO/GPIO_int.h"
#include "../../MCAL/SYSTICK/SYSTICK_int.h"

#include "LEDMATRIX_int.h"
#include "LEDMATRIX_cfg.h"

static void HLEDMATRIX_vEnableCurrentCol(u8 A_u8ColNo);
static void HLEDMATRIX_vDisableCurrentCol(void);
static void HLEDMATRIX_vSetRowValue(u8 A_u8RowValue);

GPIOx_PinConfig_t *Rows;
GPIOx_PinConfig_t *Cols;
u8 NO_ROWS;
u8 NO_COLS;

//void HLEDMATRIX_vInit(GPIOx_PinConfig_t *A_xRows,u8 A_u8RowsNo,GPIOx_PinConfig_t *A_xCols,u8 A_u8ColsNo)
//{
//	//Reassign the function parameters to the corresponding Global Variables
//
//	Rows = A_xRows;
//	Cols = A_xCols;
//	NO_ROWS = A_u8RowsNo;
//	NO_COLS= A_u8ColsNo;
//
//	for (u8 i = 0 ; i <A_u8RowsNo ; i++)
//	{
//		MGPIO_vInit(&A_xRows[i]);
//	}
//	for (u8 i = 0 ; i <A_u8ColsNo ; i++)
//	{
//		MGPIO_vInit(&A_xCols[i]);
//	}
//
//	MSYSTIC_Config_t STK_cfg = {
//			.InterruptEnable = INT_DISABLE,
//			.CLK_SRC = CLK_SRC_AHB_8
//	};
//	MSYSTIC_vInit(&STK_cfg);
//}
void HLEDMATRIX_vDisplay(u8 A_u8Frame[],u32 A_u32FrameDelay)
{
for (u32 j = 0; j <A_u32FrameDelay;j++)
{
	for(u8 i = 0 ; i < NO_COLS;i++)
	{
		//set raw value
		HLEDMATRIX_vSetRowValue(A_u8Frame[i]);
		//col enable
		HLEDMATRIX_vEnableCurrentCol(i);
		//calc delay
		MSYSTICK_vSetDelay_ms(SCAN_TIME);
		HLEDMATRIX_vDisableCurrentCol();
    }
}
}

static void HLEDMATRIX_vEnableCurrentCol(u8 A_u8ColNo)
{
	MGPIO_vSetPinVal(Cols[A_u8ColNo].Port ,Cols[A_u8ColNo].Pin, GPIO_LOW);
}

static void HLEDMATRIX_vDisableCurrentCol(void)
{
	for (u8 i = 0 ; i < NO_COLS;i++)
	{
		MGPIO_vSetPinVal(Cols[i].Port ,Cols[i].Pin, GPIO_HIGH);
	}
}

static void HLEDMATRIX_vSetRowValue(u8 A_u8RowValue)
{
	for(u8 i = 0 ; i < NO_ROWS;i++)
		{
			MGPIO_vSetPinVal(Rows[i].Port ,Rows[i].Pin, GET_BIT(A_u8RowValue,i));
		}
}
