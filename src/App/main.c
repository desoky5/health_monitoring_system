#include "../LIB/STD_TYPES.h"

#include "../MCAL/RCC/RCC_int.h"
#include "../MCAL/GPIO/GPIO_int.h"
#include "../MCAL/ADC/ADC_int.h"
#include "../MCAL/SPI/SPI_int.h"
#include "../MCAL/USART/USART_int.h"

#include "../HAL/Heart_Rate/HEART_RATE_int.h"
#include "../HAL/IMU/IMU_int.h"
#include "../HAL/TFT/TFT_int.h"
#include"../HAL/LEDMATRIX/LEDMATRIX_int.h"
#include"../HAL/S2P/S2P_int.h"

#include "../FreeRTOS/FreeRTOS.h"
#include "../FreeRTOS/task.h"
#include "../FreeRTOS/semphr.h"

#define HEART_RATE_TASK_PERIOD_MS  10U
#define MOTION_TASK_PERIOD_MS      165U
#define DISPLAY_TASK_PERIOD_MS     100U
#define UART_TASK_PERIOD_MS        1000U
#define NAVIGATION_TASK_PERIOD_MS  20U
#define Display_LEDMATRIX_PERIOD_MS 1000U
#define USART1_CLOCK_BIT           4U
#define SCB_CPACR                  (*(volatile u32*)0xE000ED88U)
#define HEART_RATE_PAGE             0U
#define MOTION_PAGE                 1U
#define STEP_GOAL_COUNT             10U
#define BPM_LOW_THRESHOLD           60U
#define BPM_HIGH_THRESHOLD          100U
#define BPM_SCALE_MAX               180U
#define MOTION_THRESHOLD_MG         1200U
#define PROGRESS_BAR_X              8U
#define PROGRESS_BAR_WIDTH          112U
#define PROGRESS_BAR_HEIGHT         14U
#define RESET_TASK_PERIOD_MS       20U
#define POWER_TASK_PERIOD_MS       20U
#define BUZZER_TASK_PERIOD_MS       2000U


/* 7x7 heart glyph, one byte per row (MSB = leftmost pixel). */
static const u8 G_au8HeartIcon[7] = { 0x36U, 0x7FU, 0x7FU, 0x3EU, 0x1CU, 0x08U, 0x00U };

typedef struct
{
	u16 HeartRateBpm;
	u32 StepCount;
	u32 AccelerationMg;
	u8 SelectedPage;
} Measurements_t;

u8 Stoped[8]={0x00, 0x08, 0x1C, 0x1C, 0x08, 0x3E, 0x08, 0x36};

u8 Walking[8]={0x00, 0x00, 0x18, 0x18, 0x08, 0x08, 0x14, 0x2C};

static u8 G_u8PowerState = 1U; /* 1 = display ON, 0 = display OFF */
static SemaphoreHandle_t G_xPowerMutex;

static Measurements_t G_xMeasurements;
static SemaphoreHandle_t G_xMeasurementsMutex;

static void Peripherals_vInit(void);
static void u32ToStr(u32 A_u32Value, char* A_pcBuffer);
static void Measurements_vGetSnapshot(Measurements_t* A_pxSnapshot);
static void HeartRateTask(void* A_pvParameters);
static void MotionTask(void* A_pvParameters);
static void DisplayTask(void* A_pvParameters);
static void UartTask(void* A_pvParameters);
static void NavigationTask(void* A_pvParameters);
static u16 HeartRate_u16GetStatusColor(u16 A_u16Bpm);
static u16 Display_u16ProgressWidth(u32 A_u32Value, u32 A_u32Max, u16 A_u16BarWidth);
static void Display_LEDMATRIX(void* A_pvParameters);
static void ResetTask(void* A_pvParameters);
static void PowerTask(void* A_pvParameters);
static u8 Power_u8GetState(void);
static void BuzzerTask(void* A_pvParameters);

