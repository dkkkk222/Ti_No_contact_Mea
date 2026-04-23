/*
 * rampgen.h
 *
 *  Created on: 2025年9月17日
 *      Author: pc
 */

#ifndef RAMPGEN_H
#define RAMPGEN_H

typedef struct {
    _iq Freq;           // Input: Ramp frequency (pu) 输入：斜坡频率
    _iq StepAngleMax;   // Parameter: Maximum step angle (pu) 参数：最大步长角度
    _iq Angle;          // Variable: Step angle (pu) 可变：步长角度
    _iq Gain;           // Input: Ramp gain (pu) 输入：斜坡增益
    _iq Out;            // Output: Ramp signal (pu) 输出：斜坡信号
    _iq Offset;         // Input: Ramp offset (pu) 输入：斜坡偏移
    void  (*calc)(void *);// Pointer to calculation function 指向计算功能的指针
} RAMPGEN;
typedef RAMPGEN RAMPGEN_handle;

#define RAMPGEN_DEFAULTS {0, 0, 0, _IQ(1), 0, _IQ(1), (void (*)(void *))rampgen_calc }


void rampgen_calc(RAMPGEN *v);
#endif // RAMPGEN_H
