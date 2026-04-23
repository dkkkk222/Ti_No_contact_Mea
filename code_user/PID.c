/*
 * PID_Control.c
 *
 *  Created on: 2025Äê9ÔÂ16ÈÕ
 *      Author: pc
 */
#include "PID.h"


uint32_t old_result= 0;
uint32_t old_result_v = 0;
//
//typedef enum { false = 0, true = 1 } BOOL;
extern MPPT_PNO_F mppt_pno1;
uint16_t mppt_Control_flag = 0;
uint16_t mppt_flag_cnt = 0;
uint8_t Power_cal = 0;
float Power_add = 0;
float Power_filt = 0;
uint16_t Power_cal_cnt = 0;

float e_k = 0;
float e_k_i_Volt = 0;
uint32_t e_k_i_Cur = 0;

float KP_Vol_IQ = (KP_Volt * 65536.0f);
float KI_Vol_IQ = (KI_Volt * 65536.0f);

uint32_t KP_Cur_IQ = (KP_Cur * 4194304);
uint32_t KI_Cur_IQ = (KI_Cur * 4194304);

bool windup_1 = true;
bool windup_2 = true;
float ADC_Volt_out = 0;
float ADC_Volt_in = 0;
float ADC_Cur_out = 0;
float ADC_Cur = 0;
uint16_t Prd_limmit_up = Limmit_UP * Prd_val;
uint16_t Prd_limmit_down = (1 - Limmit_UP) * Prd_val;

uint32_t regulationPID_Vol(void)
{
    float VDC_Sec_meas_AVG = ADC_Volt_out * 1000.0f;
    float diff_Volt = 0;
    float result_v = 0;
    //float prevPS = 0;
    //float Spgfaktor = 1.0f * VDC_Pri_meas_AVG; //Hier wird die Sollspannung eingestellt
    float Spgfaktor = 20000.0f;

    // Differenzspannung von Soll- und Istwert
    diff_Volt = Spgfaktor - VDC_Sec_meas_AVG;

    //diff_Volt = Spgfaktor;


    //  if (diff_Volt < 0.2 && diff_Volt > -0.2 ) diff_Volt=0;
    e_k = diff_Volt;
    if (windup_1 == true)
    {
        e_k_i_Volt += e_k;
    }
    //e_k_d     = e_k - e_k_1;
    result_v = KP_Vol_IQ * e_k + KI_Vol_IQ * e_k_i_Volt;

    //result_v /= 1000;
    result_v = (result_v / 65536.0f);

//  result_v += old_result_v;
    //e_k_1     = e_k;

    //res_v     = result_v;

    // 20000
    if(result_v > Prd_limmit_up)
    {
        windup_1 = false;
        result_v = Prd_limmit_up;
    }
    if(result_v < Prd_limmit_down)
    {
        //windup_1 = false;
        windup_1 = true;
        result_v = Prd_limmit_down;
    }

    if((result_v < Prd_limmit_up) && (result_v > Prd_limmit_down))
    {
        windup_1 = true;
    }

    return (Prd_val - (uint32_t)result_v);

}



uint32_t regulationPID_Vol_mppt(void)
{
    float VDC_Sec_meas_AVG = ADC_Volt_in * 1000.0f;
    static float Vmppt_old = 0;
    float diff_Volt = 0;
    float result_v = 0;
    //float prevPS = 0;
    //float Spgfaktor = 1.0f * VDC_Pri_meas_AVG; //Hier wird die Sollspannung eingestellt
    float Spgfaktor = po_output.o_voltage_ref * 1000.0f;
//    if(Vmppt_old != mppt_pno1.VmppOut)
//    {
//        Power_cal = 0;
//        mppt_Control_flag = 1;
//        Vmppt_old = mppt_pno1.VmppOut;
//    }
//    if(Vmppt_old != mppt_pno1.mppt_comp)
//    {
//        Power_cal = 0;
//        mppt_Control_flag = 1;
//        Vmppt_old = mppt_pno1.mppt_comp;
//    }
//    if(mppt_Control_flag == 1)
//    {
//        mppt_flag_cnt++;
//        if(mppt_flag_cnt >= 4700)
//        {
//            mppt_flag_cnt = 0;
//            Power_cal = 1;
//        }
//    }
//    if(Power_cal == 1)
//    {
//        Power_add += (ADC_Cur * ADC_Volt_in);
//        Power_cal_cnt++;
//    }
//    if(Power_cal_cnt >= 50)
//    {
//        Power_filt = Power_add / 50.0f;
//        Power_cal = 0;
//        mppt_Control_flag = 0;
//        Power_cal_cnt = 0;
//        Power_add = 0;
//    }

//    // Differenzspannung von Soll- und Istwert
    diff_Volt = VDC_Sec_meas_AVG - Spgfaktor;

    //diff_Volt = Spgfaktor;


    //  if (diff_Volt < 0.2 && diff_Volt > -0.2 ) diff_Volt=0;
    e_k = diff_Volt;
    if (windup_1 == true)
    {
        e_k_i_Volt += e_k;
    }
    //e_k_d     = e_k - e_k_1;
    result_v = KP_Vol_IQ * e_k + KI_Vol_IQ * e_k_i_Volt;

    //result_v /= 1000;
    result_v = (result_v / 65536.0f);

//  result_v += old_result_v;
    //e_k_1     = e_k;

    //res_v     = result_v;

    // 20000
    if(result_v > Prd_limmit_up)
    {
        windup_1 = false;
        result_v = Prd_limmit_up;
    }
    if(result_v < Prd_limmit_down)
    {
        //windup_1 = false;
        windup_1 = true;
        result_v = Prd_limmit_down;
    }

    if((result_v < Prd_limmit_up) && (result_v > Prd_limmit_down))
    {
        windup_1 = true;
    }

    return (Prd_val - (uint32_t)result_v);

}