static void Peripherals_vInit(void)
{
	GPIOx_PinConfig_t L_xTftMosi = { .Port = GPIO_PORTB, .Pin = GPIO_PIN15, .Mode = GPIO_ALF, .AltFunc = GPIO_AF5 };
	GPIOx_PinConfig_t L_xTftSck = { .Port = GPIO_PORTB, .Pin = GPIO_PIN10, .Mode = GPIO_ALF, .AltFunc = GPIO_AF5 };
	GPIOx_PinConfig_t L_xImuSck = { .Port = GPIO_PORTA, .Pin = GPIO_PIN5, .Mode = GPIO_ALF, .OutputSpeed = Output_high_speed, .OutputType = OUTPUT_push_pull, .PullType = GPIO_OT_NOPULL, .AltFunc = GPIO_AF5 };
	GPIOx_PinConfig_t L_xImuMiso = { .Port = GPIO_PORTA, .Pin = GPIO_PIN6, .Mode = GPIO_ALF, .PullType = GPIO_OT_NOPULL, .AltFunc = GPIO_AF5 };
	GPIOx_PinConfig_t L_xImuMosi = { .Port = GPIO_PORTA, .Pin = GPIO_PIN7, .Mode = GPIO_ALF, .OutputSpeed = Output_high_speed, .OutputType = OUTPUT_push_pull, .PullType = GPIO_OT_NOPULL, .AltFunc = GPIO_AF5 };
	GPIOx_PinConfig_t L_xUartTx = { .Port = GPIO_PORTA, .Pin = GPIO_PIN9, .Mode = GPIO_ALF, .OutputType = OUTPUT_push_pull, .OutputSpeed = Output_high_speed, .PullType = GPIO_OT_NOPULL, .AltFunc = GPIO_AF7 };
	GPIOx_PinConfig_t L_xUartRx = { .Port = GPIO_PORTA, .Pin = GPIO_PIN10, .Mode = GPIO_ALF, .OutputType = OUTPUT_push_pull, .OutputSpeed = Output_high_speed, .PullType = GPIO_OT_PULLUP, .AltFunc = GPIO_AF7 };
	GPIOx_PinConfig_t L_xNavigationButton = { .Port = GPIO_PORTB, .Pin = GPIO_PIN2, .Mode = GPIO_Input, .OutputType = OUTPUT_push_pull, .OutputSpeed = Output_low_speed, .PullType = GPIO_OT_PULLUP, .AltFunc = GPIO_AF0 };
	GPIOx_PinConfig_t L_xResetButton = { .Port = GPIO_PORTB, .Pin = GPIO_PIN1, .Mode = GPIO_Input, .OutputType = OUTPUT_push_pull, .OutputSpeed = Output_low_speed, .PullType = GPIO_OT_PULLUP, .AltFunc = GPIO_AF0 };
	GPIOx_PinConfig_t L_xPowerButton = { .Port = GPIO_PORTB, .Pin = GPIO_PIN0, .Mode = GPIO_Input, .OutputType = OUTPUT_push_pull, .OutputSpeed = Output_low_speed, .PullType = GPIO_OT_PULLUP, .AltFunc = GPIO_AF0 };
	GPIOx_PinConfig_t BUZZER_PIN =	{   .Port = GPIO_PORTB, .Pin = GPIO_PIN5, .Mode = GPIO_Output, .OutputType = OUTPUT_push_pull,.OutputSpeed = Output_high_speed};

	MRCC_vEnableCLK(RCC_AHB1, RCC_GPIOA);
	MRCC_vEnableCLK(RCC_AHB1, RCC_GPIOB);
	MRCC_vEnableCLK(RCC_APB2, RCC_SPI1);
	MRCC_vEnableCLK(RCC_APB1, RCC_SPI2);
	MRCC_vEnableCLK(RCC_APB2, USART1_CLOCK_BIT);
	MGPIO_vInit(&L_xTftMosi);
	MGPIO_vInit(&L_xTftSck);
	MGPIO_vInit(&L_xImuSck);
	MGPIO_vInit(&L_xImuMiso);
	MGPIO_vInit(&L_xImuMosi);
	MGPIO_vInit(&L_xUartTx);
	MGPIO_vInit(&L_xUartRx);
	MGPIO_vInit(&L_xNavigationButton);
	MGPIO_vInit(&L_xResetButton);
	MGPIO_vInit(&L_xPowerButton);
	MGPIO_vInit(&BUZZER_PIN);
	MSPI_vInit();
	HIMU_vInit();
	HTFT_vInit();
	MUSART_vInit();
	MADC_vInit();
	HHeartRate_vInit();
}

