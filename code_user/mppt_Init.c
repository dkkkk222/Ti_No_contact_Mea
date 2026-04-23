/*
 * mppt_Init.c
 *
 *  Created on: 2025Äê9ÔÂ24ÈÕ
 *      Author: pc
 */
//#############################################################################
//
// FILE:     mppt_dcdc_hal.c
//
// TITLE:    solution hardware abstraction layer
//           This file consists of board related initialization
//           this file is used to make the
//           main file more readable
//
//#############################################################################


#include <mppt_Init.h>

//
// This routine sets up the basic device configuration such as
// initializing PLL, copying code from FLASH to RAM,
// this routine will also initialize the CPU timers that are used in
// the background task for this system
//
void MPPTDCDC_HAL_setupDevice(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pull-ups.
    //
    Device_initGPIO();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // initialize CPU timers for BG tasks & ISR2
    //

    //
    // Initialize CPU Timer 0 for 100 Hz
    //
    CPUTimer_setPeriod(CPUTIMER0_BASE, MPPT_DCDC_CPUTIMER0_PERIOD);

    //
    // Initialize CPU Timer 1 for 10 Hz
    //
    CPUTimer_setPeriod(CPUTIMER1_BASE, MPPT_DCDC_CPUTIMER1_PERIOD);

    //
    // Initialize CPU Timer 2 for 10 KHz
    //
    CPUTimer_setPeriod(CPUTIMER2_BASE, MPPT_DCDC_CPUTIMER2_PERIOD);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);

    //
    // Make sure timers are stopped
    //
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_stopTimer(CPUTIMER2_BASE);
    CPUTimer_setEmulationMode(CPUTIMER0_BASE,
                              CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_setEmulationMode(CPUTIMER1_BASE,
                              CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_setEmulationMode(CPUTIMER2_BASE,
                              CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);

    //
    // Reload all counter registers with period value
    //
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    //
    // Resume all the timers
    //
    CPUTimer_resumeTimer(CPUTIMER0_BASE);
    CPUTimer_resumeTimer(CPUTIMER1_BASE);
    CPUTimer_resumeTimer(CPUTIMER2_BASE);
}

//
// Setup the ADCs and SOCs for sampling various parameters
//
void MPPTDCDC_HAL_setupADC(void)
{

    //
    // write configurations for ADC-A
    // set ADCCLK divider to /2
    // Vref as internal
    // set pulse positions to late
    // power up the ADC
    //
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_2_0);
    ADC_setVREF(ADCA_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCA_BASE);

    //
    // write configurations for ADC-B
    // set ADCCLK divider to /2
    // Vref as internal
    // set pulse positions to late
    // power up the ADC
    //
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_2_0);
    ADC_setVREF(ADCB_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
    ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCB_BASE);

    //
    // write configurations for ADC-C
    // set ADCCLK divider to /2
    // Vref as internal
    // set pulse positions to late
    // power up the ADC
    //
    ADC_setPrescaler(ADCC_BASE, ADC_CLK_DIV_2_0);
    ADC_setVREF(ADCC_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
    ADC_setInterruptPulseMode(ADCC_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCC_BASE);

    //
    // delay for 1ms to allow ADCs to power up
    //
    DEVICE_DELAY_US(1000);



    //
    // Setting up the SOCs for current sampling
    //

    ADC_setupSOC(MPPT_DCDC_IL_ADC_BASE,
                 MPPT_DCDC_IL_ADC_SOC_NO,
                 MPPT_DCDC_ADC_SOC_TRIG,
                 MPPT_DCDC_IL_ADC_PIN,
                 MPPT_DCDC_ACQPS_SYS_CLKS);//ADCA, SOC2 is selected in the settings file

    ADC_setupSOC(MPPT_DCDC_VOUT_ADC_BASE,
                 MPPT_DCDC_VOUT_ADC_SOC_NO,
                 MPPT_DCDC_ADC_SOC_TRIG,
                 MPPT_DCDC_VOUT_ADC_PIN,
                 MPPT_DCDC_ACQPS_SYS_CLKS);//ADCB, SOC12 is selected in the settings file

    //
    // Setting up the SOCs for DC Bus voltage sampling
    //
    ADC_setupSOC(MPPT_DCDC_VPANEL_ADC_BASE,
                 MPPT_DCDC_VPANEL_ADC_SOC_NO,
                 MPPT_DCDC_ADC_SOC_TRIG,
                 MPPT_DCDC_VPANEL_ADC_PIN,
                 MPPT_DCDC_ACQPS_SYS_CLKS);//ADCA, SOC0 is selected in the settings file

    ADC_setupSOC(MPPT_DCDC_VBUS_ADC_BASE,
                 MPPT_DCDC_VBUS_ADC_SOC_NO,
                 MPPT_DCDC_ADC_SOC_TRIG,
                 MPPT_DCDC_VBUS_ADC_PIN,
                 MPPT_DCDC_ACQPS_SYS_CLKS);//ADCA, SOC1 is selected in the settings file
    //
    // Set MPPT_DCDC_ADC_INTERRUPT_SOC of MPPT_DCDC_ADC_INTERRUPT_BASE to set
    // the interrupt 1 flag
    // This is just used in offset calibration routine and not to interrupt CPU
    //
    ADC_setInterruptSource(MPPT_DCDC_ADC_INTERRUPT_BASE, ADC_INT_NUMBER1,
                           MPPT_DCDC_ADC_INTERRUPT_SOC);
    ADC_enableInterrupt(MPPT_DCDC_ADC_INTERRUPT_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(MPPT_DCDC_ADC_INTERRUPT_BASE, ADC_INT_NUMBER1);

}


bool MPPTDCDC_HAL_adcConversionComplete(void)
{
    return ADC_getInterruptStatus(MPPT_DCDC_ADC_INTERRUPT_BASE, ADC_INT_NUMBER1);
}

//
// Setting up the trigger for ADCs
//
void MPPTDCDC_HAL_setupEPWMtoTriggerADCSOC(uint32_t base)
{
    //
    // Select SOC from counter at ctr =CMPBU
    //
    EPWM_setADCTriggerSource(base, EPWM_SOC_A, EPWM_SOC_TBCTR_U_CMPB);

    //
    // write value to CMPB
    //
    EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B,
                                MPPT_DCDC_ADC_TRIGGER_CMPB);

    //
    // Generate pulse on 2nd event
    //
    EPWM_setADCTriggerEventPrescale(base, EPWM_SOC_A,
                                    MPPT_DCDC_CNTRL_ISR_FREQ_RATIO);

    //
    // Enable SOC on A group
    //
    EPWM_enableADCTrigger(base, EPWM_SOC_A);

}


