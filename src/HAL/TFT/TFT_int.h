/*
 * TFT_int.h
 *
 *  Created on: Sep 3, 2026
 *      Author: Omar Desoky
 */

#ifndef HAL_TFT_TFT_INT_H_
#define HAL_TFT_TFT_INT_H_

/* Common RGB565 colors. */
#define TFT_COLOR_BLACK   0x0000U
#define TFT_COLOR_WHITE   0xFFFFU
#define TFT_COLOR_RED     0xF800U
#define TFT_COLOR_GREEN   0x07E0U
#define TFT_COLOR_BLUE    0x001FU
#define TFT_COLOR_YELLOW  0xFFE0U
#define TFT_COLOR_CYAN    0x07FFU
#define TFT_COLOR_MAGENTA 0xF81FU
#define TFT_COLOR_GRAY    0x8410U
#define TFT_COLOR_ORANGE  0xFD20U

void HTFT_vInit(void);
void HTFT_vShowImage(const u16 A_u16ImgArray[],u16 A_u16ImgSize);
void HTFT_vSetXPos(u16 A_u16xStart, u16 A_u16xEnd);
void HTFT_vSetYPos(u16 A_u16yStart, u16 A_u16yEnd);
void HTFT_vFillBackgroundColor(u16 A_u16Color);
void HTFT_vFillRectangle(u16 A_u16Color);
void HTFT_vWriteText(u16 A_u16X, u16 A_u16Y, const char *A_pcText, u16 A_u16Color);
void HTFT_vWriteNumber(u16 A_u16X, u16 A_u16Y, s32 A_s32Number, u16 A_u16Color);

#define RESET_BIT_PORT GPIO_PORTA
#define RESET_BIT GPIO_PIN0

#endif /* HAL_TFT_TFT_INT_H_ */