static void u32ToStr(u32 A_u32Value, char* A_pcBuffer)
{
	char L_cTemp[11];
	u8 L_u8Index = 0U;
	u8 L_u8Count = 0U;

	if (A_u32Value == 0U)
	{
		A_pcBuffer[0] = '0';
		A_pcBuffer[1] = '\0';
		return;
	}

	while (A_u32Value > 0U)
	{
		L_cTemp[L_u8Index++] = (char)('0' + (A_u32Value % 10U));
		A_u32Value /= 10U;
	}
	while (L_u8Index > 0U)
	{
		A_pcBuffer[L_u8Count++] = L_cTemp[--L_u8Index];
	}
	A_pcBuffer[L_u8Count] = '\0';
}

static void Measurements_vGetSnapshot(Measurements_t* A_pxSnapshot)
{
	if (xSemaphoreTake(G_xMeasurementsMutex, portMAX_DELAY) == pdTRUE)
	{
		*A_pxSnapshot = G_xMeasurements;
		(void)xSemaphoreGive(G_xMeasurementsMutex);
	}
}

static void HeartRateTask(void* A_pvParameters)
{
	TickType_t L_xLastWakeTime = xTaskGetTickCount();
	u16 L_u16AdcValue;
	u16 L_u16Bpm;
	(void)A_pvParameters;

	for (;;)
	{
		if (MADC_u8Read(&L_u16AdcValue) == ADC_STATUS_OK)
		{
			HHeartRate_vProcessSample(L_u16AdcValue);
			if (HHeartRate_u8GetBpm(&L_u16Bpm) != 0U)
			{
				if (xSemaphoreTake(G_xMeasurementsMutex, portMAX_DELAY) == pdTRUE)
				{
					G_xMeasurements.HeartRateBpm = L_u16Bpm;
					(void)xSemaphoreGive(G_xMeasurementsMutex);
				}
			}
		}
		vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(HEART_RATE_TASK_PERIOD_MS));
	}
}

static void MotionTask(void* A_pvParameters)
{
	TickType_t L_xLastWakeTime = xTaskGetTickCount();
	s16 L_s16X;
	s16 L_s16Y;
	s16 L_s16Z;
	(void)A_pvParameters;

	for (;;)
	{
		L_s16X = HIMU_s16ReadXData();
		L_s16Y = HIMU_s16ReadYData();
		L_s16Z = HIMU_s16ReadZData();
		if (HIMU_u8StepCounter() != 0U)
		{
			if (xSemaphoreTake(G_xMeasurementsMutex, portMAX_DELAY) == pdTRUE)
			{
				G_xMeasurements.StepCount++;
				(void)xSemaphoreGive(G_xMeasurementsMutex);
			}
		}

		if (xSemaphoreTake(G_xMeasurementsMutex, portMAX_DELAY) == pdTRUE)
		{
			G_xMeasurements.AccelerationMg = IMU_u32MagnitudeMg(L_s16X, L_s16Y, L_s16Z);
			(void)xSemaphoreGive(G_xMeasurementsMutex);
		}
		vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(MOTION_TASK_PERIOD_MS));
	}
}

static u16 HeartRate_u16GetStatusColor(u16 A_u16Bpm)
{
    u16 L_u16Color;

    if (A_u16Bpm == 0U)
    {
        L_u16Color = TFT_COLOR_GRAY;
    }
    else if (A_u16Bpm < BPM_LOW_THRESHOLD)
    {
        L_u16Color = TFT_COLOR_CYAN;
    }
    else if (A_u16Bpm > BPM_HIGH_THRESHOLD)
    {
        L_u16Color = TFT_COLOR_ORANGE;
    }
    else
    {
        L_u16Color = TFT_COLOR_GREEN;
    }

    return L_u16Color;
}

static u16 Display_u16ProgressWidth(u32 A_u32Value, u32 A_u32Max, u16 A_u16BarWidth)
{
    u32 L_u32Clamped = (A_u32Value > A_u32Max) ? A_u32Max : A_u32Value;
    return (u16)((L_u32Clamped * A_u16BarWidth) / A_u32Max);
}

