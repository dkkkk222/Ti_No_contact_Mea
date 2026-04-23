//#############################################################################
//
// FILE:   mppt_dcdc_settings.h
//
// TITLE:  mppt_dcdc_settings.h
//
//#############################################################################
// $TI Release: TIDM_SOLAR_DCDC v1.00.00.00 $
// $Release Date: Tue Aug 26 14:08:12 CDT 2025 $
// Copyright (C) {2021} Texas Instruments Incorporated - http://www.ti.com/
// * ALL RIGHTS RESERVED*
//#############################################################################

#ifndef _PROJSETTINGS_H
#define _PROJSETTINGS_H

#ifdef __cplusplus

extern "C" {
#endif



//*****************************************************************************
//defines
//*****************************************************************************


//
// Incremental lab defines to test the solution (Lab1, Lab2, Lab3 only)
//
#define MPPT_DCDC_LAB                2U

//
//
// Hardware Settings
//
#define MPPT_DCDC_VOUT_MAX_SENSE      ((float32_t)412.5)

#define MPPT_DCDC_VDCBUS_MAX_SENSE   ((float32_t)510.99)//Vbus*(13k/(2000K+13K))=3.3V => Vbus max = 510.99V

#define MPPT_DCDC_VPANEL_MAX_SENSE   ((float32_t)534.54)//Vpanel max =534.54V

#define MPPT_DCDC_IINV_MAX_SENSE       ((float32_t)30.18) // Max Instantaneous value
#define MPPT_DCDC_IL_MAX_SENSE       ((float32_t)8.84) // Max Instantaneous value IL*0.01*(39.2K/1.05K)=3.3V =>IL max = 8.84A
//Adjustment of gain = 95%, 8.84*0.95 = 8.403

#define Low_Pass_Filter   ((float32_t)0.9994)


//
// Diagnostic Related
//
//#define MPPT_DCDC_DC_MIN             (float32_t)200.0
#define MPPT_DCDC_VBUS_MAX             (float32_t)415 // Over-voltage threshold for Vbus
#define MPPT_DCDC_VPANEL_MAX             (float32_t)235// Over-voltage threshold for Vpanel
//
//
#define MPPTDCDC_DLOG_SIZE      100
#define MPPTDCDC_DLOG_TRIGGER   ((float32_t)0.01)
#define MPPTDCDC_DLOG_SCALE     25
//

//
// Options to select C28x/CLA
//
#define C28x_CORE                         1U
#define CLA_CORE                          2U
//
//
// SFRA Options
// 0 -> disabled
// 1 -> Current
// 2 -> Voltage
//
#define MPPT_DCDC_SFRA_DISABLED 0
#define MPPT_DCDC_SFRA_CURRENT 1
#define MPPT_DCDC_SFRA_VOLTAGE 2
//
// SFRA injection amplitude, use higher injection in open loop  because plant
// response is low
//
#define MPPT_DCDC_SFRA_INJECTION_AMPLITUDE_LEVEL1 0.001f
#define MPPT_DCDC_SFRA_INJECTION_AMPLITUDE_LEVEL2 0.01f
#define MPPT_DCDC_SFRA_INJECTION_AMPLITUDE_LEVEL3 0.015f
//
//
#define MPPT_DCDC_SFRA_AMPLITUDE MPPT_DCDC_SFRA_INJECTION_AMPLITUDE_LEVEL3
//
//
#define MPPT_DCDC_SFRA_TYPE    MPPT_DCDC_SFRA_DISABLED
//#define MPPT_DCDC_SFRA_TYPE  MPPT_DCDC_SFRA_VOLTAGE
//#define MPPT_DCDC_SFRA_TYPE  MPPT_DCDC_SFRA_CURRENT
//
//
// CPU Timers related values
//
#define MPPT_DCDC_CPUTIMER0_FREQUENCY  100U   // 10ms, 100Hz
#define MPPT_DCDC_CPUTIMER1_FREQUENCY  10U    // 100ms, 10Hz
#define MPPT_DCDC_CPUTIMER2_FREQUENCY  10000U  // 10kHz ISR2

#define MPPT_DCDC_CPUTIMER0_PERIOD  (DEVICE_SYSCLK_FREQ /                       \
                                     MPPT_DCDC_CPUTIMER0_FREQUENCY)
