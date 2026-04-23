//#############################################################################
//
//  FILE:   mppt_pno.h
//
//  TITLE:  MPPT PNO Module
//
//#############################################################################
// $TI Release: $
// $Release Date: $
// $Copyright: $
//#############################################################################

#ifndef MPPT_PNO_F_H_
#define MPPT_PNO_F_H_

#ifdef __cplusplus
extern "C" {
#endif

//
// Included Files
//
#include <stdint.h>
#ifndef __TMS320C28XX_CLA__
#include <math.h>
#else
#include <CLAmath.h>
#endif

//#############################################################################
//
// Macro Definitions
//
//#############################################################################
#ifndef C2000_IEEE754_TYPES
#define C2000_IEEE754_TYPES
#ifdef __TI_EABI__
typedef float         float32_t;
typedef double        float64_t;
#else // TI COFF
typedef float         float32_t;
typedef long double   float64_t;
#endif // __TI_EABI__
#endif // C2000_IEEE754_TYPES

//
// Typedefs
//
typedef struct {
    float  Ipv;
    float  Vpv;
    float  Vin;
    float  DeltaPmin;
    float  MaxVolt;
    float  MinVolt;
    float  Stepsize;
    float  VmppOut;
    float  Vref;
    float  DeltaP;
    float  PanelPower;
    float  PanelPower_Prev;
    int mppt_enable;
    int mppt_first;
    int mppt_comp;
} MPPT_PNO_F;

typedef MPPT_PNO_F *mppt_pno_handle;

//
// Function Prototypes
//
void MPPT_PNO_F_init(MPPT_PNO_F *v);
void MPPT_PNO_F_FUNC(MPPT_PNO_F *v);
void MPPT_PNO_F_MACRO(MPPT_PNO_F *v);
void MPPT_PNO_F_MACRO_DC(MPPT_PNO_F *pno_f);

//*********** Macro Definition ***********//
//#define MPPT_PNO_F_MACRO(v)                                             \
//if (v.mppt_enable==1)                                                   \
//{                                                                       \
//    if (v.mppt_first == 1)                                              \
//    {                                                                   \
//        v.VmppOut = v.Vref;                                             \
//        v.mppt_first = 0;                                               \
//        v.PanelPower_Prev = v.PanelPower;                               \
//    }                                                                   \
//    else                                                                \
//    {                                                                   \
//        v.PanelPower = (v.Vpv * v.Ipv);                                 \
//        v.DeltaP = v.PanelPower - v.PanelPower_Prev;                    \
//        if (v.DeltaP > v.DeltaPmin)                                     \
//        {                                                               \
//            v.VmppOut = v.Vpv + v.Stepsize;                             \
//        }                                                               \
//        else                                                            \
//        {                                                               \
//            if (v.DeltaP < -v.DeltaPmin)                                \
//            {                                                           \
//                v.Stepsize = -v.Stepsize;                               \
//                v.VmppOut = v.Vpv+v.Stepsize;                           \
//            }                                                           \
//        }                                                               \
//        v.PanelPower_Prev = v.PanelPower;                               \
//    }                                                                   \
//    if(v.VmppOut < v.MinVolt) v.VmppOut = v.MinVolt;                    \
//    if(v.VmppOut > v.MaxVolt) v.VmppOut = v.MaxVolt;                    \
//}

#ifdef __cplusplus
}
#endif // extern "C"

#endif // end of  _MPPT_PNO_F_H_ definition

//
// End of File
//
