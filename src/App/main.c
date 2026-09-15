#include "../LIB/STD_TYPES.h"

#include "../MCAL/RCC/RCC_int.h"
#include "../MCAL/GPIO/GPIO_int.h"
#include "../MCAL/SYSTICK/SYSTICK_int.h"
#include "../MCAL/SPI/SPI_int.h"
#include "../MCAL/USART/USART_int.h"

#include "../MCAL/EXTI/EXTI_int.h"
#include "../MCAL/NVIC/NVIC_int.h"

#include "../HAL/IMU/IMU_int.h"
#include "../HAL/TFT/TFT_int.h"

// SPI Pins
// SCK   PA5
// MISO  PA6
// MOSI  PA7
// ADXL345 Pins
// CS   PA2
// INT1  PA3
// INT2  PA4
// UART Pins
// tx    A9
// rx    A10
static u8 buttonPressed = 0U;
static  u8 pageNumber = 0;


static  u8 G_StepCounter = 0;
static  u8 stepEvent;
static  u8 activityStatus = 0U;
static  u8 prevActivityStatus = 0xFFU;

static  u8 counter = 0U;

void step_counter(void);
void HeartRate(void);

int main(void)
{

	MRCC_vInit();
	MRCC_vEnableCLK(RCC_AHB1, RCC_GPIOA);  // Enable GPIOA clock
	MRCC_vEnableCLK(RCC_AHB1, RCC_GPIOB);  // Enable GPIOB clock
	MRCC_vEnableCLK(RCC_APB2, RCC_SPI1);   // Enable SPI1 clock
	MRCC_vEnableCLK(RCC_APB1, RCC_SPI2);

	GPIOx_PinConfig_t MOSI1 = {
			.Port    = GPIO_PORTB,
			.Pin     = GPIO_PIN15,
			.Mode    = GPIO_ALF,
			.AltFunc = GPIO_AF5
	};
	MGPIO_vInit(&MOSI1);

	GPIOx_PinConfig_t SCK1 = {
			.Port    = GPIO_PORTB,
			.Pin     = GPIO_PIN10,
			.Mode    = GPIO_ALF,
			.AltFunc = GPIO_AF5
	};
	MGPIO_vInit(&SCK1);


	/* Configure SPI1 pins before starting the IMU. */
	GPIOx_PinConfig_t SCK = {
			.Port = GPIO_PORTA,
			.Pin = GPIO_PIN5,
			.Mode = GPIO_ALF,
			.OutputSpeed = Output_high_speed,
			.OutputType = OUTPUT_push_pull,
			.PullType = GPIO_OT_NOPULL,
			.AltFunc = GPIO_AF5};
	MGPIO_vInit(&SCK);

	GPIOx_PinConfig_t MISO = {
			.Port = GPIO_PORTA,
			.Pin = GPIO_PIN6,
			.Mode = GPIO_ALF,
			.AltFunc = GPIO_AF5,
			.PullType = GPIO_OT_NOPULL};
	MGPIO_vInit(&MISO);

	GPIOx_PinConfig_t MOSI = {
			.Port = GPIO_PORTA,
			.Pin = GPIO_PIN7,
			.Mode = GPIO_ALF,
			.OutputSpeed = Output_high_speed,
			.OutputType = OUTPUT_push_pull,
			.PullType = GPIO_OT_NOPULL,
			.AltFunc = GPIO_AF5};
	MGPIO_vInit(&MOSI);

	GPIOx_PinConfig_t nav_button ={
			.Port       = GPIO_PORTB,
			.Pin        = GPIO_PIN2,
			.Mode       = GPIO_Input,
			.PullType   = GPIO_OT_PULLUP
	};
	MGPIO_vInit(&nav_button);


	MSPI_vInit();

	HIMU_vInit();



	HTFT_vInit();
	HTFT_vFillBackgroundColor(TFT_COLOR_BLACK);

	while (1)
	{
		if(pageNumber == 0)
		{
			HeartRate();
		}
		else if(pageNumber == 1)
		{
			step_counter();
		}
	}

	return 0;
}

