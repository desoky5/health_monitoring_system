/*
 * DMA_prg.c
 *
 *  Created on: Sep 6, 2026
 *      Author: Omar Desoky
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MTH.h"

#include "DMA_int.h"
#include "DMA_prv.h"

static void (*G_Fptr[8])(void)= {NULL};

void MDMA2_vInit(u8 A_u8StreamID)
{
	// stream is initially disabled
	CLR_BIT(DMA2->Str[A_u8StreamID].CR,0);

	//Memory to Memory
	CLR_BIT(DMA2->Str[A_u8StreamID].CR,6);
	SET_BIT(DMA2->Str[A_u8StreamID].CR,7);

	SET_BIT(DMA2->Str[A_u8StreamID].CR,9);
	SET_BIT(DMA2->Str[A_u8StreamID].CR,10);
	//Enable Interrupt
	SET_BIT(DMA2->Str[A_u8StreamID].CR,4);


}
void MDMA2_vSetStreamCfg(
		u8 A_u8StreamID,
		u32 *A_u32SourceAddr,
		u32 *A_u32DestAddr,
		Size_t A_xPSize,
		Size_t A_xMSize,
		u16 A_u16BlockSize,
		Threshold_t A_xThreshold
		)
{
	//set source and destination address
	DMA2->Str[A_u8StreamID].PAR = (u32) A_u32SourceAddr ;
	DMA2->Str[A_u8StreamID].M0AR = (u32) A_u32DestAddr ;

	//select size
	DMA2->Str[A_u8StreamID].CR &= ~(0b1111<<11);
	DMA2->Str[A_u8StreamID].CR |= (A_xPSize<<11);
	DMA2->Str[A_u8StreamID].CR |= (A_xMSize<<13);

	DMA2->Str[A_u8StreamID].NDTR = A_u16BlockSize;

	//sellect threshold

	DMA2->Str[A_u8StreamID].FCR &= ~(0b11);
	DMA2->Str[A_u8StreamID].FCR = A_xThreshold;
}
void MDMA2_vEnableStream(u8 A_u8StreamID)
{
	SET_BIT(DMA2->Str[A_u8StreamID].CR,0);
}
void MDMA2_vCallBack(u8 A_u8StreamID,void(A_Fptr)(void))
{
 G_Fptr[A_u8StreamID] = A_Fptr;
}
void DMA2_Stream0_IRQHandler(void)
{
	if (G_Fptr[0] !=NULL)
	{
		G_Fptr[0]();
	}
	DMA2->LIFCR |= (1U<<0) | (1U<<2) | (1U<<3) | (1U<<4) | (1U<<5);

}
