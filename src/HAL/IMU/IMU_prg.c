/*
 * IMU_prg.c
 *
 * ADXL345 SPI driver and simple activity classifier.
 */
#include "../../LIB/STD_TYPES.h"

#include "../../MCAL/SPI/SPI_int.h"
#include "../../MCAL/GPIO/GPIO_int.h"

#include "IMU_int.h"
#include "IMU_cfg.h"

#define ADXL345_DEVID 0x00U
#define ADXL345_THRESH_TAP 0x1DU	// 0b00100000
#define ADXL345_OFSX 0x1EU			// 0b00000000
#define ADXL345_OFSY 0x1FU			// 0b00000000
#define ADXL345_OFSZ 0x20U			// 0b00000000
#define ADXL345_DUR 0x21U			// 0b00000000
#define ADXL345_LATENT 0x22U		// 0b00000000
#define ADXL345_WINDOW 0x23U		// 0b00000000
#define ADXL345_THRESH_ACT 0x24U	// 0b00010000
#define ADXL345_THRESH_INACT 0x25U	// 0b00001000
#define ADXL345_TIME_INACT 0x26U	// 0b00001010
#define ADXL345_ACT_INACT_CTL 0x27U // 0b11111111
#define ADXL345_THRESH_FF 0x28U
#define ADXL345_TIME_FF 0x29U
#define ADXL345_TAP_AXES 0x2AU // 0b00000000
#define ADXL345_ACT_TAP_STATUS 0x2BU
#define ADXL345_BW_RATE 0x2CU	 // 0b00001010
#define ADXL345_POWER_CTL 0x2DU	 // 0b00001000
#define ADXL345_INT_ENABLE 0x2EU // 0b10001000
#define ADXL345_INT_MAP 0x2FU	 // 0b10000000
#define ADXL345_INT_SOURCE 0x30U
#define ADXL345_DATA_FORMAT 0x31U // 0b00001000
#define ADXL345_DATAX0 0x32U
#define ADXL345_DATAX1 0x33U
#define ADXL345_DATAY0 0x34U
#define ADXL345_DATAY1 0x35U
#define ADXL345_DATAZ0 0x36U
#define ADXL345_DATAZ1 0x37U
#define ADXL345_FIFO_CTL 0x38U // 0b00000000
#define ADXL345_FIFO_STATUS 0x39U

#define ADXL345_READ_BIT 0x80U
#define ADXL345_MULTI_BYTE_BIT 0x40U
#define ADXL345_INACTIVE_MG 30U
#define ADXL345_SLOW_MG 150U
#define ADXL345_NORMAL_MG 300U

#define ADXL345_SPI_READ (0x80U)
#define ADXL345_SPI_WRITE (0x00U)
#define ADXL345_SPI_MB (0x40U)
#define ADXL345_ADDR_MASK (0x3FU)

static void IMU_vSelect(void)
{
	MGPIO_vSetPinValue(IMU_CS_Port, IMU_CS_Pin, GPIO_LOW);
}

static void IMU_vDeselect(void)
{
	MGPIO_vSetPinValue(IMU_CS_Port, IMU_CS_Pin, GPIO_HIGH);
}

/* Single-byte write */
static void IMU_vWriteReg(u8 A_u8Reg, u8 A_u8Data)
{
	IMU_vSelect();
	(void)MSPI_vTransev(ADXL345_SPI_WRITE | (A_u8Reg & ADXL345_ADDR_MASK));
	(void)MSPI_vTransev(A_u8Data);
	IMU_vDeselect();
}

/* Single-byte read */
static u8 IMU_u8ReadReg(u8 A_u8Reg)
{
	u8 L_u8Data;

	IMU_vSelect();
	(void)MSPI_vTransev(ADXL345_SPI_READ | (A_u8Reg & ADXL345_ADDR_MASK));
	L_u8Data = MSPI_vTransev(0x00U);
	IMU_vDeselect();

	return L_u8Data;
}

