/*
 * po_algorithm.h
 *
 *  Created on: 2025年9月28日
 *      Author: pc
 */

#ifndef CODE_USER_PO_ALGORITHM_H_
#define CODE_USER_PO_ALGORITHM_H_



/* 输入：对应 VHDL 端口 */
typedef struct
{
    uint16_t i_po_alg_en; // 0/1 /
    uint16_t i_po_alg_start; // 0/1, 建议50ms脉冲1次 /
    uint16_t i_po_alg_mode; // 0: MPPT, 1: LPPT /
    uint16_t i_po_alg_step; /* 步进 */

    uint16_t  i_po_mppt_mode;    /* 0: ALL, 1: LEFT */
    uint16_t  i_po_lppt_mode;    /* 0: ALL, 1: LEFT */

    uint16_t i_pwm_vout_max;
    uint16_t i_pwm_vout_min;

    float i_set_chg_vmax;    /* 充电上限电压 */
    float i_set_chg_vmin;    /* 充电下限电压 */
    float i_set_chg_imax;    /* 充电电流上限 */
    float i_set_lppt_vmax;   /* LPPT 输入电压上限 */
    float i_set_lppt_vmin;   /* LPPT 输入电压下限 */
    float i_set_lppt_pout;   /* LPPT 目标功率 */
    float i_set_iniprot_vol; /* 启动输入保护电压阈值 */
    float i_set_iniprot_cur; /* 启动输入保护电流阈值 */
    uint16_t i_set_pi_vref;     /* 初始 PI 电压参考 */

    float i_vi_ave;          /* 输入电压平均值 */
    float i_ii_ave;          /* 输入电流平均值 */
    float i_vo_ave;          /* 输出电压平均值 */
    float i_io_ave;          /* 输出电流平均值 */
} po_inputs_t;

//* 输出：对应 VHDL 端口 /
typedef struct
{
    uint16_t o_voltage_ref;
    uint16_t o_po_alg_done;
} po_outputs_t;


typedef enum
{
    PO_STATE_IDLE = 0,
    PO_STATE_ADJ_INI,
    PO_STATE_VIN_VOUT_ADJ,
    PO_STATE_MPPT_ADJ_MODE,
    PO_STATE_LPPT_ADJ_MODE,
    PO_STATE_DC_ADJUST,
    PO_STATE_ADJUST_DONE,
    PO_STATE_ALG_RST
} po_state_t;

    //* 状态保持：对应 VHDL 内部寄存器 /
typedef struct
{
   // 参数寄存与采样值寄存 */
    float set_chg_vmax;
    float set_chg_vmin;
    float set_chg_imax;
    float set_lppt_vmax;
    float set_lppt_vmin;
    float set_lppt_pout;
    float set_iniprot_vol;
    float set_iniprot_cur;

    float vi_ave;
    float vo_ave;
    float ii_ave;
    float io_ave;

    float pout_ave;
    float pout_ave_last;
    float vi_ave_last;

    uint16_t set_voltage_ref;
    uint16_t set_voltage_MAX;
    uint16_t set_voltage_MIN;

    uint16_t delta_voltage_step;
    uint16_t set_po_step;

    uint16_t  po_alg_dirt;    /* 0: 向下调, 1: 向上调 */
    uint16_t  vout_ini_flag;  /* 启动允许标志 */
    uint16_t  po_alg_done;

    po_state_t s_state_alg;
} po_algorithm_t;

extern po_algorithm_t  s;
extern po_inputs_t     po_input;
extern po_outputs_t    po_output;
void po_input_set(po_inputs_t *input_data);
void po_algorithm_step(const po_inputs_t in);
void po_algorithm_init(po_algorithm_t s);

#endif /* CODE_USER_PO_ALGORITHM_H_ */
