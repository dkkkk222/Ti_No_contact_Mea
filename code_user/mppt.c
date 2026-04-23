/*
 * mppt.c
 *
 *  Created on: 2025Äê9ÔÂ24ÈÕ
 *      Author: pc
 */




//#############################################################################
//
// FILE:  mppt_dcdc.c
//
// TITLE: This is the solution file.
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

//
//********************* the includes ******************************************
//

#include <mppt.h>

//
// Global Variables
//
//
// Controller variables
//
MPPTDCDC_GI MPPTDCDC_gi_id = PI_DEFAULTS;

MPPTDCDC_GV MPPTDCDC_gv_vBus = PI_DEFAULTS;


float32_t MPPTDCDC_gi_id_out;

float32_t MPPTDCDC_gv_vBus_out;

MPPT_PNO_F mppt_pno1;
//
//
// SOLAR variables
//
//
// Board State and Fault status variables
//
MPPTDCDC_stateType MPPTDCDC_state;
MPPTDCDC_struct_boardFaultFlags MPPTDCDC_boardFaultFlags;

//
// Measurement Variables
//
float32_t MPPTDCDC_vOut_sensed_pu;

float32_t MPPTDCDC_vOut_sensedOffset_pu;

float32_t MPPTDCDC_iL_sensed_pu;

float32_t MPPTDCDC_iL_sensedOffset_pu;

volatile float32_t MPPTDCDC_iL_sensed_Avg_pu;

float32_t MPPTDCDC_iL_sensed_pu_prev;

float32_t MPPTDCDC_vPanel_sensed_pu;

volatile float32_t MPPTDCDC_vPanel_sensed_Avg_pu;

float32_t MPPTDCDC_vBus_sensed;
float32_t MPPTDCDC_vBus_sensed_filter;
float32_t MPPTDCDC_vBus_sensed_pu;

float32_t MPPTDCDC_vPanel_sensedOffset_pu;

float32_t MPPTDCDC_vBus_sensedOffset_pu;
//

//
// Absolute/Display Values
//
volatile float32_t MPPTDCDC_guiVpanel,
                   MPPTDCDC_guiVbus;

volatile float32_t MPPTDCDC_guivOut_inst;

volatile float32_t MPPTDCDC_guiiL_inst,MPPTDCDC_guiiL_Avg;
//
// PWM duty variables
//
float32_t MPPTDCDC_duty_A, MPPTDCDC_duty_B, MPPTDCDC_llc_prd;

//
// User command to clear the PWM trip and restore PWM signals
//
volatile uint16_t MPPTDCDC_clearPWMTrip;

//
// User command to update the duty of PWM outputs
// Make sure that the desired duty values for 3 phases are written prior
// using this command
//
volatile uint16_t MPPTDCDC_updateDuty;

//
//
// User command to reset board state and fault flags
//
uint16_t MPPTDCDC_resetAllFaultStatus;


//

//MPPT related variables
float32_t MPPTDCDC_Vpvref_Out;

uint16_t MPPTDCDC_MPPT_ENABLE;
uint16_t MPPTDCDC_Run_MPPT;
uint16_t MPPTDCDC_MPPT_slew;
//
float32_t MPPTDCDC_id_pu;
volatile float32_t MPPTDCDC_idRef_pu;

volatile float32_t MPPTDCDC_idRef;

//

float32_t MPPTDCDC_activePower;

#pragma SET_DATA_SECTION()

//DLOG_4CH MPPTDCDC_dLog1;
//float32_t MPPTDCDC_dBuff1[MPPTDCDC_DLOG_SIZE];
//float32_t MPPTDCDC_dBuff2[MPPTDCDC_DLOG_SIZE];
//float32_t MPPTDCDC_dBuff3[MPPTDCDC_DLOG_SIZE];
//float32_t MPPTDCDC_dBuff4[MPPTDCDC_DLOG_SIZE];
//float32_t MPPTDCDC_dVal1;
//float32_t MPPTDCDC_dVal2;
//float32_t MPPTDCDC_dVal3;
//float32_t MPPTDCDC_dVal4;



void MPPTDCDC_globalVariablesInit(void)
{

    //mppt pno  500
    mppt_pno1.DeltaPmin = 0;            //10e-4 W  //0.01V * 0.01A
    mppt_pno1.MaxVolt = 45.0f;       //mV
    mppt_pno1.MinVolt = 0.0;       //mV
    mppt_pno1.Stepsize = 0.2f;    //mV
    mppt_pno1.Vref = 30.0f;
    mppt_pno1.PanelPower_Prev = 0;
    mppt_pno1.PanelPower = 0;
    mppt_pno1.mppt_first = 1;
}

//
// SFRA Related Variables
//
float32_t MPPTDCDC_plantMagVect[MPPT_DCDC_SFRA_FREQ_LENGTH];
float32_t MPPTDCDC_plantPhaseVect[MPPT_DCDC_SFRA_FREQ_LENGTH];
float32_t MPPTDCDC_olMagVect[MPPT_DCDC_SFRA_FREQ_LENGTH];
float32_t MPPTDCDC_olPhaseVect[MPPT_DCDC_SFRA_FREQ_LENGTH];
float32_t MPPTDCDC_freqVect[MPPT_DCDC_SFRA_FREQ_LENGTH];

