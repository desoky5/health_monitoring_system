/*
 * TFT_prg.c
 *
 *  Created on: Sep 3, 2026
 *      Author: Omar Desoky
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MTH.h"

#include "../../MCAL/GPIO/GPIO_int.h"
#include "../../MCAL/SYSTICK/SYSTICK_int.h"
#include "../../MCAL/SPI/SPI_int.h"

#include "TFT_int.h"

GPIOx_PinConfig_t TFT_RST_PIN =
{
		.Port = GPIO_PORTA,
		.Pin = GPIO_PIN0,
		.Mode =GPIO_Output,
		.OutputType = OUTPUT_push_pull,
		.OutputSpeed = Output_high_speed

				};

GPIOx_PinConfig_t TFT_A0_PIN =
{
		.Port = GPIO_PORTA,
		.Pin = GPIO_PIN1,
		.Mode =GPIO_Output,
		.OutputType = OUTPUT_push_pull,
		.OutputSpeed = Output_high_speed
				};

static void Reset_seq(void)
{
	//RST PIN = 1
	MGPIO_vSetPinVal(TFT_RST_PIN.Port,TFT_RST_PIN.Pin,GPIO_HIGH);
	//DELAY
	MSYSTICK_vSetDelay_us(100);
	//RST PIN = 0
	MGPIO_vSetPinVal(TFT_RST_PIN.Port,TFT_RST_PIN.Pin,GPIO_LOW);
	// delay 1us
	MSYSTICK_vSetDelay_us(1);
	//pin high
	MGPIO_vSetPinVal(TFT_RST_PIN.Port,TFT_RST_PIN.Pin,GPIO_HIGH);
	//delay 100us
	MSYSTICK_vSetDelay_us(100);
	//pin low
	MGPIO_vSetPinVal(TFT_RST_PIN.Port,TFT_RST_PIN.Pin,GPIO_LOW);

	//delay 100us
	MSYSTICK_vSetDelay_us(100);
	//pin high
	MGPIO_vSetPinVal(TFT_RST_PIN.Port,TFT_RST_PIN.Pin,GPIO_HIGH);
	//delay 120 ms
	MSYSTICK_vSetDelay_ms(120);
}


static void write_cmd(u8  A_u8cmd)
{
	MGPIO_vSetPinVal(TFT_A0_PIN.Port,TFT_A0_PIN.Pin,GPIO_LOW);
	(void)MSPI_u8Transcieve(A_u8cmd);
}
static void write_data(u8  A_u8data)
{
	MGPIO_vSetPinVal(TFT_A0_PIN.Port,TFT_A0_PIN.Pin,GPIO_HIGH);
	(void)MSPI_u8Transcieve(A_u8data);
}

	void HTFT_vInit(void)
{
	MGPIO_vInit(&TFT_A0_PIN);
	MGPIO_vInit(&TFT_RST_PIN);

	MSPI_vInit();
	MSYSTIC_Config_t STK_cfg =
	{
			.InterruptEnable = INT_DISABLE,
			.CLK_SRC = CLK_SRC_AHB_8
	};
	//RESET
	Reset_seq();

	//SLEEP PUT
	write_cmd(0x11);
	//delay 15 ms
	MSYSTICK_vSetDelay_ms(15);
	write_cmd(0X3A);
	write_data(0x05);
	write_cmd(0x29);
}
void HTFT_vShowImage(const u16 A_u16ImgArray[],u16 A_u16ImgSize)
{
	u8 MSB = 0 ;
	u8 LSB = 0 ;
	write_cmd(0x2A);
	write_data(0);
	write_data(0);
	write_data(0);
	write_data(127);
    //Y Boundary
	write_cmd(0x2B);
	write_data(0);
	write_data(0);
	write_data(0);
	write_data(159);
	//send image
	write_cmd(0x2C);

	for (u16 i = 0 ; i <A_u16ImgSize; i++)
	{
	 MSB = (A_u16ImgArray[i]&0xFF00)>>8;
	 LSB = A_u16ImgArray[i]&0xFF00;
	 write_data(MSB);
	 write_data(LSB);
	}
}
void HTFT_vSetXPos(u16 A_u16xStart, u16 A_u16xEnd)
{
	write_cmd(0x2A);
	write_data((u8)(A_u16xStart >> 8));
	write_data((u8)(A_u16xStart & 0xFF));
	write_data((u8)(A_u16xEnd   >> 8));
	write_data((u8)(A_u16xEnd   & 0xFF));
}

void HTFT_vSetYPos(u16 A_u16yStart, u16 A_u16yEnd)
{
	write_cmd(0x2B);
	write_data((u8)(A_u16yStart >> 8));
	write_data((u8)(A_u16yStart & 0xFF));
	write_data((u8)(A_u16yEnd   >> 8));
	write_data((u8)(A_u16yEnd   & 0xFF));

}
void HTFT_vFillBackgroundColor(u16 A_u16Color)
{
	u8 MSB = (u8)((A_u16Color & 0xFF00) >> 8);
	u8 LSB = (u8)(A_u16Color & 0x00FF);

	HTFT_vSetXPos(0, 127);
	HTFT_vSetYPos(0, 159);
	write_cmd(0x2C); // Memory Write

	for (u32 i = 0; i < (128u * 160u); i++)
	{
		write_data(MSB);
		write_data(LSB);
	}
}
void HTFT_vFillRectangle(u16 A_u16Color)
{
	u8 MSB = (u8)((A_u16Color & 0xFF00) >> 8);
	u8 LSB = (u8)(A_u16Color & 0x00FF);

	HTFT_vSetXPos(10, 100);
	HTFT_vSetYPos(20, 80);
	write_cmd(0x2C); // Memory Write

	for (u32 i = 0; i < (128u * 160u); i++)
	{
		write_data(MSB);
		write_data(LSB);
	}

}
