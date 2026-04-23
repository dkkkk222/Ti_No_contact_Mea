/*
 * mppt_Init.h
 *
 *  Created on: 2025Äê9ÔÂ24ÈÕ
 *      Author: pc
 */

//#############################################################################
//
// FILE:        MPPT_DCDCerter_hal.h
//
// TITLE:       solution hardware abstraction layer header file
//              This file consists of common variables and functions
//              for a particular hardware board, like functions to read current
//              and voltage signals on the board and functions to setup the
//              basic peripherals of the board
//              This file must be settings independent, all settings dependent
//              code should reside in the parent solution project.
//
//#############################################################################
// $TI Release: TIDM_SOLAR_DCDC v1.00.00.00 $
// $Release Date: Tue Aug 26 14:08:12 CDT 2025 $
// $Copyright:
// Copyright (C) 2021 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

#ifndef MPPT_DCDC_HAL_H
#define MPPT_DCDC_HAL_H

#ifdef __cplusplus

extern "C" {
#endif


//
//******************** the includes *******************************************
//

//
// Include files for device support
//
#include "driverlib.h"
#include "device.h"
#include <mppt_setting.h>

//
// Defines
//
#define MPPT_DCDC_HAL_VOUT_FB    ADC_readResult(MPPT_DCDC_VOUT_ADCRESULT_BASE, \
                                    MPPT_DCDC_VOUT_ADC_SOC_NO)

#define MPPT_DCDC_HAL_IL_FB    ADC_readResult(MPPT_DCDC_IL_ADCRESULT_BASE,\
                                    MPPT_DCDC_IL_ADC_SOC_NO)

#define MPPT_DCDC_VPANEL_FB       ADC_readResult(MPPT_DCDC_VPANEL_ADCRESULT_BASE,  \
                                    MPPT_DCDC_VPANEL_ADC_SOC_NO)

#define MPPT_DCDC_VBUS_FB       ADC_readResult(MPPT_DCDC_VBUS_ADCRESULT_BASE,  \
                                    MPPT_DCDC_VBUS_ADC_SOC_NO)

#define MPPTDCDC_HAL_GET_TASK_A_TIMER_OVERFLOW_STATUS                           \
                                CPUTimer_getTimerOverflowStatus(CPUTIMER0_BASE)
#define MPPTDCDC_HAL_CLEAR_TASK_A_TIMER_OVERFLOW_FLAG                           \
                                     CPUTimer_clearOverflowFlag(CPUTIMER0_BASE)

#define MPPTDCDC_HAL_GET_TASK_B_TIMER_OVERFLOW_STATUS                           \
                                CPUTimer_getTimerOverflowStatus(CPUTIMER1_BASE)
#define MPPTDCDC_HAL_CLEAR_TASK_B_TIMER_OVERFLOW_FLAG                           \
                                     CPUTimer_clearOverflowFlag(CPUTIMER1_BASE)


//
// the function prototypes
//
void MPPTDCDC_HAL_setupDevice(void);
void MPPTDCDC_HAL_setupADC(void);
void MPPTDCDC_HAL_forceAllSOCs(void);
bool MPPTDCDC_HAL_adcConversionComplete(void);
void MPPTDCDC_HAL_setupEPWMtoTriggerADCSOC(uint32_t);
void MPPTDCDC_HAL_configureECAPforSDFMClk(uint16_t);

void MPPTDCDC_HAL_disablePWMCLKCounting(void);
void MPPTDCDC_HAL_enablePWMCLKCounting(void);

void MPPTDCDC_HAL_setupPWM(void);
void MPPTDCDC_HAL_setupLLCPWM(void);
void MPPTDCDC_HAL_setupCMPSS(uint32_t, float32_t, float32_t);
void MPPT_DCDC_HAL_setupPWMforTrip(uint32_t);
void MPPTDCDC_HAL_setupBoardProtection(void);

void MPPTDCDC_HAL_setupDebugGPIO(void);
void MPPTDCDC_HAL_setupHBGPIO(void);
void MPPTDCDC_HAL_setupBoardGPIOs(void);
void MPPTDCDC_HAL_setupInterrupt(void);
void MPPTDCDC_HAL_setupPWMInterruptGeneration(uint32_t, uint16_t);

//
// ISR related
// Configure ISR1 as High Priority Interrupt and assign to ramfuncs
//
#if MPPT_DCDC_ISR1_RUNNING_ON == C28x_CORE
#ifndef __TMS320C28XX_CLA__

    #pragma CODE_SECTION(ISR1,".TI.ramfunc");
    #pragma INTERRUPT (ISR1, HPI)
    interrupt void ISR1(void);

    static inline void MPPTDCDC_HAL_clearISR1InterruptFlag(void);
#endif
#endif

#ifndef __TMS320C28XX_CLA__
    #pragma CODE_SECTION(ISR2,"ramfuncs");
    interrupt void ISR2(void);
#endif


#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_get_iL_overCurrentFlag)
static inline bool MPPTDCDC_HAL_get_iL_overCurrentFlag(void)
{
    return (XBAR_getInputFlagStatus(MPPT_DCDC_IL_XBAR_FLAG2));//Only HIGH Comparator is used. No need of low comparator
}


