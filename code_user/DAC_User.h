/*
 * DAC.h
 *
 *  Created on: 2025年9月8日
 *      Author: pc
 */

#ifndef CODE_USER_DAC_USER_H_
#define CODE_USER_DAC_USER_H_


#include "driverlib.h"
#include "device.h"
#include <C28x_FPU_FastRTS.h>
#include <fastrts.h>
#include "math.h"
#include <fpu32/C28x_FPU_FastRTS.h>

#define myDAC0_BASE DACA_BASE
#define myDAC1_BASE DACB_BASE
#define DLOG_SIZE             1024
#define LOW_THD_SINE          0
#define HIGH_PRECISION_SINE   1
#define SINEWAVE_TYPE         LOW_THD_SINE

#define SAMPLE_RATE 160000.0 // 采样率Fs（Hz），由定时器中断频率提供 /
#define TABLE_SIZE 128 // 正弦表长度，建议2^N /
#define PHASE_BITS 32 // 相位累加器位宽，保持32位 /
#define DEFAULT_F_OUT 5000.0 // 默认输出频率（Hz） */


#define DAC_MAX ((1u << 16) - 1)

#define M_PI   3.141592f



extern volatile uint32_t phase_acc;
extern volatile uint32_t phase_inc;
extern uint16_t sine_table[TABLE_SIZE];

void DAC_Init();
void DAC_GPIO_Init(void);
void DACA_setVoltage(float vout, float vref);
static inline uint16_t DAC_codeFromVolt(float vout, float vref);
void DAC_1_Init();

void DDS_SetFrequency(double fout);
void SineTable_Init(float amplitude, float dc_offset);

#endif /* CODE_USER_DAC_USER_H_ */
