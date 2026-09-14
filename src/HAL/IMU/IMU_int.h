/*
 * IMU_int.h
 *
 *  Created on: Sep 6, 2026
 *      Author: khale
 */

#ifndef HAL_IMU_IMU_INT_H_
#define HAL_IMU_IMU_INT_H_



void HIMU_vInit(void);

s32 HIMU_s16ReadXData(void);

s16 HIMU_s16ReadYData(void);

s16 HIMU_s16ReadZData(void);

u32 IMU_u32MagnitudeMg(s16 A_s16X, s16 A_s16Y, s16 A_s16Z);
u8 HIMU_u8ReadStatus(void);

u8 HIMU_u8StepCounter(void);

u8 HIMU_u8ReadDEVID(void);

u8 HIMU_u8ActivityStatus(void);

#endif /* HAL_IMU_IMU_INT_H_ */