static void DisplayTask(void* A_pvParameters)
{
    TickType_t L_xLastWakeTime = xTaskGetTickCount();
    Measurements_t L_xSnapshot;
    u16 L_u16StatusColor;
    u16 L_u16BarFillWidth;
    u8 L_u8PrevPage = 0xFFU;
    u8 L_u8PrevPowerState = 1U;
    u8 L_u8PowerState;
    (void)A_pvParameters;

    for (;;)
    {
        L_u8PowerState = Power_u8GetState();

        if (L_u8PowerState == 0U)
        {
            if (L_u8PrevPowerState == 1U)
            {
                HTFT_vFillBackgroundColor(TFT_COLOR_BLACK);
            }
            L_u8PrevPowerState = 0U;
            L_u8PrevPage = 0xFFU;
            vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(DISPLAY_TASK_PERIOD_MS));
            continue;
        }
        L_u8PrevPowerState = 1U;

        Measurements_vGetSnapshot(&L_xSnapshot);

        /* Only repaint the full page (border/labels/outlines) when the page actually changes. */
        if (L_xSnapshot.SelectedPage != L_u8PrevPage)
        {
            L_u8PrevPage = L_xSnapshot.SelectedPage;
            HTFT_vFillBackgroundColor(TFT_COLOR_BLACK);
            HTFT_vDrawRectOutline(0U, 0U, 128U, 160U, TFT_COLOR_GRAY);

            if (L_xSnapshot.SelectedPage == HEART_RATE_PAGE)
            {
                HTFT_vWriteText(6U, 6U, "Heart Rate", TFT_COLOR_WHITE);
                HTFT_vDrawBitmap(96U, 4U, 7U, 7U, G_au8HeartIcon, TFT_COLOR_RED, 2U);
                HTFT_vWriteText(6U, 55U, "BPM:", TFT_COLOR_WHITE);
                HTFT_vDrawRectOutline(PROGRESS_BAR_X, 125U, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, TFT_COLOR_WHITE);
            }
            else
            {
                HTFT_vWriteText(6U, 6U, "Steps Counter", TFT_COLOR_WHITE);
                HTFT_vWriteText(6U, 40U, "Steps:", TFT_COLOR_WHITE);
                HTFT_vDrawRectOutline(PROGRESS_BAR_X, 60U, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, TFT_COLOR_WHITE);
                HTFT_vWriteText(6U, 100U, "Accel mg:", TFT_COLOR_WHITE);
            }
        }

        if (L_xSnapshot.SelectedPage == HEART_RATE_PAGE)
        {
            L_u16StatusColor = HeartRate_u16GetStatusColor(L_xSnapshot.HeartRateBpm);

            HTFT_vDrawFilledRect(66U, 55U, 40U, 9U, TFT_COLOR_BLACK);
            HTFT_vWriteNumber(66U, 55U, (s32)L_xSnapshot.HeartRateBpm, L_u16StatusColor);

            HTFT_vDrawFilledRect((u16)(PROGRESS_BAR_X + 1U), 126U, (u16)(PROGRESS_BAR_WIDTH - 2U),
                                 (u16)(PROGRESS_BAR_HEIGHT - 2U), TFT_COLOR_BLACK);
            L_u16BarFillWidth = Display_u16ProgressWidth(L_xSnapshot.HeartRateBpm, BPM_SCALE_MAX, (u16)(PROGRESS_BAR_WIDTH - 2U));
            if (L_u16BarFillWidth > 0U)
            {
                HTFT_vDrawFilledRect((u16)(PROGRESS_BAR_X + 1U), 126U, L_u16BarFillWidth,
                                     (u16)(PROGRESS_BAR_HEIGHT - 2U), L_u16StatusColor);
            }
        }
        else
        {
            HTFT_vDrawFilledRect(76U, 40U, 46U, 9U, TFT_COLOR_BLACK);
            HTFT_vWriteNumber(76U, 40U, (s32)L_xSnapshot.StepCount, TFT_COLOR_YELLOW);

            HTFT_vDrawFilledRect((u16)(PROGRESS_BAR_X + 1U), 61U, (u16)(PROGRESS_BAR_WIDTH - 2U),
                                 (u16)(PROGRESS_BAR_HEIGHT - 2U), TFT_COLOR_BLACK);
            L_u16BarFillWidth = Display_u16ProgressWidth(L_xSnapshot.StepCount, STEP_GOAL_COUNT, (u16)(PROGRESS_BAR_WIDTH - 2U));
            if (L_u16BarFillWidth > 0U)
            {
                HTFT_vDrawFilledRect((u16)(PROGRESS_BAR_X + 1U), 61U, L_u16BarFillWidth,
                                     (u16)(PROGRESS_BAR_HEIGHT - 2U), TFT_COLOR_YELLOW);
            }

            HTFT_vDrawFilledRect(66U, 100U, 48U, 9U, TFT_COLOR_BLACK);
            HTFT_vWriteNumber(66U, 100U, (s32)L_xSnapshot.AccelerationMg, TFT_COLOR_CYAN);

            if (L_xSnapshot.AccelerationMg >= MOTION_THRESHOLD_MG)
            {
            	HTFT_vWriteText(32U, 137U, "STILL", TFT_COLOR_BLACK);
            	 HTFT_vDrawFilledRect(6U, 130U, 20U, 20U, TFT_COLOR_BLACK);

                HTFT_vDrawFilledRect(6U, 130U, 20U, 20U, TFT_COLOR_ORANGE);
                HTFT_vWriteText(32U, 137U, "MOVING", TFT_COLOR_ORANGE);
            }
            else
            {
                 HTFT_vWriteText(32U, 137U, "MOVING", TFT_COLOR_BLACK);
                 HTFT_vDrawFilledRect(6U, 130U, 20U, 20U, TFT_COLOR_BLACK);

                HTFT_vDrawFilledRect(6U, 130U, 20U, 20U, TFT_COLOR_GREEN);
                HTFT_vWriteText(32U, 137U, "STILL", TFT_COLOR_GREEN);
            }
        }
        vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(DISPLAY_TASK_PERIOD_MS));
    }
