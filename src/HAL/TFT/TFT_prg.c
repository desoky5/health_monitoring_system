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
	 LSB = A_u16ImgArray[i]&0x00FF;
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
	u16 MSB = (u8)((A_u16Color & 0xFF00) >> 8);
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
static const u8 TFT_Font5x7[][5] =
{
	{0x00,0x00,0x00,0x00,0x00}, /* space */
	{0x00,0x00,0x5F,0x00,0x00}, /* ! */
	{0x00,0x07,0x00,0x07,0x00}, /* " */
	{0x14,0x7F,0x14,0x7F,0x14}, /* # */
	{0x24,0x2A,0x7F,0x2A,0x12}, /* $ */
	{0x23,0x13,0x08,0x64,0x62}, /* % */
	{0x36,0x49,0x55,0x22,0x50}, /* & */
	{0x00,0x05,0x03,0x00,0x00}, /* ' */
	{0x00,0x1C,0x22,0x41,0x00}, /* ( */
	{0x00,0x41,0x22,0x1C,0x00}, /* ) */
	{0x14,0x08,0x3E,0x08,0x14}, /* * */
	{0x08,0x08,0x3E,0x08,0x08}, /* + */
	{0x00,0x50,0x30,0x00,0x00}, /* , */
	{0x08,0x08,0x08,0x08,0x08}, /* - */
	{0x00,0x60,0x60,0x00,0x00}, /* . */
	{0x20,0x10,0x08,0x04,0x02}, /* / */
	{0x3E,0x51,0x49,0x45,0x3E}, /* 0 */
	{0x00,0x42,0x7F,0x40,0x00}, /* 1 */
	{0x42,0x61,0x51,0x49,0x46}, /* 2 */
	{0x21,0x41,0x45,0x4B,0x31}, /* 3 */
	{0x18,0x14,0x12,0x7F,0x10}, /* 4 */
	{0x27,0x45,0x45,0x45,0x39}, /* 5 */
	{0x3C,0x4A,0x49,0x49,0x30}, /* 6 */
	{0x01,0x71,0x09,0x05,0x03}, /* 7 */
	{0x36,0x49,0x49,0x49,0x36}, /* 8 */
	{0x06,0x49,0x49,0x29,0x1E}, /* 9 */
	{0x00,0x36,0x36,0x00,0x00}, /* : */
	{0x00,0x56,0x36,0x00,0x00}, /* ; */
	{0x08,0x14,0x22,0x41,0x00}, /* < */
	{0x14,0x14,0x14,0x14,0x14}, /* = */
	{0x00,0x41,0x22,0x14,0x08}, /* > */
	{0x02,0x01,0x51,0x09,0x06}, /* ? */
	{0x32,0x49,0x79,0x41,0x3E}, /* @ */
	{0x7E,0x11,0x11,0x11,0x7E}, /* A */
	{0x7F,0x49,0x49,0x49,0x36}, /* B */
	{0x3E,0x41,0x41,0x41,0x22}, /* C */
	{0x7F,0x41,0x41,0x22,0x1C}, /* D */
	{0x7F,0x49,0x49,0x49,0x41}, /* E */
	{0x7F,0x09,0x09,0x09,0x01}, /* F */
	{0x3E,0x41,0x49,0x49,0x7A}, /* G */
	{0x7F,0x08,0x08,0x08,0x7F}, /* H */
	{0x00,0x41,0x7F,0x41,0x00}, /* I */
	{0x20,0x40,0x41,0x3F,0x01}, /* J */
	{0x7F,0x08,0x14,0x22,0x41}, /* K */
	{0x7F,0x40,0x40,0x40,0x40}, /* L */
	{0x7F,0x02,0x0C,0x02,0x7F}, /* M */
	{0x7F,0x04,0x08,0x10,0x7F}, /* N */
	{0x3E,0x41,0x41,0x41,0x3E}, /* O */
	{0x7F,0x09,0x09,0x09,0x06}, /* P */
	{0x3E,0x41,0x51,0x21,0x5E}, /* Q */
	{0x7F,0x09,0x19,0x29,0x46}, /* R */
	{0x46,0x49,0x49,0x49,0x31}, /* S */
	{0x01,0x01,0x7F,0x01,0x01}, /* T */
	{0x3F,0x40,0x40,0x40,0x3F}, /* U */
	{0x1F,0x20,0x40,0x20,0x1F}, /* V */
	{0x3F,0x40,0x38,0x40,0x3F}, /* W */
	{0x63,0x14,0x08,0x14,0x63}, /* X */
	{0x07,0x08,0x70,0x08,0x07}, /* Y */
	{0x61,0x51,0x49,0x45,0x43}  /* Z */
};

static void TFT_vWritePixel(u16 A_u16X, u16 A_u16Y, u16 A_u16Color)
{
	if(A_u16X >= 128 || A_u16Y >= 160)
	{
		return;
	}

	HTFT_vSetXPos(A_u16X, A_u16X);
	HTFT_vSetYPos(A_u16Y, A_u16Y);
	Write_cmd(0x2C);
	Write_data((u8)(A_u16Color >> 8));
	Write_data((u8)A_u16Color);
}

void HTFT_vWriteText(u16 A_u16X, u16 A_u16Y, const char *A_pcText, u16 A_u16Color)
{
	u16 Local_u16X = A_u16X;
	u8 Local_u8Char;
	u8 Local_u8Column;
	u8 Local_u8Row;
	u8 Local_u8FontIndex;

	if(A_pcText == 0)
	{
		return;
	}

	while(*A_pcText != '\0')
	{
		Local_u8Char = (u8)*A_pcText;

		/* Use the upper-case glyph for lower-case input. */
		if(Local_u8Char >= 'a' && Local_u8Char <= 'z')
		{
			Local_u8Char -= ('a' - 'A');
		}

		if(Local_u8Char < ' ' || Local_u8Char > 'Z')
		{
			Local_u8Char = '?';
		}

		Local_u8FontIndex = Local_u8Char - ' ';

		for(Local_u8Column = 0; Local_u8Column < 5; Local_u8Column++)
		{
			for(Local_u8Row = 0; Local_u8Row < 7; Local_u8Row++)
			{
				if((TFT_Font5x7[Local_u8FontIndex][Local_u8Column] &
					(1u << Local_u8Row)) != 0)
				{
					TFT_vWritePixel(Local_u16X + Local_u8Column,
									A_u16Y + Local_u8Row,
									A_u16Color);
				}
			}
		}

		Local_u16X += 6;
		A_pcText++;

		if(Local_u16X >= 128)
		{
			break;
		}
	}
}