//SFRA_F32 MPPTDCDC_sfra1;
//
// Setup SFRA for capturing plant and close loop response
//
//void MPPTDCDC_setupSFRA(void)
//{
//    SFRA_F32_reset(&MPPTDCDC_sfra1);
//    SFRA_F32_config(&MPPTDCDC_sfra1,
//                    MPPT_DCDC_SFRA_ISR_FREQ,
//                    MPPT_DCDC_SFRA_AMPLITUDE,
//                    MPPT_DCDC_SFRA_FREQ_LENGTH,
//                    MPPT_DCDC_SFRA_FREQ_START,
//                    MPPT_DCDC_SFRA_FREQ_STEP_MULTIPLY,
//                    MPPTDCDC_plantMagVect,
//                    MPPTDCDC_plantPhaseVect,
//                    MPPTDCDC_olMagVect,
//                    MPPTDCDC_olPhaseVect,
//                    NULL,
//                    NULL,
//                    MPPTDCDC_freqVect,
//                    1);
//
//    SFRA_F32_resetFreqRespArray(&MPPTDCDC_sfra1);
//
//    SFRA_F32_initFreqArrayWithLogSteps(&MPPTDCDC_sfra1,
//                                       MPPT_DCDC_SFRA_FREQ_START,
//                                       MPPT_DCDC_SFRA_FREQ_STEP_MULTIPLY);
//
//    SFRA_GUI_config(MPPT_DCDC_SFRA_GUI_SCI_BASE,
//                    MPPT_DCDC_SCI_VBUS_CLK,
//                    MPPT_DCDC_SFRA_GUI_SCI_BAUDRATE,
//                    MPPT_DCDC_SFRA_GUI_SCIRX_GPIO,
//                    MPPT_DCDC_SFRA_GUI_SCIRX_GPIO_PIN_CONFIG,
//                    MPPT_DCDC_SFRA_GUI_SCITX_GPIO,
//                    MPPT_DCDC_SFRA_GUI_SCITX_GPIO_PIN_CONFIG,
//                    MPPT_DCDC_SFRA_GUI_LED_INDICATOR,
//                    0,
//                    0,
//                   &MPPTDCDC_sfra1,
//                   1);
//
//}


//void MPPTDCDC_runSFRABackGroundTasks(void)
//{
//    //
//    // Call SFRA BG Task which takes care of switching between
//    // frequency points and saving gain/phase data for each
//    // frequency point
//    //
//    SFRA_F32_runBackgroundTask(&MPPTDCDC_sfra1);
//    //
//    // Call to this routine keeps the SCI communication alive with GUI
//    //
//    SFRA_GUI_runSerialHostComms(&MPPTDCDC_sfra1);
//}


/*
void MPPTDCDC_calibrateOffset(uint16_t count)
{
    uint16_t n = count;

    while (n > 0)
    {
        n--;

        MPPTDCDC_HAL_forceAllSOCs();

        //
        // Wait till all ADC conversion complete check
        //
        while(!MPPTDCDC_HAL_adcConversionComplete());

        MPPTDCDC_vOut_sensedOffset_pu += (float32_t)MPPT_DCDC_HAL_VOUT_FB *
                MPPT_DCDC_ADC_PU_SCALE_FACTOR;

        MPPTDCDC_iL_sensedOffset_pu += (float32_t)MPPT_DCDC_HAL_IL_FB *
                MPPT_DCDC_ADC_PU_SCALE_FACTOR;

        MPPTDCDC_vPanel_sensedOffset_pu += (float32_t)MPPT_DCDC_VPANEL_FB *
            MPPT_DCDC_ADC_PU_SCALE_FACTOR;

        MPPTDCDC_vBus_sensedOffset_pu += (float32_t)MPPT_DCDC_VBUS_FB *
            MPPT_DCDC_ADC_PU_SCALE_FACTOR;
    }

    MPPTDCDC_vOut_sensedOffset_pu = MPPTDCDC_vOut_sensedOffset_pu /
                                    ((float32_t)count);

    MPPTDCDC_iL_sensedOffset_pu = MPPTDCDC_iL_sensedOffset_pu /
                                    ((float32_t)count);

    MPPTDCDC_vPanel_sensedOffset_pu = MPPTDCDC_vPanel_sensedOffset_pu /
                                    ((float32_t)count);

    MPPTDCDC_vBus_sensedOffset_pu = MPPTDCDC_vBus_sensedOffset_pu /
                                        ((float32_t)count);

}*/

void MPPTDCDC_updateFaultStatus(void)
{

#if MPPT_DCDC_CMPSS_PROTECTION_ENABLED == 1U

    if(MPPTDCDC_HAL_get_iL_overCurrentFlag())
    {
        MPPTDCDC_boardFaultFlags.fault_iL_overCurrent = true;
    }
    else
    {
        MPPTDCDC_boardFaultFlags.fault_iL_overCurrent = false;
    }


#else
    MPPTDCDC_boardFaultFlags.fault_iL_overCurrent = false;

#endif

    if(MPPTDCDC_resetAllFaultStatus == 1)
    {
        MPPTDCDC_HAL_clearAllFaultFlags();

        MPPTDCDC_resetAllFaultStatus = 0;

        MPPTDCDC_state = STATE_NO_FAULT;

        MPPTDCDC_boardFaultFlags.fault_iL_overCurrent = false;

        MPPTDCDC_boardFaultFlags.fault_vPanel_overVoltage = false;
        MPPTDCDC_boardFaultFlags.fault_vBus_overVoltage = false;

    }


}
