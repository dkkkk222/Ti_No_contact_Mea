/*
 * ePWM_User.h
 *
 *  Created on: 2025Äê9ÔÂ12ÈÕ
 *      Author: pc
 */

#ifndef CODE_USER_EPWM_USER_H_
#define CODE_USER_EPWM_USER_H_

#include "driverlib.h"
#include "device.h"


#define EPWM1_FREQ          100000UL   // 1 MHz

#define EPWM1_PERIOD        (uint16_t)(DEVICE_SYSCLK_FREQ / (EPWM1_FREQ))


void ePWM_Init(void);

#endif /* CODE_USER_EPWM_USER_H_ */