#define MPPT_DCDC_CPUTIMER1_PERIOD  (DEVICE_SYSCLK_FREQ /                       \
                                     MPPT_DCDC_CPUTIMER1_FREQUENCY)
#define MPPT_DCDC_CPUTIMER2_PERIOD  (DEVICE_SYSCLK_FREQ /                       \
                                     MPPT_DCDC_CPUTIMER2_FREQUENCY)

//MPPT Related. MPPT INCC algo
//
#define MPPT_DCDC_INCC_MAX_V        0.9f;
#define MPPT_DCDC_INCC_MIN_V        0.0f;
#define MPPT_DCDC_INCC_STEPSIZE     0.001f;//0.001f;

//
// Control Related
//
#define MPPT_DCDC_IREF_DEFAULT        0.05f


//
// Current PI Controller Settings from Compensation Designer
//
#define MPPT_DCDC_GI_PI_KP ((float32_t)0.1)  //Updated values
#define MPPT_DCDC_GI_PI_KI ((float32_t)0.005) // Updated Values


#define MPPT_DCDC_GI_PI_MAX ((float32_t)0.5)  // Updated Values
#define MPPT_DCDC_GI_PI_MIN ((float32_t)0.0)  // Updated Values

//voltage control loop
#define MPPT_DCDC_GV_PI_KP ((float32_t)0.5)//0.25)//Updated Values
#define MPPT_DCDC_GV_PI_KI ((float32_t)0.0005)//Updated Values

#define MPPT_DCDC_GV_PI_MAX ((float32_t)0.255)//Limiting Ipv current to 0.255*8.84A
#define MPPT_DCDC_GV_PI_MIN ((float32_t)0.0)  //
//voltage control loop

//
//
// Board Protection related defines
//
#define MPPT_DCDC_CMPSS_PROTECTION_ENABLED      1U// Over-current protection

#define MPPT_DCDC_IL_TRIP_LIMIT_AMPS          5U//

#define MPPT_DCDC_OVERVOLTAGE_DC_PROTECTION_ENABLED  1U

#define MPPT_DCDC_IL_CMPSS_BASE             CMPSS1_BASE

#define MPPT_DCDC_IL_XBAR_MUX               XBAR_MUX00

#define MPPT_DCDC_IL_XBAR_MUX_VAL          XBAR_EPWM_MUX00_CMPSS1_CTRIPH
#define MPPT_DCDC_IL_XBAR_FLAG1             XBAR_INPUT_FLG_CMPSS1_CTRIPL//Not using Low Comparator for PV current protection because of DC current
#define MPPT_DCDC_IL_XBAR_FLAG2             XBAR_INPUT_FLG_CMPSS1_CTRIPH

//

#define MPPT_DCDC_DEBUG_GPIO                    39
#define MPPT_DCDC_DEBUG_GPIO_PIN_CONFIG         GPIO_39_GPIO39

//Changed to GPIO34 to toggle LED D3 on F280049 CC
//#define MPPT_DCDC_HB_GPIO                       28
//#define MPPT_DCDC_HB_GPIO_PIN_CONFIG            GPIO_28_GPIO28
#define MPPT_DCDC_HB_GPIO                       34
#define MPPT_DCDC_HB_GPIO_PIN_CONFIG            GPIO_34_GPIO34

//
// PWM related defines
//