/* Multi-byte burst read (e.g. 6 axis bytes) */
static void IMU_vReadAcceleration(s16 *A_ps16X, s16 *A_ps16Y, s16 *A_ps16Z)
{
	u8 L_u8Data[6];

	IMU_vSelect();
	/* Must include ADXL345_SPI_MB so sensor auto-increments register address */
	(void)MSPI_vTransev(ADXL345_SPI_READ | ADXL345_SPI_MB | (ADXL345_DATAX0 & ADXL345_ADDR_MASK));

	for (u8 i = 0U; i < 6U; i++)
	{
		L_u8Data[i] = MSPI_vTransev(0x00U);
	}
	IMU_vDeselect();

	*A_ps16X = (s16)((u16)L_u8Data[0] | ((u16)L_u8Data[1] << 8));
	*A_ps16Y = (s16)((u16)L_u8Data[2] | ((u16)L_u8Data[3] << 8));
	*A_ps16Z = (s16)((u16)L_u8Data[4] | ((u16)L_u8Data[5] << 8));
}

static u32 integer_sqrt(u32 val)
{
	u32 res = 0;
	u32 bit = 1UL << 30; // The second-to-top bit is set
	while (bit > val)
		bit >>= 2;
	while (bit != 0)
	{
		if (val >= res + bit)
		{
			val -= res + bit;
			res = (res >> 1) + bit;
		}
		else
		{
			res >>= 1;
		}
		bit >>= 2;
	}
	return res;
}

u32 IMU_u32MagnitudeMg(s16 A_s16X, s16 A_s16Y, s16 A_s16Z)
{
	s32 L_s32X = A_s16X;
	s32 L_s32Y = A_s16Y;
	s32 L_s32Z = A_s16Z;
	u32 L_u32MagnitudeSquared = (u32)(L_s32X * L_s32X) +
								(u32)(L_s32Y * L_s32Y) +
								(u32)(L_s32Z * L_s32Z);
	u32 L_u32Magnitude = 0U;

	L_u32Magnitude = integer_sqrt(L_u32MagnitudeSquared);

	/* DATA_FORMAT is full-resolution +/-2 g: 1 count is approximately 4 mg. */
	return L_u32Magnitude * 4U;
}

void HIMU_vInit(void)
{
	static GPIOx_PinConfig_t G_xCS = {
		.Port = IMU_CS_Port,
		.Pin = IMU_CS_Pin,
		.Mode = GPIO_MODE_OUTPUT,
		.OutputType = GPIO_OT_PUSHPULL,
		.PullType = GPIO_NO_PULL,
		.Speed = GPIO_SPEED_HIGH};
	static GPIOx_PinConfig_t IMU_INT1 = {
		.Port = IMU_INT1_Port,
		.Pin = IMU_INT1_Pin,
		.Mode = GPIO_MODE_INPUT,
		.PullType = GPIO_NO_PULL

	};
	static GPIOx_PinConfig_t IMU_INT2 = {
		.Port = IMU_INT2_Port,
		.Pin = IMU_INT2_Pin,
		.Mode = GPIO_MODE_INPUT,
		.PullType = GPIO_NO_PULL

	};
	MGPIO_vPinInit(&G_xCS);
	MGPIO_vPinInit(&IMU_INT1);
	MGPIO_vPinInit(&IMU_INT2);


	IMU_vWriteReg(ADXL345_POWER_CTL, 0x08U); /* Enable measurement mode. */
	IMU_vWriteReg(ADXL345_THRESH_TAP, 0b00100000);
	IMU_vWriteReg(ADXL345_OFSX, 0b00000000);
	IMU_vWriteReg(ADXL345_OFSY, 0b00000000);
	IMU_vWriteReg(ADXL345_OFSZ, 0b00000000);
	IMU_vWriteReg(ADXL345_DUR, 0b00000000);
	IMU_vWriteReg(ADXL345_LATENT, 0b00000000);
	IMU_vWriteReg(ADXL345_WINDOW, 0b00000000);
	IMU_vWriteReg(ADXL345_THRESH_ACT, 0b00010000);
	IMU_vWriteReg(ADXL345_THRESH_INACT, 0b00001000);
	IMU_vWriteReg(ADXL345_TIME_INACT, 0b00001010);
	IMU_vWriteReg(ADXL345_ACT_INACT_CTL, 0b11111111);
	IMU_vWriteReg(ADXL345_BW_RATE, 0b00001010);
	IMU_vWriteReg(ADXL345_POWER_CTL, 0b00001000);
	IMU_vWriteReg(ADXL345_INT_ENABLE, 0b10001000);
	IMU_vWriteReg(ADXL345_INT_MAP, 0b10000000);
	IMU_vWriteReg(ADXL345_DATA_FORMAT, 0b00001000);
	IMU_vWriteReg(ADXL345_FIFO_CTL, 0b00000000);

	IMU_u8ReadReg(ADXL345_INT_SOURCE);
	IMU_u8ReadReg(ADXL345_DEVID);
}

