/*
 * Timer_User.c
 *
 *  Created on: 2025Äê9ÔÂ9ÈÕ
 *      Author: pc
 */
#include "Timer_User.h"
#ifndef CODE_USER_TIMER_USER_C_
#define CODE_USER_TIMER_USER_C_


float freqResolution_hz = 0;
float cpuPeriod_us = 0;
uint32_t interruptCycles = 0;
float interruptDuration_us = 0;
float samplingPeriod_us = 0;

uint32_t samplingFreq_hz = 64000;

void Init_Timer(void)
{

    Interrupt_register(INT_TIMER0, &cpu_timer0_isr);
    Interrupt_register(INT_TIMER1, &cpu_timer1_isr);
    //
    // Initialize variables
    //
    //cpuPeriod_us = (1000000.0F/DEVICE_SYSCLK_FREQ);
    samplingPeriod_us = (1000000.0F/samplingFreq_hz);

    //
    // Initialize Cpu Timers
    //
    initCPUTimers();

    //
    // Configure Cpu Timer0 to interrupt at specified sampling frequency
    //
    configCPUTimer(CPUTIMER0_BASE, samplingPeriod_us);

    //Init Timer1 period.
    //samplingPeriod_us = (1000000.0F / TABLE_SIZE);
    configCPUTimer(CPUTIMER1_BASE, 50);
    //
    // Enable interrupt
    //
    Interrupt_enable(INT_TIMER0);
    Interrupt_enable(INT_TIMER1);
    //Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP7);

    //
    // Start Cpu Timer0
    //
    CPUTimer_startTimer(CPUTIMER0_BASE);
    CPUTimer_startTimer(CPUTIMER1_BASE);

}


//
// initCPUTimers - This function initializes all three CPU timers
// to a known state.
//
void initCPUTimers(void)
{
    //
    // Initialize timer period to maximum
    //
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER2_BASE, 0xFFFFFFFF);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);

    //
    // Make sure timer is stopped
    //
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    //
    // Reload all counter register with period value
    //
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    //
    // Reset interrupt counter
    //

}

//
// configCPUTimer - This function initializes the selected timer to the
// period specified by the "freq" and "period" variables. The "freq" is
// CPU frequency in Hz and the period in uSeconds. The timer is held in
// the stopped state after configuration.
//
void configCPUTimer(uint32_t cpuTimer, float period)
{
    uint32_t temp, freq = DEVICE_SYSCLK_FREQ;       //120M

    //
    // Initialize timer period:
    //
    temp = ((freq / 1000000) * period);
    CPUTimer_setPeriod(cpuTimer, temp - 1);

    //
    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    //
    CPUTimer_setPreScaler(cpuTimer, 0);

    //
    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    //
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer,
                              CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(cpuTimer);


}









#endif /* CODE_USER_TIMER_USER_C_ */
