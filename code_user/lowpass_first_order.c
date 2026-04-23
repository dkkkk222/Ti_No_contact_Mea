/*
 * lowpass_first_order.c
 *
 *  Created on: 2025年9月11日
 *      Author: pc
 */
#include "lowpass_first_order.h"

//typedef struct {
//int16_t y_prev; // 历史输出（Q1.15）
//uint8_t vld_dly; // 上一拍的 i_data_vld，用于上升沿检测
//uint8_t o_test; // 和 VHDL 对应的测试标志（本次输出置 1）
//} LPF1_Q15;
//
//// 可选：饱和到 int16_t
static inline int16_t sat_q15(int32_t x)
{
    if (x > 32767) return 32767;
    if (x < -32768) return -32768;
    return (int16_t)x;
}

//static inline void lpf1_q15_init(LPF1_Q15* f, int16_t y0)
//{
//    if (!f) return;
//    f->y_prev = y0; // 也可用首样本初始化以减小启动瞬态
//    f->vld_dly = 0;
//    f->o_test = 0;
//}
//
//// 处理一次输入；在 i_data_vld 上升沿时产生输出并返回 1，否则返回 0
//// i_filter_coeff_q15: α 的 Q1.15 表示，建议范围 [0..32767]
//// i_data_in_q15: 输入样本（Q1.15）
//// 输出：当返回 1 时，o_data_out_q15 为有效输出，o_test 脉冲为 1
//static inline uint8_t lpf1_q15_process(LPF1_Q15 f,
//uint16_t i_filter_coeff_q15,
//int16_t i_data_in_q15,
//uint8_t i_data_vld,
//int16_t* o_data_out_q15,
//uint8_t* o_data_vld,
//uint8_t* o_test)
//{
//    if (!f || !o_data_out_q15 || !o_data_vld || !o_test) return 0;
//
//    // 上升沿检测：前一拍 0，本拍 1
//    uint8_t fire = ((f->vld_dly == 0) && (i_data_vld != 0)) ? 1u : 0u;
//    f->vld_dly = (i_data_vld != 0);
//
//    *o_data_vld = 0;
//    f->o_test = 0;
//    *o_test = 0;
//
//    if (!fire) {
//        return 0;
//    }
//
//    // 约束 α 到 [0..32767]，避免超出 1.0
//    uint32_t alpha = i_filter_coeff_q15;
//    if (alpha > 32767u) alpha = 32767u;
//
//    // β = 32768 - α，注意用 32 位无符号做，避免 0x8000 变负数
//    uint32_t beta = 32768u - alpha;
//
//    // 16x16 -> 32，有符号乘法。x 与 y_prev 为 Q1.15
//    int32_t x = (int32_t)i_data_in_q15;
//    int32_t yprev = (int32_t)f->y_prev;
//
//    // α·x + β·y_prev
//    // 注意：alpha/beta 作为正的 32 位系数参与乘法，匹配硬件做法
//    int32_t ax = (int32_t)(alpha) * x;     // 结果处于 Q16.15（逻辑上 Q2.30）
//    int32_t by = (int32_t)(beta)  * yprev; // 同上
//
//    int32_t sum = ax + by;
//
//    // 右移 15 位回到 Q1.15（C28x 对有符号右移为算术右移）
//    int32_t y32 = (sum >> 15);
//
//    int16_t y = sat_q15(y32);
//
//    // 更新状态并给出输出
//    f->y_prev = y;
//    *o_data_out_q15 = y;
//    *o_data_vld = 1;
//    f->o_test = 1;
//    *o_test = 1;
//
//    return 1;
//}


int16_t lpf1_q15_step(int16_t x_q15, uint16_t alpha_q15, int16_t* y_state_q15)
{
    // 约束 α
    int32_t y32, x, yprev;
    int16_t y;
    uint32_t a, b;
    a = alpha_q15 > 32767u ? 32767u : alpha_q15;
    b = 32768u - a;

    x = (int32_t)x_q15;
    yprev = (int32_t)(*y_state_q15);

    y32 = ((int32_t)a * x + (int32_t)b * yprev) >> 15;
    y = sat_q15(y32);
    *y_state_q15 = y;
    return y;
}








