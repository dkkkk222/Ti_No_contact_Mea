/*
 * mppt_pno.c
 *
 *  Created on: 2025年9月26日
 *      Author: pc
 */

#include "mppt_pno.h"

extern float Power_filt;

void MPPT_PNO_F_MACRO(MPPT_PNO_F *pno_f)
{
    MPPT_PNO_F v;
    v = *pno_f;
    if (v.mppt_enable==1)
    {
        if (v.mppt_first == 1)
        {
            v.VmppOut = v.Vref;
            v.mppt_first = 0;
            v.PanelPower_Prev = v.PanelPower;
        }
        else
        {
            v.PanelPower = Power_filt;//(v.Vpv * v.Ipv);
            v.DeltaP = v.PanelPower - v.PanelPower_Prev;
            if (v.DeltaP > v.DeltaPmin)
            {
                v.VmppOut = v.Vin + v.Stepsize;
            }
            else
            {
                if (v.DeltaP < -v.DeltaPmin)
                {
                    v.Stepsize = -v.Stepsize;
                    v.VmppOut = v.Vin + v.Stepsize;
                }
            }
            v.PanelPower_Prev = v.PanelPower;
        }
        if(v.VmppOut < v.MinVolt)
            v.VmppOut = v.MinVolt + v.Stepsize;
        if(v.VmppOut > v.MaxVolt)
            v.VmppOut = v.MaxVolt - v.Stepsize;
    }

    if (v.mppt_enable == 0)
    {
        v.VmppOut = v.Vref;
    }
    *pno_f = v;
}


int mppt_DC_step = 5;
//关闭PI的算法
void MPPT_PNO_F_MACRO_DC(MPPT_PNO_F *pno_f)
{
    MPPT_PNO_F v;
    v = *pno_f;
    if (v.mppt_enable==1)
    {
        if (v.mppt_first == 1)
        {
            v.mppt_comp = 240;//10;
            v.VmppOut = v.Vref;
            v.mppt_first = 0;
            v.PanelPower_Prev = v.PanelPower;
        }
        else
        {
            v.PanelPower = Power_filt;//(v.Vpv * v.Ipv);
            v.DeltaP = v.PanelPower - v.PanelPower_Prev;
            //if (v.DeltaP >= v.DeltaPmin)
            if(v.PanelPower >= v.PanelPower_Prev)
            {
//                v.VmppOut = v.Vin + v.Stepsize;
                v.mppt_comp += mppt_DC_step;
            }
            else
            {
                //if (v.DeltaP < -v.DeltaPmin)
                if(v.PanelPower < v.PanelPower_Prev)
                {
                    mppt_DC_step = -mppt_DC_step;
//                    v.VmppOut = v.Vin + v.Stepsize;
                    v.mppt_comp += mppt_DC_step;
                }
            }
            v.PanelPower_Prev = v.PanelPower;
        }
        if(v.mppt_comp < 0)
            v.mppt_comp += mppt_DC_step;
        if(v.mppt_comp > 540)
            v.mppt_comp -= mppt_DC_step;
    }

    if (v.mppt_enable == 0)
    {
        v.mppt_comp = 240;//v.Vref;
    }
    *pno_f = v;
}


