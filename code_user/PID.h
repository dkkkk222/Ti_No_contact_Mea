/*
 * PID.h
 *
 *  Created on: 2025Äê9ÔÂ24ÈÕ
 *      Author: pc
 */

#ifndef CODE_USER_PID_H_
#define CODE_USER_PID_H_

#include "driverlib.h"
#include "device.h"
#include "mppt.h"
#include "po_algorithm.h"

#define KP_Volt     0.0008f
#define KI_Volt     0.000008f

#define KP_Cur      0.001f
#define KI_Cur      0.000015f

#define IQ(A)   (A * 4194304)

#define PWM_Enable_Reg   0x1A3C

#define Limmit_UP   0.95

#define Prd_val  (600)

extern float ADC_Volt_out;
extern float ADC_Volt_in;
extern float ADC_Cur_out;
extern float ADC_Cur;
extern float e_k_i_Volt;
extern float Power_filt;

uint32_t regulationPID_Vol_mppt(void);
#endif /* CODE_USER_PID_H_ */
