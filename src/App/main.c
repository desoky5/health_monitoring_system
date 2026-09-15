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

static void UART_vSendNumber(u32 A_u32Number)
{
    char L_str[11];
    s8 L_s8Index = 0;

    if (A_u32Number == 0U)
    {
        MUSART_vSendData('0');
        return;
    }

    while (A_u32Number > 0U)
    {
        L_str[L_s8Index++] = (char)('0' + (A_u32Number % 10U));
        A_u32Number /= 10U;
    }

    while (--L_s8Index >= 0)
    {
        MUSART_vSendData((u8)L_str[L_s8Index]);
    }
}
int main(void)
{

    MRCC_vInit();
    MRCC_vEnableCLK(RCC_AHB1, RCC_GPIOA);  // Enable GPIOA clock
    MRCC_vEnableCLK(RCC_AHB1, RCC_GPIOB);  // Enable GPIOB clock
    MRCC_vEnableCLK(RCC_APB2, RCC_SPI1);   // Enable SPI1 clock
    MRCC_vEnableCLK(RCC_APB2, RCC_USART1); // Enable USART1 clock
    MRCC_vEnableCLK(RCC_APB2, RCC_USART1); // Enable USART1 clock
    MRCC_vEnableCLK(RCC_APB1, 14);

    MSYSTIC_Config_t DELAY = {
        .InterruptEnable = INT_ENABLE,
        .CLK_SRC = CLK_SRC_AHB_8};
    MSYSTICK_vInit(&DELAY);
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
    GPIOx_PinConfig_t tx = {
        .Port = GPIO_PORTA,
        .Pin = GPIO_PIN9,
        .Mode = GPIO_ALF,
        .OutputSpeed = Output_low_speed,
        .OutputType = OUTPUT_push_pull,
        .AltFunc = GPIO_AF7};
    MGPIO_vInit(&tx);

    GPIOx_PinConfig_t rx = {
        .Port = GPIO_PORTA,
        .Pin = GPIO_PIN10,
        .Mode = GPIO_ALF,
        .PullType = GPIO_OT_NOPULL,
        .AltFunc = GPIO_AF7};
    MGPIO_vInit(&rx);

    MUSART_vInit();
    MUSART_vSendString("System started\r\n");
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
        .AltFunc = GPIO_AF5,
        .OutputSpeed = Output_high_speed,
        .OutputType = OUTPUT_push_pull,
        .PullType = GPIO_OT_NOPULL};
    MGPIO_vInit(&MOSI);

    MSPI_vInit();

    HIMU_vInit();
    MUSART_vSendString("IMU_ID=");
    UART_vSendNumber(HIMU_u8ReadDEVID());
    MUSART_vSendString("\r\n");

    u8 stepEvent;
    u8 activityStatus = 0U;
    u8 prevActivityStatus = 0xFFU; /* Force initial state print */

    u8 counter = 0U;
    /*u32 X_ACC;
    u32 Y_ACC;
    u32 Z_ACC;
    u32 ACC_MAG;*/

    MUSART_vSendString("Steps=");
    UART_vSendNumber(0);
    MUSART_vSendString("\r\n");
    HTFT_vInit();
    HTFT_vFillBackgroundColor(0x0000);
    while (1)
    {
        /*
        //  * Polling avoids making USART output depend on the sensor interrupt
        //  * wiring. The ADXL345 data is sampled at a controlled rate instead.
        //  */
        activityStatus = HIMU_u8ActivityStatus();

        stepEvent = HIMU_u8StepCounter();
        if (stepEvent == 1U)
        {
            counter = (counter < 255U) ? (counter + 1U) : 0U;
            MUSART_vSendString("Steps=");
            UART_vSendNumber(counter);
            HTFT_vWriteNumber(2, 5 ,counter, 0xFFFF);
            MUSART_vSendString("\r\n");
            stepEvent = 0;
        }

        if (activityStatus == prevActivityStatus)
        {
            MSYSTICK_vStartTimer(5000);
        }
        u32 time_passed = MSYSTICK_u32GetElapsedTime_SingleShot();
        if (((activityStatus == prevActivityStatus) && time_passed > 1000) || (activityStatus != prevActivityStatus))
        {
            MSYSTICK_vStopTimer();
            prevActivityStatus = activityStatus;
            switch (activityStatus)
            {
            case 0U:
                MUSART_vSendString("Status: Stopped\r\n");
                break;
            case 1U:
                MUSART_vSendString("Status: SLOW Walk\r\n");

                break;
            case 2U:
                MUSART_vSendString("Status: Mid Walk\r\n");

                break;
            case 3U:
                MUSART_vSendString("Status: Running\r\n");

                break;
            default:
                MUSART_vSendString("Status: NOTHING\r\n");

                break;
            }
        }

        /* X_ACC = HIMU_s16ReadXData();
         Y_ACC = HIMU_s16ReadYData();
         Z_ACC = HIMU_s16ReadZData();
         ACC_MAG = IMU_u32MagnitudeMg(X_ACC, Y_ACC, Z_ACC);

         UART_vSendNumber(ACC_MAG);
         MUSART_vSendString("\r\n");*/

        MSYSTICK_vSetDelay_ms(200U);
    }

    return 0;
}
