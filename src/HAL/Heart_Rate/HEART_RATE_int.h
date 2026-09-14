#ifndef HAL_HEART_RATE_HEART_RATE_INT_H_
#define HAL_HEART_RATE_HEART_RATE_INT_H_

#include "../../LIB/STD_TYPES.h"

void HHeartRate_vInit(void);
void HHeartRate_vProcessSample(u16 A_u16AdcSample);
u8 HHeartRate_u8GetBpm(u16* A_pu16Bpm);

#endif /* HAL_HEART_RATE_HEART_RATE_INT_H_ */