//	TickType_t L_xLastWakeTime = xTaskGetTickCount();
//	Measurements_t L_xSnapshot;
	(void)A_pvParameters;

	for (;;)
	{
		Measurements_vGetSnapshot(&L_xSnapshot);
		HTFT_vFillBackgroundColor(TFT_COLOR_BLACK);
		if (L_xSnapshot.SelectedPage == HEART_RATE_PAGE)
		{
			HTFT_vWriteText(0U, 0U, "Heart Rate", TFT_COLOR_WHITE);
			HTFT_vWriteText(0U, 50U, "BPM:", TFT_COLOR_WHITE);
			HTFT_vWriteNumber(60U, 50U, (s32)L_xSnapshot.HeartRateBpm, TFT_COLOR_GREEN);
		}
		else
		{
			HTFT_vWriteText(0U, 0U, "Steps Counter", TFT_COLOR_WHITE);
			HTFT_vWriteText(0U, 50U, "Steps:", TFT_COLOR_WHITE);
			HTFT_vWriteNumber(70U, 50U, (s32)L_xSnapshot.StepCount, TFT_COLOR_YELLOW);
			HTFT_vWriteText(0U, 90U, "Accel mg:", TFT_COLOR_WHITE);
			HTFT_vWriteNumber(95U, 90U, (s32)L_xSnapshot.AccelerationMg, TFT_COLOR_CYAN);
		}
		vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(DISPLAY_TASK_PERIOD_MS));
	}
}