#define MPPT_DCDC_PWM_SWITCHING_FREQUENCY  500U//100000U  // 100 KHz PWM Frequency
#define MPPT_DCDC_PWM_PERIOD               (DEVICE_SYSCLK_FREQ /                \
                                          MPPT_DCDC_PWM_SWITCHING_FREQUENCY)
#define MPPT_DCDC_HIGH_FREQ_PWM_DB_TICKS   15U
#define MPPT_DCDC_LOW_FREQ_PWM_DB_TICKS    20U
#define MPPT_DCDC_EXTEND_ON_TICKS          20U

#define MPPT_DCDC_DUTY_MAX                 0.92f
#define MPPT_DCDC_DUTY_MIN                 0.0f

#define MPPT_DCDC_HIGH_FREQ_PWM_A_BASE     EPWM1_BASE
#define MPPT_DCDC_HIGH_FREQ_PWM_B_BASE     EPWM2_BASE


#define MPPT_DCDC_LLC_PWM_FREQ_DEFAULT  50//120000U  // 120 KHz PWM Frequency
#define MPPT_DCDC_LLC_PWM_PRD_DEFAULT               (DEVICE_SYSCLK_FREQ /                \
                                          MPPT_DCDC_LLC_PWM_FREQ_DEFAULT)

#define MPPT_DCDC_LLC_PWM_BASE     EPWM3_BASE

#define MPPT_DCDC_LLC_PWM_FREQ_MAX  200000U  // 200 KHz Max PWM Frequency
#define MPPT_DCDC_LLC_PWM_FREQ_MIN  100000U  // 100 KHz Min PWM Frequency
#define MPPT_DCDC_LLC_PWM_PRD_MAX               (DEVICE_SYSCLK_FREQ /                \
                                          MPPT_DCDC_LLC_PWM_FREQ_MIN)
#define MPPT_DCDC_LLC_PWM_PRD_MIN               (DEVICE_SYSCLK_FREQ /                \
                                          MPPT_DCDC_LLC_PWM_FREQ_MAX)
//
// GPIO defines
//

#define MPPT_DCDC_HIGH_FREQ_PWM_A_GPIO_POS             0
#define MPPT_DCDC_HIGH_FREQ_PWM_A_GPIO_POS_PIN_CONFIG  GPIO_0_EPWM1_A
#define MPPT_DCDC_HIGH_FREQ_PWM_A_GPIO_NEG             1
#define MPPT_DCDC_HIGH_FREQ_PWM_A_GPIO_NEG_PIN_CONFIG  GPIO_1_EPWM1_B

#define MPPT_DCDC_HIGH_FREQ_PWM_B_GPIO_POS             2
#define MPPT_DCDC_HIGH_FREQ_PWM_B_GPIO_POS_PIN_CONFIG  GPIO_2_EPWM2_A
#define MPPT_DCDC_HIGH_FREQ_PWM_B_GPIO_NEG             3
#define MPPT_DCDC_HIGH_FREQ_PWM_B_GPIO_NEG_PIN_CONFIG  GPIO_3_EPWM2_B
//
// LLC stage GPIO defines
//

#define MPPT_DCDC_LLC_PWM_GPIO_POS             4
#define MPPT_DCDC_LLC_PWM_GPIO_POS_PIN_CONFIG  GPIO_4_EPWM3_A
#define MPPT_DCDC_LLC_PWM_GPIO_NEG             5
#define MPPT_DCDC_LLC_PWM_GPIO_NEG_PIN_CONFIG  GPIO_5_EPWM3_B

//
// Interrupt related defines
//

#define MPPT_DCDC_ISR1_ENABLE              1U
#define MPPT_DCDC_ISR2_ENABLE              1U

#define MPPT_DCDC_ISR1_RUNNING_ON          C28x_CORE
#define MPPT_DCDC_CNTRL_ISR_FREQ_RATIO     2U
#define MPPT_DCDC_ISR1_FREQUENCY_HZ        (MPPT_DCDC_PWM_SWITCHING_FREQUENCY /  \
                                           MPPT_DCDC_CNTRL_ISR_FREQ_RATIO)