#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_setDebugGPIO)
static inline void MPPTDCDC_HAL_setDebugGPIO(void)
{
    GPIO_writePin(MPPT_DCDC_DEBUG_GPIO, 1);
}

#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_clearDebugGPIO)
static inline void MPPTDCDC_HAL_clearDebugGPIO(void)
{
    GPIO_writePin(MPPT_DCDC_DEBUG_GPIO, 0);
}

#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_toggleHBGPIO)
static inline void MPPTDCDC_HAL_toggleHBGPIO(void)
{
    GPIO_togglePin(MPPT_DCDC_HB_GPIO);
}

#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_AQ_SW_PWMXA_HIGH)
static inline void MPPTDCDC_HAL_AQ_SW_PWMXA_HIGH(uint32_t base)
{
     EPWM_setActionQualifierContSWForceAction(base, EPWM_AQ_OUTPUT_A,
                                              EPWM_AQ_SW_OUTPUT_HIGH);
}

#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_AQ_SW_PWMXA_LOW)
static inline void MPPTDCDC_HAL_AQ_SW_PWMXA_LOW(uint32_t base)
{
     EPWM_setActionQualifierContSWForceAction(base, EPWM_AQ_OUTPUT_A,
                                              EPWM_AQ_SW_OUTPUT_LOW);
}

#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_updateCMPA)
static inline void MPPTDCDC_HAL_updateCMPA(uint32_t base, float32_t duty)
{
    EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A,
                                (uint16_t)((1.0f-fabsf(duty)) *
                                (MPPT_DCDC_PWM_PERIOD >> 1U)));
}

#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_updateTBPRD)
static inline void MPPTDCDC_HAL_updateTBPRD(uint32_t base, uint16_t periodCount)
{
    /*EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A,
                                (uint16_t)((1.0f-fabsf(duty)) *
                                (MPPT_DCDC_PWM_PERIOD >> 1U)));*/
    EPWM_setTimeBasePeriod(base, periodCount);

}

#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_updatePWMDeadBand)
static inline void MPPTDCDC_HAL_updatePWMDeadBand(uint32_t db)
{
    EPWM_setRisingEdgeDelayCount(MPPT_DCDC_HIGH_FREQ_PWM_A_BASE, db);
    EPWM_setFallingEdgeDelayCount(MPPT_DCDC_HIGH_FREQ_PWM_A_BASE, db);

    EPWM_setRisingEdgeDelayCount(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE, db);
    EPWM_setFallingEdgeDelayCount(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE, db);
}

#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_forceOSTPWMs)
static inline void MPPTDCDC_HAL_forceOSTPWMs()
{
    //
    // Forcing OSTs for all operating PWM modules at initialization
    //
    EPWM_forceTripZoneEvent(MPPT_DCDC_HIGH_FREQ_PWM_A_BASE,
                            EPWM_TZ_FORCE_EVENT_OST);
    EPWM_forceTripZoneEvent(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE,
                            EPWM_TZ_FORCE_EVENT_OST);
    EPWM_forceTripZoneEvent(MPPT_DCDC_LLC_PWM_BASE,
                            EPWM_TZ_FORCE_EVENT_OST);
}

#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_clearPWMTripFlags)
static inline void MPPTDCDC_HAL_clearPWMTripFlags(uint32_t base)
{
    //
    // Clear all trip flags
    //
    EPWM_clearTripZoneFlag(base, (EPWM_TZ_INTERRUPT_OST |
                                  EPWM_TZ_INTERRUPT_CBC |
                                  EPWM_TZ_INTERRUPT_DCAEVT1));

    //
    // Clear all source flags for the One-shot trip zone
    //
    EPWM_clearOneShotTripZoneFlag(base, (EPWM_TZ_OST_FLAG_OST1 |
                                         EPWM_TZ_OST_FLAG_OST2 |
                                         EPWM_TZ_OST_FLAG_OST3 |
                                         EPWM_TZ_OST_FLAG_DCAEVT1));
}

//
// Clear XBAR fault flags
//
#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_clearAllFaultFlags)
static inline void MPPTDCDC_HAL_clearAllFaultFlags(void)
{
    XBAR_clearInputFlag(MPPT_DCDC_IL_XBAR_FLAG1);
    XBAR_clearInputFlag(MPPT_DCDC_IL_XBAR_FLAG2);
}



//
// Clear EPWM Interrupt Flag
//
#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_clearPWMInterruptFlag)
static inline void MPPTDCDC_HAL_clearPWMInterruptFlag(uint32_t base)
{
    EPWM_clearEventTriggerInterruptFlag(base);
}

#ifndef __TMS320C28XX_CLA__
//
// Clear ISR1 Acknowledgment flag
//
#pragma FUNC_ALWAYS_INLINE(MPPTDCDC_HAL_clearISR1InterruptFlag)
static inline void MPPTDCDC_HAL_clearISR1InterruptFlag(void)
{
    Interrupt_clearACKGroup(MPPT_DCDC_ISR1_PIE_GROUP_NO);

}


#endif



#endif /* CODE_USER_MPPT_INIT_H_ */