static void Display_LEDMATRIX(void* A_pvParameters)
{
	TickType_t L_xLastWakeTime = xTaskGetTickCount();

	S2P_Init_t ACT_STAT={
			.DataPort=GPIO_PORTA,
			.DataPin=GPIO_PIN11,
			.ShiftCLKPort=GPIO_PORTA,
			.ShiftCLKPin=GPIO_PIN12,
			.LatchCLKPort=GPIO_PORTA,
			.LatchCLKPin=GPIO_PIN8
	};
	HS2P_vInit(&ACT_STAT);
	(void)A_pvParameters;
	for (;;)
	{
		u8 Activity=HIMU_u8ActivityStatus();

		if(Activity==0)
		{
			HLEDMATRIX_vDisplayFrameS2P(Stoped,50,8,&ACT_STAT);
		}
		else
		{
			HLEDMATRIX_vDisplayFrameS2P(Walking,50,8,&ACT_STAT);
		}

		vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(Display_LEDMATRIX_PERIOD_MS));

	}
}
static void NavigationTask(void* A_pvParameters)
{
	TickType_t L_xLastWakeTime = xTaskGetTickCount();
	u8 L_u8ButtonWasPressed = 0U;
	(void)A_pvParameters;

	for (;;)
	{
		if (MGPIO_u8GetPinVal(GPIO_PORTB, GPIO_PIN2) == GPIO_LOW)
		{
			if (L_u8ButtonWasPressed == 0U)
			{
				L_u8ButtonWasPressed = 1U;
				if (xSemaphoreTake(G_xMeasurementsMutex, portMAX_DELAY) == pdTRUE)
				{
					G_xMeasurements.SelectedPage =
							(G_xMeasurements.SelectedPage == HEART_RATE_PAGE) ? MOTION_PAGE : HEART_RATE_PAGE;
					(void)xSemaphoreGive(G_xMeasurementsMutex);
				}
			}
		}
		else
		{
			L_u8ButtonWasPressed = 0U;
		}
		vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(NAVIGATION_TASK_PERIOD_MS));
	}
}

static void UartTask(void* A_pvParameters)
{
	TickType_t L_xLastWakeTime = xTaskGetTickCount();
	Measurements_t L_xSnapshot;
	char L_acBpm[6];
	char L_acSteps[11];
	char L_acAcceleration[11];
	(void)A_pvParameters;

	for (;;)
	{
		Measurements_vGetSnapshot(&L_xSnapshot);
		u32ToStr((u32)L_xSnapshot.HeartRateBpm, L_acBpm);
		u32ToStr(L_xSnapshot.StepCount, L_acSteps);
		u32ToStr(L_xSnapshot.AccelerationMg, L_acAcceleration);
		MUSART_vSendString("DATA: BPM=");
		MUSART_vSendString(L_acBpm);
		MUSART_vSendString(", STEPS=");
		MUSART_vSendString(L_acSteps);
		MUSART_vSendString(", ACC=");
		MUSART_vSendString(L_acAcceleration);
		MUSART_vSendString("\r\n");
		vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(UART_TASK_PERIOD_MS));
	}
}
static void ResetTask(void* A_pvParameters)
{
	TickType_t L_xLastWakeTime = xTaskGetTickCount();
	u8 L_u8ButtonWasPressed = 0U;
	(void)A_pvParameters;

	for (;;)
	{
		if (MGPIO_u8GetPinVal(GPIO_PORTB, GPIO_PIN1) == GPIO_LOW)
		{
			if (L_u8ButtonWasPressed == 0U)
			{
				L_u8ButtonWasPressed = 1U;

				HTFT_vDrawFilledRect(0U, 148U, 128U, 12U, TFT_COLOR_BLACK);
				HTFT_vWriteText(2U, 148U, "RST PRESSED", TFT_COLOR_RED);   /* debug */

				if (xSemaphoreTake(G_xMeasurementsMutex, portMAX_DELAY) == pdTRUE)
				{
					G_xMeasurements.HeartRateBpm = 0U;
					G_xMeasurements.StepCount = 0U;
					G_xMeasurements.AccelerationMg = 0U;
					(void)xSemaphoreGive(G_xMeasurementsMutex);
				}

				HTFT_vDrawFilledRect(0U, 148U, 128U, 12U, TFT_COLOR_BLACK);
				HTFT_vWriteText(2U, 148U, "RST DONE", TFT_COLOR_GREEN);   /* debug */
			}
		}
		else
		{
			L_u8ButtonWasPressed = 0U;
		}
		vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(RESET_TASK_PERIOD_MS));
	}
}