#define MPPT_DCDC_ISR1_PIE_GROUP_NO        INTERRUPT_ACK_GROUP3
#define MPPT_DCDC_ISR1_TRIG_BASE           EPWM1_BASE
#define MPPT_DCDC_ISR1_TRIG                INT_EPWM1
#define MPPT_DCDC_ISR1_TRIG_CLA            CLA_TRIGGER_EPWM1INT

#define MPPT_DCDC_ISR2_FREQUENCY_HZ        (float32_t)MPPT_DCDC_CPUTIMER2_FREQUENCY
#define MPPT_DCDC_ISR2_TRIG_BASE           CPUTIMER2_BASE
#define MPPT_DCDC_ISR2_TRIG                INT_TIMER2


//
// SFRA Related
//
#define MPPT_DCDC_SFRA_ISR_FREQ MPPT_DCDC_ISR1_FREQUENCY_HZ

#define MPPT_DCDC_SFRA_GUI_SCI_BASE SCIA_BASE
#define MPPT_DCDC_SCI_VBUS_CLK 50000000  // 50 Mhz
#define MPPT_DCDC_SFRA_GUI_SCI_BAUDRATE 57600

#define MPPT_DCDC_SFRA_GUI_SCIRX_GPIO 28
#define MPPT_DCDC_SFRA_GUI_SCITX_GPIO 29

#define MPPT_DCDC_SFRA_GUI_SCIRX_GPIO_PIN_CONFIG GPIO_28_SCIRXDA
#define MPPT_DCDC_SFRA_GUI_SCITX_GPIO_PIN_CONFIG GPIO_29_SCITXDA

//
// if the following #define is set to 1 SFRA GUI indicates status on an LED
// otherwise LED code is ignored
//
#define MPPT_DCDC_SFRA_GUI_LED_INDICATOR 1//0
#define MPPT_DCDC_SFRA_GUI_LED_GPIO 31
#define MPPT_DCDC_SFRA_GUI_LED_GPIO_PIN_CONFIG GPIO_31_GPIO31

#define MPPT_DCDC_SFRA_GUI_PLOT_OL_PLANT 1
//
//
// System Settings
//
#if MPPT_DCDC_SFRA_TYPE == 0
//
//If SFRA is Disabled
//
#define MPPT_DCDC_SFRA_FREQ_START 0
#define MPPT_DCDC_SFRA_FREQ_LENGTH 0
#define MPPT_DCDC_SFRA_FREQ_STEP_MULTIPLY (float)0.0f
//#define MPPT_DCDC_SFRA_AMPLITUDE (float)0.0f
//
// else if SFRA enabled for current loop
//
#elif MPPT_DCDC_SFRA_TYPE == MPPT_DCDC_SFRA_CURRENT
#define MPPT_DCDC_SFRA_FREQ_START 110
#define MPPT_DCDC_SFRA_FREQ_LENGTH 100
//
// SFRA step Multiply = 10^(1/No of steps per decade(52)
//
#define MPPT_DCDC_SFRA_FREQ_STEP_MULTIPLY (float)1.045f

//
// else if SFRA is enabled for voltage loop
//
#elif MPPT_DCDC_SFRA_TYPE == MPPT_DCDC_SFRA_VOLTAGE
#define MPPT_DCDC_SFRA_FREQ_START 3//1
#define MPPT_DCDC_SFRA_FREQ_LENGTH 30
//
//
// SFRA step Multiply = 10^(1/No of steps per decade(52))
//
#define MPPT_DCDC_SFRA_FREQ_STEP_MULTIPLY (float)1.2f

#endif

//
// ADC Setup related defines
//
#define MPPT_DCDC_ADC_SOC_TRIG             ADC_TRIGGER_EPWM1_SOCA
#define MPPT_DCDC_ACQPS_SYS_CLKS           10U