//
// Stop all PWM clocks
//
void MPPTDCDC_HAL_disablePWMCLKCounting(void)
{
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

//
// Start all PWM clocks
//
void MPPTDCDC_HAL_enablePWMCLKCounting(void)
{
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

//
// Setup PWM signals for single phase
//
void MPPTDCDC_HAL_setupDCDCBoostPWM(uint32_t highFreqBase,
                               uint16_t pwm_period_ticks,
                               uint16_t pwm_dbred_ticks,
                               uint16_t pwm_dbfed_ticks)
{
    //

    //
    // Time Base SubModule Registers
    //
    EPWM_setPeriodLoadMode(highFreqBase, EPWM_PERIOD_SHADOW_LOAD);
    EPWM_setTimeBasePeriod(highFreqBase, pwm_period_ticks >> 1);
    EPWM_setTimeBaseCounter(highFreqBase, 1000);
   // EPWM_setPhaseShift(base3A, 0);
    EPWM_setTimeBaseCounterMode(highFreqBase, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_setClockPrescaler(highFreqBase, EPWM_CLOCK_DIVIDER_1,
                           EPWM_HSCLOCK_DIVIDER_14);

    //
    // Counter Compare Submodule Registers
    // Initialize with 0% duty
    //
    EPWM_setCounterCompareValue(highFreqBase, EPWM_COUNTER_COMPARE_A,
                                (0.5f * (pwm_period_ticks >> 1)));
    EPWM_setCounterCompareShadowLoadMode(highFreqBase, EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    //
    // Action Qualifier SubModule Registers
    //
    EPWM_setActionQualifierAction(highFreqBase, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(highFreqBase, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

    //
    // Configure AHC mode with suitable dead time
    //
    EPWM_setDeadBandCounterClock(highFreqBase,
                                 EPWM_DB_COUNTER_CLOCK_FULL_CYCLE);
    EPWM_setRisingEdgeDelayCount(highFreqBase, pwm_dbred_ticks);
    EPWM_setFallingEdgeDelayCount(highFreqBase, pwm_dbfed_ticks);
    EPWM_setDeadBandDelayMode(highFreqBase, EPWM_DB_RED, 1);
    EPWM_setDeadBandDelayMode(highFreqBase, EPWM_DB_FED, 1);
    EPWM_setRisingEdgeDeadBandDelayInput(highFreqBase, EPWM_DB_INPUT_EPWMA);
    EPWM_setFallingEdgeDeadBandDelayInput(highFreqBase, EPWM_DB_INPUT_EPWMA);
    EPWM_setDeadBandDelayPolarity(highFreqBase, EPWM_DB_FED,
                                  EPWM_DB_POLARITY_ACTIVE_LOW);
    EPWM_setDeadBandDelayPolarity(highFreqBase, EPWM_DB_RED,
                                  EPWM_DB_POLARITY_ACTIVE_HIGH);
}
//
//Set up HB LLC PWM********************
void MPPTDCDC_HAL_setupHBLLCPWM(uint32_t highFreqBase,
                               uint16_t pwm_period_ticks,
                               uint16_t pwm_dbred_ticks,
                               uint16_t pwm_dbfed_ticks)
{
    //
    // Time Base SubModule Registers
    //
    EPWM_setPeriodLoadMode(highFreqBase, EPWM_PERIOD_SHADOW_LOAD);
    //EPWM_setPeriodLoadMode(highFreqBase, EPWM_PERIOD_DIRECT_LOAD);
    EPWM_setTimeBasePeriod(highFreqBase, pwm_period_ticks >> 1);
    EPWM_setTimeBaseCounter(highFreqBase, 1000);
   // EPWM_setPhaseShift(base3A, 0);
    EPWM_setTimeBaseCounterMode(highFreqBase, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_setClockPrescaler(highFreqBase, EPWM_CLOCK_DIVIDER_1,
                           EPWM_HSCLOCK_DIVIDER_1);

    //
    // Counter Compare Submodule Registers
    // Initialize with 50% duty
    //
    EPWM_setCounterCompareValue(highFreqBase, EPWM_COUNTER_COMPARE_A,
                                pwm_period_ticks >> 2);
    EPWM_setCounterCompareShadowLoadMode(highFreqBase, EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    //
    // Action Qualifier SubModule Registers
    //
    /*EPWM_setActionQualifierAction(highFreqBase, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(highFreqBase, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);*/
    EPWM_setActionQualifierAction(highFreqBase, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(highFreqBase, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);

    //
    // Configure AHC mode with suitable dead time
    //
    EPWM_setDeadBandCounterClock(highFreqBase,
                                 EPWM_DB_COUNTER_CLOCK_FULL_CYCLE);
    EPWM_setRisingEdgeDelayCount(highFreqBase, pwm_dbred_ticks);
    EPWM_setFallingEdgeDelayCount(highFreqBase, pwm_dbfed_ticks);
    EPWM_setDeadBandDelayMode(highFreqBase, EPWM_DB_RED, 1);
    EPWM_setDeadBandDelayMode(highFreqBase, EPWM_DB_FED, 1);
    EPWM_setRisingEdgeDeadBandDelayInput(highFreqBase, EPWM_DB_INPUT_EPWMA);
    EPWM_setFallingEdgeDeadBandDelayInput(highFreqBase, EPWM_DB_INPUT_EPWMA);
    EPWM_setDeadBandDelayPolarity(highFreqBase, EPWM_DB_FED,
                                  EPWM_DB_POLARITY_ACTIVE_LOW);
    EPWM_setDeadBandDelayPolarity(highFreqBase, EPWM_DB_RED,
                                  EPWM_DB_POLARITY_ACTIVE_HIGH);
}
//*****************end of HB LLC PWM Set up*********
//
// Setup PWM signals for 2 phases
//
void MPPTDCDC_HAL_setupPWM()
{
    SysCtl_setSyncOutputConfig(SYSCTL_SYNC_OUT_SRC_EPWM1SYNCOUT);
    //
    // Setup IL PWM Phase 1
    //
    MPPTDCDC_HAL_setupDCDCBoostPWM(MPPT_DCDC_HIGH_FREQ_PWM_A_BASE,
                           MPPT_DCDC_PWM_PERIOD,
                           MPPT_DCDC_HIGH_FREQ_PWM_DB_TICKS,
                           MPPT_DCDC_HIGH_FREQ_PWM_DB_TICKS);

    //
    // Setup IL PWM Phase 2
    //
    MPPTDCDC_HAL_setupDCDCBoostPWM(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE,
                           MPPT_DCDC_PWM_PERIOD,
                           MPPT_DCDC_HIGH_FREQ_PWM_DB_TICKS,
                           MPPT_DCDC_HIGH_FREQ_PWM_DB_TICKS);

    //
    MPPTDCDC_HAL_setupHBLLCPWM(MPPT_DCDC_LLC_PWM_BASE,
                               MPPT_DCDC_LLC_PWM_PRD_DEFAULT,
                              MPPT_DCDC_HIGH_FREQ_PWM_DB_TICKS,
                              MPPT_DCDC_HIGH_FREQ_PWM_DB_TICKS);

    //
    // configure PWM 1 as master and rest all PWMs as slaves in a serial chain
    // let it pass the sync in pulse from PWM1
    //
    EPWM_disablePhaseShiftLoad(MPPT_DCDC_HIGH_FREQ_PWM_A_BASE);
    EPWM_enableSyncOutPulseSource(MPPT_DCDC_HIGH_FREQ_PWM_A_BASE,
                                  EPWM_SYNC_OUT_PULSE_ON_CNTR_ZERO);

    EPWM_enablePhaseShiftLoad(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE);
//    EPWM_enableSyncOutPulseSource(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE,
//                                EPWM_SYNC_OUT_PULSE_ON_SOFTWARE);
    EPWM_setSyncInPulseSource(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE, EPWM_SYNC_IN_PULSE_SRC_SYNCOUT_EPWM1);
    EPWM_setPhaseShift(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE, 2);
//    EPWM_setPhaseShift(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE, MPPT_DCDC_PWM_PERIOD*0.8f);//SAC. Phase shift is half period
    EPWM_setCountModeAfterSync(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE,
                                EPWM_COUNT_MODE_UP_AFTER_SYNC);

    //LLC PWM cannot be synced with fixed freq IL Boost PWMM
    EPWM_disablePhaseShiftLoad(MPPT_DCDC_LLC_PWM_BASE);
    //EPWM_setSyncOutPulseMode(MPPT_DCDC_LLC_PWM_BASE,
     //                           EPWM_SYNC_OUT_PULSE_ON_SOFTWARE);

    //GPIO_setControllerCore(0, GPIO_CORE_CPU1);
    GPIO_setDirectionMode(MPPT_DCDC_HIGH_FREQ_PWM_A_GPIO_POS, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(MPPT_DCDC_HIGH_FREQ_PWM_A_GPIO_POS, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(MPPT_DCDC_HIGH_FREQ_PWM_A_GPIO_POS_PIN_CONFIG);

    GPIO_setDirectionMode(MPPT_DCDC_HIGH_FREQ_PWM_A_GPIO_NEG, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(MPPT_DCDC_HIGH_FREQ_PWM_A_GPIO_NEG, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(MPPT_DCDC_HIGH_FREQ_PWM_A_GPIO_NEG_PIN_CONFIG);

    GPIO_setDirectionMode(MPPT_DCDC_HIGH_FREQ_PWM_B_GPIO_POS, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(MPPT_DCDC_HIGH_FREQ_PWM_B_GPIO_POS, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(MPPT_DCDC_HIGH_FREQ_PWM_B_GPIO_POS_PIN_CONFIG);
    GPIO_setDirectionMode(MPPT_DCDC_HIGH_FREQ_PWM_B_GPIO_NEG, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(MPPT_DCDC_HIGH_FREQ_PWM_B_GPIO_NEG, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(MPPT_DCDC_HIGH_FREQ_PWM_B_GPIO_NEG_PIN_CONFIG);

    // Setting up GPIOs for LLC PWMs
    //
    GPIO_setDirectionMode(MPPT_DCDC_LLC_PWM_GPIO_POS, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(MPPT_DCDC_LLC_PWM_GPIO_POS, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(MPPT_DCDC_LLC_PWM_GPIO_POS_PIN_CONFIG);
    GPIO_setDirectionMode(MPPT_DCDC_LLC_PWM_GPIO_NEG, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(MPPT_DCDC_LLC_PWM_GPIO_NEG, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(MPPT_DCDC_LLC_PWM_GPIO_NEG_PIN_CONFIG);
}

//
// Setting up CMPSS for on-board over-current detection
//
void MPPTDCDC_HAL_setupCMPSS(uint32_t base1,
                     float32_t current_limit,
                     float32_t current_max_sense)
{
    //
    // Enable CMPSS1
    //
    CMPSS_enableModule(base1);

    //
    // Use VDDA as the reference for comparator DACs
    //
    CMPSS_configDAC(base1,
                    CMPSS_DACVAL_SYSCLK |
                    CMPSS_DACREF_VDDA |
                    CMPSS_DACSRC_SHDW);

    //
    // Set DAC H/L values for positive and negative limit of current
    //
    CMPSS_setDACValueHigh(base1, 2048 + (int16_t)((float32_t)current_limit *
                          (float32_t)2048.0f / (float32_t)current_max_sense));
    CMPSS_setDACValueLow(base1, 2048 - (int16_t)((float32_t)current_limit *
                         (float32_t)2048.0f / (float32_t)current_max_sense));

    //
    // Configures High Comparator trip signal as asynchronous and non-inverted
    //
    CMPSS_configHighComparator(base1, CMPSS_INSRC_DAC);

    //
    // Configures Low Comparator trip signal as asynchronous and inverted
    //
    CMPSS_configLowComparator(base1, CMPSS_INSRC_DAC | CMPSS_INV_INVERTED);

    //
    // Configure digital filters for High and low comparators
    //
    CMPSS_configFilterHigh(base1, 2, 10, 7);
    CMPSS_configFilterLow(base1, 2, 10, 7);

    //
    // Reset filter logic & start filtering
    //
    CMPSS_initFilterHigh(base1);
    CMPSS_initFilterLow(base1);

    //
    // Configure CTRIPOUTH and CTRIPH outputs as digital filtered
    //
    CMPSS_configOutputsHigh(base1, CMPSS_TRIPOUT_FILTER | CMPSS_TRIP_FILTER);
    CMPSS_configOutputsLow(base1, CMPSS_TRIPOUT_FILTER | CMPSS_TRIP_FILTER);

    //
    // Comparator hysteresis (positive feedback), set to 2x typical value
    //
    CMPSS_setHysteresis(base1, 2);

    //
    // Clear the latched comparator events
    //
    CMPSS_clearFilterLatchHigh(base1);
    CMPSS_clearFilterLatchLow(base1);
}

//
// Setup PWM module for Trip conditions
//
void MPPT_DCDC_HAL_setupPWMforTrip(uint32_t base)
{

#if MPPT_DCDC_CMPSS_PROTECTION_ENABLED == 1U

    //
    // Configure Digital Compare Sub-module as Trip 4 is the input to the
    // DCAHCOMPSEL
    //
    EPWM_selectDigitalCompareTripInput(base,
                                      EPWM_DC_TRIP_TRIPIN4,
                                      EPWM_DC_TYPE_DCAH);
    EPWM_setTripZoneDigitalCompareEventCondition(base,
                                                EPWM_TZ_DC_OUTPUT_A1,
                                                EPWM_TZ_EVENT_DCXH_HIGH);
    EPWM_setDigitalCompareEventSource(base,
                                     EPWM_DC_MODULE_A,
                                     EPWM_DC_EVENT_1,
                                     EPWM_DC_EVENT_SOURCE_ORIG_SIGNAL);
    EPWM_setDigitalCompareEventSyncMode(base,
                                       EPWM_DC_MODULE_A,
                                       EPWM_DC_EVENT_1,
                                       EPWM_DC_EVENT_INPUT_NOT_SYNCED);

    //
    // Enable the following trips - DCAEVT1
    //
    EPWM_enableTripZoneSignals(base, EPWM_TZ_SIGNAL_DCAEVT1);

    //
    // Clear any spurious DCAEVT1 flags
    //
    EPWM_clearTripZoneFlag(base, EPWM_TZ_FLAG_DCAEVT1);

#endif

    //
    // Configuration to make PWMxA/B outputs ACTIVE LOW at trip conditions
    //
    EPWM_setTripZoneAction(base, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);
    EPWM_setTripZoneAction(base, EPWM_TZ_ACTION_EVENT_TZB, EPWM_TZ_ACTION_LOW);

    //
    // Enable the following Cycle-by-Cycle trip for Emulator Stop
    //
    EPWM_enableTripZoneSignals(base, EPWM_TZ_SIGNAL_CBC6);

    //
    // Clear spurious emulator stop signal
    //
    EPWM_clearCycleByCycleTripZoneFlag(base, EPWM_TZ_CBC_FLAG_6);
}

//
// Sets up Board protection by enabling trip signals for various fault
// conditions
//
void MPPTDCDC_HAL_setupBoardProtection()
{

#if MPPT_DCDC_CMPSS_PROTECTION_ENABLED == 1U
    //
    // Setting up CMPSS modules for 3-phase over-current protection
    //
    MPPTDCDC_HAL_setupCMPSS(MPPT_DCDC_IL_CMPSS_BASE,
                           MPPT_DCDC_IL_TRIP_LIMIT_AMPS,
                           MPPT_DCDC_IL_MAX_SENSE);

    //
    // Setting up EPWM X-Bar for 3 CMPSS input signals
    //

    //
    // Disable all the muxes first to avoid any spurious enables
    //
    XBAR_disableEPWMMux(XBAR_TRIP4, 0xFF);
    XBAR_setEPWMMuxConfig(XBAR_TRIP4, MPPT_DCDC_IL_XBAR_MUX_VAL);
    XBAR_enableEPWMMux(XBAR_TRIP4, MPPT_DCDC_IL_XBAR_MUX);

    XBAR_clearInputFlag(MPPT_DCDC_IL_XBAR_FLAG1);
    XBAR_clearInputFlag(MPPT_DCDC_IL_XBAR_FLAG2);

#endif

    //
    // Setting up all PWM modules for One-shot Trip events based on external
    // Trip-zone fault signals and internal CMPSS signals
    //
    MPPT_DCDC_HAL_setupPWMforTrip(MPPT_DCDC_HIGH_FREQ_PWM_A_BASE);
    MPPT_DCDC_HAL_setupPWMforTrip(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE);
    MPPT_DCDC_HAL_setupPWMforTrip(MPPT_DCDC_LLC_PWM_BASE);

    MPPTDCDC_HAL_forceOSTPWMs();

}

void MPPTDCDC_HAL_setupDebugGPIO(void)
{
    GPIO_setDirectionMode(MPPT_DCDC_DEBUG_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(MPPT_DCDC_DEBUG_GPIO, GPIO_QUAL_ASYNC);
    GPIO_setPinConfig(MPPT_DCDC_DEBUG_GPIO_PIN_CONFIG);
}

void MPPTDCDC_HAL_setupHBGPIO(void)
{
    GPIO_setDirectionMode(MPPT_DCDC_HB_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(MPPT_DCDC_HB_GPIO, GPIO_QUAL_ASYNC);
    GPIO_setPinConfig(MPPT_DCDC_HB_GPIO_PIN_CONFIG);
}

void MPPTDCDC_HAL_setupBoardGPIOs(void)
{

    //
    // Setup Debug GPIO for ISR profiling
    //
    MPPTDCDC_HAL_setupDebugGPIO();

    //
    // Setup Heartbeat led GPIO
    //
    MPPTDCDC_HAL_setupHBGPIO();
}

//
// Setup EPWM interrupt generation
//
void MPPTDCDC_HAL_setupPWMInterruptGeneration(uint32_t base,
                                                         uint16_t cmpc_val)
{
    //
    // write value to CMPC
    //
    EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_C, cmpc_val);
    EPWM_setInterruptSource(base, EPWM_INT_TBCTR_D_CMPC);
    EPWM_setInterruptEventCount(base, MPPT_DCDC_CNTRL_ISR_FREQ_RATIO);
    EPWM_enableInterrupt(base);
    EPWM_clearEventTriggerInterruptFlag(base);
}


//
// Setting up C28x Interrupts or CLA tasks for control loops
//
void MPPTDCDC_HAL_setupInterrupt(void)
{

#if(MPPT_DCDC_ISR1_RUNNING_ON == CLA_CORE ||                                    \
                    MPPT_DCDC_ISR2_RUNNING_ON == CLA_CORE)

        MPPTDCDC_HAL_setupCLA();
#endif

    //
    // Setting up EPWM to trigger interrupt during down count where the ADC
    // conversion completes
    //
    MPPTDCDC_HAL_setupPWMInterruptGeneration(MPPT_DCDC_ISR1_TRIG_BASE,
                                             MPPT_DCDC_ADC_TRIGGER_CMPB);

    CPUTimer_enableInterrupt(MPPT_DCDC_ISR2_TRIG_BASE);

    CPUTimer_clearOverflowFlag(MPPT_DCDC_ISR2_TRIG_BASE);

#if MPPT_DCDC_ISR1_ENABLE == 1U
#if MPPT_DCDC_ISR1_RUNNING_ON == C28x_CORE

    //
    // PWM was already enabled to trigger ISR
    //
    Interrupt_register(MPPT_DCDC_ISR1_TRIG, &ISR1);
    MPPTDCDC_HAL_clearISR1InterruptFlag();
    Interrupt_enable(MPPT_DCDC_ISR1_TRIG);

#endif
#endif

#if MPPT_DCDC_ISR2_ENABLE == 1U
    //
    // CPU Timer 2 was already enabled as part of Device setup
    //
    Interrupt_register(MPPT_DCDC_ISR2_TRIG, &ISR2);
    Interrupt_enable(MPPT_DCDC_ISR2_TRIG);

#endif

    EALLOW;

    //
    // Enable Global interrupt INTM
    //
    EINT;

    //
    //Enable Global realtime interrupt DBGM
    //
    ERTM;

    EDIS;

}





