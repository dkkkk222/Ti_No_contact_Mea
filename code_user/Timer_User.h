/*
 * Timer_User.h
 *
 *  Created on: 2025Äê9ÔÂ9ÈÕ
 *      Author: pc
 */

#ifndef CODE_USER_TIMER_USER_H_
#define CODE_USER_TIMER_USER_H_

#include "driverlib.h"
#include "device.h"
#include "DAC_User.h"





void Init_Timer(void);
void initCPUTimers(void);
void configCPUTimer(uint32_t cpuTimer, float period);
interrupt void cpu_timer0_isr(void);
interrupt void cpu_timer1_isr(void);

#endif /* CODE_USER_TIMER_USER_H_ */
