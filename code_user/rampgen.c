/*
 * rampgen.c
 *
 *  Created on: 2025Äê9ÔÂ17ÈÕ
 *      Author: pc
 */


#include <IQmathLib.h> // Include header for IQmath library
// Don¡¯t forget to set a proper GLOBAL_Q in ¡°IQmathLib.h¡± file
#include "rampgen.h"

void rampgen_calc(RAMPGEN *v)
{

    // Compute the angle rate
    v->Angle += _IQmpy(v->StepAngleMax,v->Freq);

    // Saturate the angle rate within (-1,1)
    if (v->Angle>_IQ(1.0))
    v->Angle -= _IQ(1.0);
    else if (v->Angle<_IQ(-1.0))
    v->Angle += _IQ(1.0);

    // Compute the ramp output
    v->Out = _IQmpy(v->Angle,v->Gain) + v->Offset;

    // Saturate the ramp output within (-1,1)
    if (v->Out>_IQ(1.0))
    v->Out -= _IQ(1.0);
    else if (v->Out<_IQ(-1.0))
    v->Out += _IQ(1.0);
}

#define SIGNAL_LENGTH 512
RAMPGEN rgen = RAMPGEN_DEFAULTS;

void use_rampen(void)
{


    /* Create an instance of Signal generator module    */


    int ipcb[SIGNAL_LENGTH];
    int i;

    _iq x1;
    rgen.Offset=0;
    rgen.Gain=0x7fff;        /* gain=1 in Q15                              */
    rgen.Freq=5369;          /* freq = (Required Freq/Max Freq)*2^15       */
                    /*      = (50/305.17)*2^15 = 5369             */
    rgen.StepAngleMax=1000;      /* Max Freq= (step_max * sampling freq)/65536 */
                    /* Max Freq = (1000*20k)/65536 = 305.17       */
    rgen.Angle=8192;    /* phase_norm =(pi/4/(2*pi))*2^16=8192   */

    for(i=0;i<SIGNAL_LENGTH;i++)
    {
        ipcb[i]=0;
    }

    /*---------------------------------------------------------------------------
    Nothing running in the background at present
    ----------------------------------------------------------------------------*/

    for(i=0;i<SIGNAL_LENGTH;i++)
    {
        rgen.calc(&rgen);
        x1 = rgen.Out;
        ipcb[i]=x1;
    }


}