s32 HIMU_s16ReadXData(void)
{

	s16 L_s16X;
	s16 L_s16Y;
	s16 L_s16Z;

	IMU_vReadAcceleration(&L_s16X, &L_s16Y, &L_s16Z);
	return L_s16X;
}


s16 HIMU_s16ReadYData(void)
{
	s16 L_s16X;
	s16 L_s16Y;
	s16 L_s16Z;

	IMU_vReadAcceleration(&L_s16X, &L_s16Y, &L_s16Z);
	return L_s16Y;
}

s16 HIMU_s16ReadZData(void)
{
	s16 L_s16X;
	s16 L_s16Y;
	s16 L_s16Z;

	IMU_vReadAcceleration(&L_s16X, &L_s16Y, &L_s16Z);
	return L_s16Z;
}

u8 HIMU_u8ReadStatus(void)
{
	return IMU_u8ReadReg(ADXL345_INT_SOURCE);
}

u8 HIMU_u8StepCounter(void)
{
	static u8 L_u8PreviousMovement = 0U;
	s16 L_s16X;
	s16 L_s16Y;
	s16 L_s16Z;
	u32 L_u32MagnitudeMg;
	u8 L_u8Movement;

	IMU_vReadAcceleration(&L_s16X, &L_s16Y, &L_s16Z);
	L_u32MagnitudeMg = IMU_u32MagnitudeMg(L_s16X, L_s16Y, L_s16Z);
	L_u8Movement = (L_u32MagnitudeMg < (1000U - ADXL345_SLOW_MG) ||
					L_u32MagnitudeMg > (1000U + ADXL345_SLOW_MG))
					   ? 1U
					   : 0U;

	if ((L_u8Movement == 1U) && (L_u8PreviousMovement == 0U))
	{
		L_u8PreviousMovement = 1U;
		return 1U;
	}

	L_u8PreviousMovement = L_u8Movement;
	return 0U;
}

u8 HIMU_u8ActivityStatus(void)
{
	s16 L_s16X;
	s16 L_s16Y;
	s16 L_s16Z;
	u32 L_u32MagnitudeMg;
	u32 L_u32DeviationMg;

	IMU_vReadAcceleration(&L_s16X, &L_s16Y, &L_s16Z);
	L_u32MagnitudeMg = IMU_u32MagnitudeMg(L_s16X, L_s16Y, L_s16Z);
	L_u32DeviationMg = (L_u32MagnitudeMg > 1000U) ? (L_u32MagnitudeMg - 1000U) : (1000U - L_u32MagnitudeMg);

	if (L_u32DeviationMg < ADXL345_INACTIVE_MG)
	{
		return 0U; /* inactive */
	}
	if (L_u32DeviationMg < ADXL345_SLOW_MG)
	{
		return 1U; /* slow walking */
	}
	if (L_u32DeviationMg < ADXL345_NORMAL_MG)
	{
		return 2U; /* normal walking */
	}
	return 3U; /* fast movement/running */
}


u8 HIMU_u8ReadDEVID(void)
{
	u8 X;
	X= IMU_u8ReadReg(ADXL345_DEVID);
	return X;
}