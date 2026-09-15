#include "../LIB/STD_TYPES.h"

#include "../MCAL/RCC/RCC_int.h"
#include "../MCAL/GPIO/GPIO_int.h"
#include "../MCAL/ADC/ADC_int.h"
#include "../MCAL/SPI/SPI_int.h"
#include "../MCAL/USART/USART_int.h"

#include "../HAL/Heart_Rate/HEART_RATE_int.h"
#include "../HAL/IMU/IMU_int.h"
#include "../HAL/TFT/TFT_int.h"

#include "../FreeRTOS/FreeRTOS.h"
#include "../FreeRTOS/task.h"
#include "../FreeRTOS/semphr.h"

#define HEART_RATE_TASK_PERIOD_MS  10U
#define MOTION_TASK_PERIOD_MS      100U
#define DISPLAY_TASK_PERIOD_MS     500U
#define UART_TASK_PERIOD_MS        1000U
#define USART1_CLOCK_BIT           4U
#define SCB_CPACR                  (*(volatile u32*)0xE000ED88U)

typedef struct
{
    u16 HeartRateBpm;
    u32 StepCount;
    u32 AccelerationMg;
} Measurements_t;

static Measurements_t G_xMeasurements;
static SemaphoreHandle_t G_xMeasurementsMutex;

static void Peripherals_vInit(void);
static void u32ToStr(u32 A_u32Value, char* A_pcBuffer);
static void Measurements_vGetSnapshot(Measurements_t* A_pxSnapshot);
static void HeartRateTask(void* A_pvParameters);
static void MotionTask(void* A_pvParameters);
static void DisplayTask(void* A_pvParameters);
static void UartTask(void* A_pvParameters);

static void Peripherals_vInit(void)
{
    GPIOx_PinConfig_t L_xTftMosi = { .Port = GPIO_PORTB, .Pin = GPIO_PIN15, .Mode = GPIO_ALF, .AltFunc = GPIO_AF5 };
    GPIOx_PinConfig_t L_xTftSck = { .Port = GPIO_PORTB, .Pin = GPIO_PIN10, .Mode = GPIO_ALF, .AltFunc = GPIO_AF5 };
    GPIOx_PinConfig_t L_xImuSck = { .Port = GPIO_PORTA, .Pin = GPIO_PIN5, .Mode = GPIO_ALF, .OutputSpeed = Output_high_speed, .OutputType = OUTPUT_push_pull, .PullType = GPIO_OT_NOPULL, .AltFunc = GPIO_AF5 };
    GPIOx_PinConfig_t L_xImuMiso = { .Port = GPIO_PORTA, .Pin = GPIO_PIN6, .Mode = GPIO_ALF, .PullType = GPIO_OT_NOPULL, .AltFunc = GPIO_AF5 };
    GPIOx_PinConfig_t L_xImuMosi = { .Port = GPIO_PORTA, .Pin = GPIO_PIN7, .Mode = GPIO_ALF, .OutputSpeed = Output_high_speed, .OutputType = OUTPUT_push_pull, .PullType = GPIO_OT_NOPULL, .AltFunc = GPIO_AF5 };
    GPIOx_PinConfig_t L_xUartTx = { .Port = GPIO_PORTA, .Pin = GPIO_PIN9, .Mode = GPIO_ALF, .OutputType = OUTPUT_push_pull, .OutputSpeed = Output_high_speed, .PullType = GPIO_OT_NOPULL, .AltFunc = GPIO_AF7 };
    GPIOx_PinConfig_t L_xUartRx = { .Port = GPIO_PORTA, .Pin = GPIO_PIN10, .Mode = GPIO_ALF, .OutputType = OUTPUT_push_pull, .OutputSpeed = Output_high_speed, .PullType = GPIO_OT_PULLUP, .AltFunc = GPIO_AF7 };

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
        L_s16X = (s16)HIMU_s16ReadXData();
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

static void DisplayTask(void* A_pvParameters)
{
    TickType_t L_xLastWakeTime = xTaskGetTickCount();
    Measurements_t L_xSnapshot;
    (void)A_pvParameters;

    for (;;)
    {
        Measurements_vGetSnapshot(&L_xSnapshot);
        HTFT_vFillBackgroundColor(TFT_COLOR_BLACK);
        HTFT_vWriteText(0U, 0U, "Health Monitor", TFT_COLOR_WHITE);
        HTFT_vWriteText(0U, 35U, "BPM:", TFT_COLOR_WHITE);
        HTFT_vWriteNumber(80U, 35U, (s32)L_xSnapshot.HeartRateBpm, TFT_COLOR_GREEN);
        HTFT_vWriteText(0U, 75U, "Steps:", TFT_COLOR_WHITE);
        HTFT_vWriteNumber(80U, 75U, (s32)L_xSnapshot.StepCount, TFT_COLOR_YELLOW);
        HTFT_vWriteText(0U, 115U, "Accel mg:", TFT_COLOR_WHITE);
        HTFT_vWriteNumber(120U, 115U, (s32)L_xSnapshot.AccelerationMg, TFT_COLOR_CYAN);
        vTaskDelayUntil(&L_xLastWakeTime, pdMS_TO_TICKS(DISPLAY_TASK_PERIOD_MS));
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

    (void)xTaskCreate(HeartRateTask, "HeartRate", 256U, NULL, 3U, NULL);
    (void)xTaskCreate(MotionTask, "Motion", 256U, NULL, 2U, NULL);
    (void)xTaskCreate(DisplayTask, "Display", 256U, NULL, 1U, NULL);
    (void)xTaskCreate(UartTask, "UART", 256U, NULL, 1U, NULL);
    vTaskStartScheduler();

    for (;;)
    {
    }
}
