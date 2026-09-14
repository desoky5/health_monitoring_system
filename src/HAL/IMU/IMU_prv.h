/*
 * IMU_prv.h
 *
 *  Created on: Sep 6, 2026
 *      Author: khale
 */

#ifndef HAL_IMU_IMU_PRV_H_
#define HAL_IMU_IMU_PRV_H_


#define IMU_BASE_ADDR 	0x00U

typedef struct{
u8  DEVID;
u8 Reserved0;
u8 Reserved1;
u8 Reserved2;
u8 THRESH_TAP;
u8 OFSX;
u8 OFSY;
u8 OFSZ;
u8 DUR;
u8 Latent;
u8 Window;
u8 THRESH_ACT;
u8 THRESH_INACT;
u8 TIME_INACT;
u8 ACT_INACT_CTL;
u8 THRESH_FF;
u8 TIME_FF;
u8 TAP_AXES;
u8 ACT_TAP_STATUS;
u8 BW_RATE;
u8 POWER_CTL;
u8 INT_ENABLE;
u8 INT_MAP;
u8 INT_SOURCE;
u8 DATA_FORMAT;
u8 DATAX0;
u8 DATAX1;
u8 DATAY0;
u8 DATAY1;
u8 DATAZ0;
u8 DATAZ1;
u8 FIFO_CTL;
u8 FIFO_STATUS;
}IMU_MemMap_t;


#define IMU ((volatile IMU_MemMap_t*) (IMU_BASE_ADDR))




#endif /* HAL_IMU_IMU_PRV_H_ */
