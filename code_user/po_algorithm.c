/*
 * po_algorithm.c
 *
 *  Created on: 2025年9月28日
 *      Author: pc
 */


#include <stdint.h>
#include <string.h>
#include "po_algorithm.h"

po_algorithm_t  s;
po_inputs_t     po_input;
po_outputs_t    po_output;
uint16_t vout_ini_flag;

static inline uint16_t sat_add16(uint16_t a, uint16_t b)
{
    uint16_t s = (uint16_t)a + (uint16_t)b;
    return (s > 0xFFFFu) ? 0xFFFFu : (uint16_t)s;
}

static inline uint16_t sat_sub16(uint16_t a, uint16_t b)
{
    return (a >= b) ? (uint16_t)(a - b) : 0u;
}

static inline float u16x16_to_u32(float a, float b)
{
    return (float)a * (float)b;
}



/* 复位（等价于 VHDL 的 ALG_RST/禁能时的清零） */
static void po_algorithm_reset(po_algorithm_t s_data)
{
    memset(&s_data, 0, sizeof(s_data));
    s_data.s_state_alg = PO_STATE_IDLE;
    s_data.set_po_step = 0x0002u; // 默认步进 */
}

/* 初始化 */
void po_algorithm_init(po_algorithm_t s_data)
{
    po_algorithm_reset(s_data);
}

