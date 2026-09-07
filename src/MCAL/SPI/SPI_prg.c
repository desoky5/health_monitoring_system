/*
 * SPI_prg.c
 *
 *  Created on: Sep 2, 2026
 *      Author: Omar Desoky
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MTH.h"

#include "SPI_int.h"
#include "SPI_prv.h"
#include "SPI_cfg.h"

void MSPI_vInit(void)
{
//	// Disable Bidirectional Data Mode
//	CLR_BIT(SPI1->CR1,15);
//	//Configure Data Frame Format
//	CLR_BIT(SPI1->CR1,11);
	//SW SLAVE
	SET_BIT(SPI1->CR1,SSM);
	SET_BIT(SPI1->CR1,SSI);
	//MASTER SELLECT
	SET_BIT(SPI1->CR1,MSTR);
	//data frame
	CLR_BIT(SPI1->CR1,DFF);
	//clk 1 when idle
	CLR_BIT(SPI1->CR1,CPHA);
	//The first clock transition is the first data capture edge
	CLR_BIT(SPI1->CR1,CPOL);
	CLR_BIT(SPI1->CR1,LSBFIRST);
	//SPI Enabled
	SET_BIT(SPI1->CR1,SPE);
}

u8 MSPI_u8Transcieve(u8 A_u8Data)
{
	//transmit
	while(!GET_BIT(SPI1->SR,TXE));
	SPI1->DR = A_u8Data;
	while(!GET_BIT(SPI1->SR,RXNE));
	return SPI1->DR;

}