void step_counter(void)
{
	HTFT_vFillBackgroundColor(TFT_COLOR_BLACK);
	HTFT_vWriteText(25, 0, "Steps Counter", TFT_COLOR_WHITE);
	HTFT_vWriteText(0, 50, "Steps=", TFT_COLOR_WHITE);
	HTFT_vWriteText(0, 90, "Status: ", TFT_COLOR_WHITE);
	while(1)
	{
		activityStatus = HIMU_u8ActivityStatus();
		stepEvent = HIMU_u8StepCounter();
		if (stepEvent == 1U)
		{
			HTFT_vWriteNumber(50, 50, counter, TFT_COLOR_BLACK);
			counter = (counter < 255U) ? (counter + 1U) : 0U;
			MSYSTICK_vSetDelay_ms(100);
			HTFT_vWriteNumber(50, 50, counter, TFT_COLOR_WHITE);
			stepEvent = 0;
		}

		if (activityStatus == prevActivityStatus)
		{
			MSYSTICK_vStartTimer(5000);
		}
		u32 time_passed = MSYSTICK_u32GetElapsedTime_SingleShot();
		if (((activityStatus == prevActivityStatus) && time_passed > 1000) ||
				(activityStatus != prevActivityStatus))
		{
			HTFT_vWriteText(0, 90, "Status: Stopped", TFT_COLOR_BLACK);
			HTFT_vWriteText(0, 90, "Status: SLOW Walk", TFT_COLOR_BLACK);
			HTFT_vWriteText(0, 90, "Status: Mid Walk", TFT_COLOR_BLACK);
			HTFT_vWriteText(0, 90, "Status:  Running", TFT_COLOR_BLACK);
			HTFT_vWriteText(0, 90, "Status: NOTHING", TFT_COLOR_BLACK);

			MSYSTICK_vStopTimer();
			prevActivityStatus = activityStatus;
			switch (activityStatus)
			{
			case 0U:
				HTFT_vWriteText(0, 90, "Status: Stopped", TFT_COLOR_WHITE);
				break;
			case 1U:
				HTFT_vWriteText(0, 90, "Status: SLOW Walk", TFT_COLOR_WHITE);
				break;
			case 2U:
				HTFT_vWriteText(0, 90, "Status: Mid Walk", TFT_COLOR_WHITE);
				break;
			case 3U:
				HTFT_vWriteText(0, 90, "Status:  Running", TFT_COLOR_WHITE);
				break;
			default:
				HTFT_vWriteText(0, 90, "Status: NOTHING", TFT_COLOR_WHITE);
				break;
			}
		}
		MSYSTICK_vSetDelay_ms(150U);
		if(MGPIO_u8GetPinVal(GPIO_PORTB, GPIO_PIN2) == GPIO_LOW)
		{
			if(buttonPressed == 0U)
			{
				MSYSTICK_vSetDelay_ms(10);

				if(MGPIO_u8GetPinVal(GPIO_PORTB, GPIO_PIN2) == GPIO_LOW)
				{
					buttonPressed = 1U;
					pageNumber = 0;
					return;
				}
			}
		}
		else
		{
			buttonPressed = 0U;
		}
	}
}

void HeartRate(void)
{
	HTFT_vFillBackgroundColor(TFT_COLOR_BLACK);
	HTFT_vWriteText(0,0,"Heart Rate Page", TFT_COLOR_WHITE);
	HTFT_vWriteText(0, 50 , "BPM = ", TFT_COLOR_WHITE);
	while(1)
	{
		if(MGPIO_u8GetPinVal(GPIO_PORTB, GPIO_PIN2) == GPIO_LOW)
		{
			if(buttonPressed == 0U)
			{
				MSYSTICK_vSetDelay_ms(10);

				if(MGPIO_u8GetPinVal(GPIO_PORTB, GPIO_PIN2) == GPIO_LOW)
				{
					buttonPressed = 1U;
					pageNumber = 1;
					return;
				}
			}
		}
		else
		{
			buttonPressed = 0U;
		}
	}
}