//* 单次调用：建议在50ms定时中断中调用一次。
//约定：当需要执行一次算法调整时，将 i_po_alg_start=1 传入本函数。 /
void po_algorithm_step(const po_inputs_t in)
{
    int guard = 0;
    //po_algorithm_t s = *s_t;
    /* 输出先回传当前寄存器值（等价于 VHDL 输出寄存器跟随内部寄存器） */
    s.set_voltage_ref = po_output.o_voltage_ref;
    //po_po_outputput.o_po_alg_done = s.po_alg_done;

    /* 步进设置进位过程（等价于 VHDL 的 set_po_step 过程） */
    if (in.i_po_alg_step > 2 && in.i_po_alg_step < 249)
    {
        s.set_po_step = in.i_po_alg_step;
    }
    else
    {
        s.set_po_step = 0x0002u;
    }

/* 使能判断：未使能则清零并退出 */
    if (!in.i_po_alg_en)
    {
        po_algorithm_reset(s);
        po_output.o_voltage_ref = s.set_voltage_ref;
        po_output.o_po_alg_done = s.po_alg_done;
        return;
    }

    /* 为避免因软件一次调用需要跨越多个状态，这里用一个小的状态迭代，
       一次 step 完成从 IDLE . ... . ADJUST_DONE/ALG_RST . IDLE 的逻辑。 */

    for (guard = 0 ; guard < 16; ++guard)
    {
        switch (s.s_state_alg)
        {
            case PO_STATE_IDLE:
                if (in.i_po_alg_start)
                {
                    /* 捕获参数与量测 */
                    s.set_chg_vmax     = in.i_set_chg_vmax;
                    s.set_chg_vmin     = in.i_set_chg_vmin;
                    s.set_chg_imax     = in.i_set_chg_imax;
                    s.set_lppt_vmax    = in.i_set_lppt_vmax;
                    s.set_lppt_vmin    = in.i_set_lppt_vmin;
                    s.set_lppt_pout    = in.i_set_lppt_pout;
                    s.set_iniprot_vol  = in.i_set_iniprot_vol;
                    s.set_iniprot_cur  = in.i_set_iniprot_cur;

                    s.set_voltage_MAX  = in.i_pwm_vout_max;
                    s.set_voltage_MIN  = in.i_pwm_vout_min;

                    s.vi_ave           = in.i_vi_ave;
                    s.ii_ave           = in.i_ii_ave;
                    s.vo_ave           = in.i_vo_ave;
                    s.io_ave           = in.i_io_ave;
                    s.pout_ave         = u16x16_to_u32(in.i_vo_ave, in.i_io_ave);

                    s.delta_voltage_step = s.set_po_step;
                    s.po_alg_done        = 0u;

                    if (vout_ini_flag)
                    {
                        s.s_state_alg = PO_STATE_VIN_VOUT_ADJ;
                    }
                    else
                    {
                        s.set_voltage_ref = in.i_set_pi_vref;
                        s.s_state_alg = PO_STATE_ADJ_INI;
                    }
                }
                else
                {
                    /* 保持空闲 */
                }
                break;

            case PO_STATE_ADJ_INI:
                if ((s.vo_ave < s.set_iniprot_vol) && (s.io_ave < s.set_iniprot_cur))
                {
                    vout_ini_flag = 1u;
                    s.s_state_alg = PO_STATE_VIN_VOUT_ADJ;
                }
                else
                {
                    vout_ini_flag = 0u;
                    s.s_state_alg = PO_STATE_IDLE;
                }
                break;

            case PO_STATE_VIN_VOUT_ADJ:
            {
                uint16_t max_minus_d = sat_sub16(s.set_voltage_MAX, s.delta_voltage_step);
                uint16_t min_plus_d  = sat_add16(s.set_voltage_MIN, s.delta_voltage_step);

                if (s.vo_ave > s.set_chg_vmax)
                {
                    if (s.set_voltage_ref >= max_minus_d)
                    {
                        s.s_state_alg = PO_STATE_ALG_RST;
                    }
                    else
                    {
                        s.po_alg_dirt = 1u; /* 往上调 */
                        s.s_state_alg = PO_STATE_DC_ADJUST;
                    }
                }
                else if (s.vo_ave < s.set_chg_vmin)
                {
                    if (s.set_voltage_ref <= min_plus_d)
                    {
                        s.s_state_alg = PO_STATE_ALG_RST;
                    }
                    else
                    {
                        s.po_alg_dirt = 0u; /* 往下调 */
                        s.s_state_alg = PO_STATE_DC_ADJUST;
                    }
                }
                else if (s.vi_ave > s.set_lppt_vmax)
                {
                    if (s.set_voltage_ref <= min_plus_d)
                    {
                        s.s_state_alg = PO_STATE_ALG_RST;
                    }
                    else
                    {
                        s.po_alg_dirt = 0u; /* 往下调 */
                        s.s_state_alg = PO_STATE_DC_ADJUST;
                    }
                }
                else if (s.vi_ave < s.set_lppt_vmin)
                {
                    if (s.set_voltage_ref >= max_minus_d)
                    {
                        s.s_state_alg = PO_STATE_ALG_RST;
                    }
                    else
                    {
                        s.po_alg_dirt = 1u; /* 往上调 */
                        s.s_state_alg = PO_STATE_DC_ADJUST;
                    }
                }
                else
                {
                    if (s.io_ave < s.set_iniprot_cur)
                    {
                        if (in.i_po_alg_mode)
                        {
                            s.s_state_alg = PO_STATE_LPPT_ADJ_MODE;
                        }
                        else
                        {
                            s.s_state_alg = PO_STATE_MPPT_ADJ_MODE;
                        }
                    }
                    else
                    {
                        /* 保持方向不变，直接去调整 */
                        s.s_state_alg = PO_STATE_DC_ADJUST;
                    }
                }
            }
            break;

            case PO_STATE_MPPT_ADJ_MODE:
                if (s.io_ave < s.set_chg_imax)
                {
                    if (s.pout_ave > s.pout_ave_last)
                    {
                        /* 方向保持 */
                    }
                    else
                    {
                        s.po_alg_dirt = (uint16_t)!s.po_alg_dirt;
                    }
                }
                else
                {
                    if (in.i_po_mppt_mode == 0u)
                    {
                        if ((s.pout_ave > s.pout_ave_last && s.vi_ave < s.vi_ave_last) ||
                            (s.pout_ave < s.pout_ave_last && s.vi_ave > s.vi_ave_last))
                        {
                            s.po_alg_dirt = 0u;
                        }
                        else
                        {
                            if (s.pout_ave < s.pout_ave_last)
                            {
                                /* 方向保持 */
                            }
                            else
                            {
                                s.po_alg_dirt = (uint16_t)!s.po_alg_dirt;
                            }
                        }
                    }
                    else
                    {
                        if (s.pout_ave < s.pout_ave_last)
                        {
                            /* 方向保持 */
                        }
                        else
                        {
                            s.po_alg_dirt = (uint16_t)!s.po_alg_dirt;
                        }
                    }
                }
                s.s_state_alg = PO_STATE_DC_ADJUST;
                break;

            case PO_STATE_LPPT_ADJ_MODE:
                if (s.pout_ave < s.set_lppt_pout)
                {
                    if (s.pout_ave > s.pout_ave_last)
                    {
                        /* 方向保持 */
                    }
                    else
                    {
                        s.po_alg_dirt = (uint16_t)!s.po_alg_dirt;
                    }
                }
                else
                {
                    if (in.i_po_lppt_mode == 0u)
                    {
                        if ((s.pout_ave > s.pout_ave_last && s.vi_ave < s.vi_ave_last) ||
                            (s.pout_ave < s.pout_ave_last && s.vi_ave > s.vi_ave_last))
                        {
                            s.po_alg_dirt = 0u;
                        }
                        else
                        {
                            if (s.pout_ave < s.pout_ave_last)
                            {
                                /* 方向保持 */
                            }
                            else
                            {
                                s.po_alg_dirt = (uint16_t)!s.po_alg_dirt;
                            }
                        }
                    }
                    else
                    {
                        if (s.pout_ave < s.pout_ave_last)
                        {
                            /* 方向保持 */
                        }
                        else
                        {
                            s.po_alg_dirt = (uint16_t)!s.po_alg_dirt;
                        }
                    }
                }
                s.s_state_alg = PO_STATE_DC_ADJUST;
                break;

            case PO_STATE_DC_ADJUST:
                if (s.po_alg_dirt)
                {
                    /* 往上调 */
                    uint16_t bound = sat_sub16(s.set_voltage_MAX, s.set_po_step);
                    if (s.set_voltage_ref <= bound)
                    {
                        s.set_voltage_ref = sat_add16(s.set_voltage_ref, s.delta_voltage_step);
                    }
                    else
                    {
                        s.set_voltage_ref = s.set_voltage_MAX;
                    }
                }
                else
                {
                    /* 往下调 */
                    uint16_t bound = sat_add16(s.set_voltage_MIN, s.set_po_step);
                    if (s.set_voltage_ref >= bound)
                    {
                        s.set_voltage_ref = sat_sub16(s.set_voltage_ref, s.delta_voltage_step);
                    }
                    else
                    {
                        s.set_voltage_ref = s.set_voltage_MIN;
                    }
                }
                s.s_state_alg = PO_STATE_ADJUST_DONE;
                break;

            case PO_STATE_ADJUST_DONE:
                s.pout_ave_last = s.pout_ave;
                s.vi_ave_last   = s.vi_ave;
                s.po_alg_done   = 1u;
                s.s_state_alg   = PO_STATE_IDLE;
                break;

            case PO_STATE_ALG_RST:
                /* 与 VHDL 的 ALG_RST 分支一致 */
                s.pout_ave_last = 0u;
                s.vi_ave_last   = 0u;
                s.po_alg_dirt   = 0u;
                s.vout_ini_flag = 0u;

                s.set_chg_vmax  = 0u;
                s.set_chg_vmin  = 0u;
                s.set_chg_imax  = 0u;
                s.set_lppt_vmax = 0u;
                s.set_lppt_vmin = 0u;
                s.set_lppt_pout = 0u;
                s.set_iniprot_vol = 0u;
                s.set_iniprot_cur = 0u;

                s.vi_ave = 0u;
                s.ii_ave = 0u;
                s.vo_ave = 0u;
                s.io_ave = 0u;
                s.pout_ave = 0u;

                s.set_voltage_MAX = 0u;
                s.set_voltage_MIN = 0u;
                s.delta_voltage_step = 0u;
                s.set_voltage_ref = 0u;
                s.po_alg_done = 0u;

                s.s_state_alg = PO_STATE_IDLE;
                break;

            default:
                s.s_state_alg = PO_STATE_ALG_RST;
                break;
        }

        /* 刷新输出以反映本次状态变化后的寄存器值 */
        po_output.o_voltage_ref = s.set_voltage_ref;
        if (!in.i_po_alg_start)
            po_output.o_voltage_ref = in.i_set_pi_vref;
        po_output.o_po_alg_done = s.po_alg_done;

        /* 若回到 IDLE，则可结束一次 step */
        if (s.s_state_alg == PO_STATE_IDLE)
        {
            break;
        }
    }
    //*s_t = s;
}

void po_input_set(po_inputs_t *input_data)
{
    po_inputs_t input;
    input.i_po_alg_en = 1;        //开启po算法使能
    //input.i_po_alg_start = 1;     //po算法开始运行使能
    input.i_po_alg_mode = 0;      // 0: MPPT, 1: LPPT
    input.i_po_alg_step = 5;   //步长设置

    input.i_po_mppt_mode = 0;     // 0: ALL, 1: LEFT
    input.i_po_lppt_mode = 0;     // 0: ALL, 1: LEFT

    input.i_pwm_vout_max = 600 * (0.95);
    input.i_pwm_vout_min = 600 * (1 - 0.95);

    input.i_set_chg_vmax = 46.0f;
    input.i_set_chg_vmin = 5.0f;

    input.i_set_chg_imax = 2.0f;
    input.i_set_lppt_vmax = 80.0f;
    input.i_set_lppt_vmin = 0.0f;
    //input.i_set_lppt_pout
    input.i_set_iniprot_vol = 80.0f;
    input.i_set_iniprot_cur = 5.0f;
    input.i_set_pi_vref = 540;//45.0f * 0.85f;
    *input_data = input;
    po_output.o_voltage_ref = input.i_set_pi_vref;
}