/*#define MPPT_DCDC_ADC_TRIGGER_CMPB         (uint16_t)((MPPT_DCDC_PWM_PERIOD -    \
                                        (MPPT_DCDC_ACQPS_SYS_CLKS + 21U) * 15U) \
                                             >> 1U)*/
#define MPPT_DCDC_ADC_TRIGGER_CMPB         (uint16_t)((MPPT_DCDC_PWM_PERIOD -    \
                                        (MPPT_DCDC_ACQPS_SYS_CLKS + 21U) * 3U) \
                                             >> 1U)
//Changed 15U to 3U in the above constant #define. Because we need to convert only 4 signals (and not all 16)
//for this MPPT DC_DC

#define MPPT_DCDC_ADC_OFFSET_COUNT         10U
#define MPPT_DCDC_ADC_INTERRUPT_BASE       ADCA_BASE //ADCB_BASE
#define MPPT_DCDC_ADC_INTERRUPT_SOC        ADC_SOC_NUMBER2//ADC_SOC_NUMBER4 //ADC_SOC_NUMBER14

#define MPPT_DCDC_IL_ADC_BASE         ADCA_BASE
#define MPPT_DCDC_IL_ADCRESULT_BASE   ADCARESULT_BASE
#define MPPT_DCDC_IL_ADC_PIN          ADC_CH_ADCIN2 //ADC_CH_ADCIN5
#define MPPT_DCDC_IL_ADC_SOC_NO       ADC_SOC_NUMBER2

#define MPPT_DCDC_VOUT_ADC_BASE          ADCB_BASE
#define MPPT_DCDC_VOUT_ADCRESULT_BASE    ADCBRESULT_BASE
#define MPPT_DCDC_VOUT_ADC_PIN           ADC_CH_ADCIN0
#define MPPT_DCDC_VOUT_ADC_SOC_NO        ADC_SOC_NUMBER12

#define MPPT_DCDC_VPANEL_ADC_BASE            ADCA_BASE
#define MPPT_DCDC_VPANEL_ADCRESULT_BASE      ADCARESULT_BASE
#define MPPT_DCDC_VPANEL_ADC_PIN             ADC_CH_ADCIN0
#define MPPT_DCDC_VPANEL_ADC_SOC_NO          ADC_SOC_NUMBER0

#define MPPT_DCDC_VBUS_ADC_BASE            ADCA_BASE
#define MPPT_DCDC_VBUS_ADCRESULT_BASE      ADCARESULT_BASE
#define MPPT_DCDC_VBUS_ADC_PIN             ADC_CH_ADCIN1
#define MPPT_DCDC_VBUS_ADC_SOC_NO          ADC_SOC_NUMBER1

//
// ADC Reading Default Calibrations
//
#define MPPT_DCDC_ADC_PU_SCALE_FACTOR     0.000244140625f  // (1/2^12)
#define MPPT_DCDC_SD32_PU_SCALE_FACTOR    ((float32_t)(1.0f /                      \
                                       ((float32_t)MPPT_DCDC_SDFM_OSR *             \
                                      MPPT_DCDC_SDFM_OSR * MPPT_DCDC_SDFM_OSR)))

#define MPPT_DCDC_CURRENT_OFFSET          ((float32_t)0.5) // in per unit
#define MPPT_DCDC_CURRENT_SCALE           ((float32_t)1.0)
#define MPPT_DCDC_VOLTAGE_OFFSET          ((float32_t)0.5) // in per unit
#define MPPT_DCDC_VOLTAGE_SCALE           ((float32_t)1.0)
#define MPPT_DCDC_VBUS_SCALE              ((float32_t)1.0)

//=============================================================================
// User code settings file
//=============================================================================
//#include <MPPT_DCDCerter_user_settings.h>

#ifdef __cplusplus
}
#endif                                  /* extern "C" */

#endif //_PROJSETTINGS_H
