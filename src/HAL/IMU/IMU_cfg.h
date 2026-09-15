/*
 * IMU_cfg.h
 *
 *  Created on: Sep 6, 2026
 *      Author: khale
 */

#ifndef HAL_IMU_IMU_CFG_H_
#define HAL_IMU_IMU_CFG_H_

#define IMU_CS_Port	GPIO_PORTA

#define IMU_CS_Pin	GPIO_PIN2

#define IMU_INT2_Port	GPIO_PORTA
#define IMU_INT2_Pin	GPIO_PIN4

#define IMU_INT1_Port	GPIO_PORTA
#define IMU_INT1_Pin	GPIO_PIN3

#define IMU_STEP_DELTA_THRESHOLD_MG 200U
#define IMU_STEP_LOCKOUT_SAMPLES    6U
//SDA -> MOSI
//SDO -> MISO

#endif /* HAL_IMU_IMU_CFG_H_ */
