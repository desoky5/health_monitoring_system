/*
 * SPI_prv.h
 *
 *  Created on: Sep 2, 2026
 *      Author: Omar Desoky
 */

#ifndef MCAL_SPI_SPI_PRV_H_
#define MCAL_SPI_SPI_PRV_H_

#define SPI1_BASE_ADDR 0x40013000U
#define SPI2_BASE_ADDR 0x40003800U
#define SPI3_BASE_ADDR 0x40003C00U
#define SPI4_BASE_ADDR 0x40013400U

#define DFF 11
#define SSM 9
#define SSI 8
#define LSBFIRST 7
#define SPE 6
#define MSTR 2
#define CPOL 1
#define CPHA 0
#define TXE 1
#define RXNE 0
typedef struct {
	u32 CR1;
	u32 CR2;
	u32 SR;
	u32 DR;
	u32 CRCPR;
	u32 RXCRCR;
	u32 TXCRCR;
}SPIx_MemMap_t;

#define SPI1 ((volatile SPIx_MemMap_t*)(SPI1_BASE_ADDR))
#define SPI2 ((volatile SPIx_MemMap_t*)(SPI2_BASE_ADDR))
#define SPI3 ((volatile SPIx_MemMap_t*)(SPI3_BASE_ADDR))
#define SPI4 ((volatile SPIx_MemMap_t*)(SPI4_BASE_ADDR))

#endif /* MCAL_SPI_SPI_PRV_H_ */