static u8 Power_u8GetState(void)
{
	u8 L_u8State = 1U;
	if (xSemaphoreTake(G_xPowerMutex, portMAX_DELAY) == pdTRUE)
	{
		L_u8State = G_u8PowerState;
		(void)xSemaphoreGive(G_xPowerMutex);
	}
	return L_u8State;
}

static void PowerTask(void* A_pvParameters)
{
	TickType_t L_xLastWakeTime = xTaskGetTickCount();
	u8 L_u8ButtonWasPressed = 0U;
	(void)A_pvParameters;

	for (;;)
	{
		if (MGPIO_u8GetPinVal(GPIO_PORTB, GPIO_PIN0) == GPIO_LOW)
		{
			if (L_u8ButtonWasPressed == 0U)
			{
				L_u8ButtonWasPressed = 1U;
				if (xSemaphoreTake(G_xPowerMutex, portMAX_DELAY) == pdTRUE)
				{
					G_u8PowerState = (G_u8PowerState == 1U) ? 0U : 1U;
					(void)xSemaphoreGive(G_xPowerMutex);
				}
			}
		}
		else
		{
			L_u8ButtonWasPressed = 0U;
		}
		vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(POWER_TASK_PERIOD_MS));
	}
}

static void BuzzerTask(void *pvParameters)
{
	TickType_t L_xLastWakeTime = xTaskGetTickCount();
	Measurements_t L_xSnapshot;
	(void)pvParameters;

	for (;;)
	{
		if (xSemaphoreTake(G_xMeasurementsMutex, portMAX_DELAY) == pdTRUE)
		{
			L_xSnapshot = G_xMeasurements;
			(void)xSemaphoreGive(G_xMeasurementsMutex);
		}
		else
		{
			vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(BUZZER_TASK_PERIOD_MS));
			continue;
		}

		if ((L_xSnapshot.StepCount >= STEP_GOAL_COUNT) ||
		    ((L_xSnapshot.HeartRateBpm != 0U) &&
		     ((L_xSnapshot.HeartRateBpm > BPM_HIGH_THRESHOLD) ||
		      (L_xSnapshot.HeartRateBpm < BPM_LOW_THRESHOLD))))
		{
			HTFT_vWriteText(20,20,"GOAL REACHED", TFT_COLOR_WHITE);
			MGPIO_vSetPinVal(GPIO_PORTB, GPIO_PIN5, GPIO_HIGH);
			vTaskDelay(pdMS_TO_TICKS(1000U));
			MGPIO_vSetPinVal(GPIO_PORTB, GPIO_PIN5, GPIO_LOW);
			HTFT_vWriteText(20,20,"GOAL REACHED", TFT_COLOR_BLACK);
		}

		vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(BUZZER_TASK_PERIOD_MS));
	}
}

int main(void)
{
	SCB_CPACR |= (0xFU << 20U);
	MRCC_vInit();
	Peripherals_vInit();

	G_xMeasurementsMutex = xSemaphoreCreateMutex();
	if (G_xMeasurementsMutex == NULL)
	{
		for (;;)
		{
		}
	}

	G_xPowerMutex = xSemaphoreCreateMutex();
	if (G_xPowerMutex == NULL)
	{
		for (;;)
		{
		}
	}

	(void)xTaskCreate(HeartRateTask, "HeartRate", 256U, NULL, 3U, NULL);
	(void)xTaskCreate(MotionTask, "Motion", 256U, NULL, 2U, NULL);
	(void)xTaskCreate(DisplayTask, "Display", 384U, NULL, 1U, NULL);
	(void)xTaskCreate(UartTask, "UART", 256U, NULL, 1U, NULL);
	(void)xTaskCreate(NavigationTask, "Navigation", 128U, NULL, 2U, NULL);
	(void)xTaskCreate(ResetTask, "Reset", 128U, NULL, 2U, NULL);
	(void)xTaskCreate(PowerTask, "Power", 128U, NULL, 2U, NULL);
	//(void)xTaskCreate(Display_LEDMATRIX, "LED Matrix", 256U, NULL, 1U, NULL);
	(void)xTaskCreate(BuzzerTask, "Buzzer", 128U, NULL, 2U, NULL);
	vTaskStartScheduler();

	for (;;)
	{
	}
}
