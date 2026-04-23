#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "c2000ware_libraries.h"
#include "main.h"

#define ADC_Fre 1000

uint8_t  send_data_1[2] = {0x90, 0x00};
uint8_t  send_data_2[2] = {0xA2, 0x00};
uint16_t  send_data_3[8] = {0x0010, 0x0020, 0x0030, 0x0040, 0x0050, 0x0060, 0x0070, 0x0080};
//
// Main
//
int16_t order_vul = 0;
uint32_t Rev_data = 0;
float val_lowpass_int[50];
float temp_float;
float comp = 0.5f;

float ADC_UAT[ADC_Fre];
float ADC_EAT[ADC_Fre];


uint32_t index_bits;// = (uint32_t)(logf((double)TABLE_SIZE) / logf(2.0) + 0.5);
uint16_t rise_dead = 20;
uint16_t fall_dead = 20;

uint16_t rise_dead_1 = 20;
uint16_t fall_dead_1 = 20;
uint16_t Comp_PI;

uint16_t adc_Get = 0;
uint16_t adc_Get1 = 0;
uint8_t PI_flag = 0;

float temp_Vout;
float temp_Cur;
float temp_Vin;

void main(void)
{
    extern volatile float vol_lowpass;
    extern volatile float vol_lowpass_in;
    extern volatile float cur_lowpass;
    //
    // Initialize device clock and peripherals
    //

    index_bits = (uint32_t)(logf((double)TABLE_SIZE) / logf(2.0) + 0.5);
    Device_init();

    MPPTDCDC_globalVariablesInit();
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

    MPPTDCDC_HAL_setupPWM();

    //
    // PinMux and Peripheral Initialization
    //
    Board_init();

    //
    // C2000Ware Library initialization
    //
    C2000Ware_libraries_init();

    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //
    //CLA_Init();

    //ePWM_Init();
    setupProfileGpio();

    Init_Timer();
    CAN_Init();

    DAC_Init();
//    DAC_1_Init();

    SineTable_Init(1.00f, 0.50f);
//
//    /* �������Ƶ��? */
//    DDS_SetFrequency(DEFAULT_F_OUT);
    SPI_Init();
    //SCIB_Init();
    SCI_Init();
    ADC_Init();
    po_algorithm_init(s);
    po_input_set(&po_input);
    // Enable Global Interrupt (INTM) and real time interrupt (DBGM)
    //
    EINT;
    ERTM;

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

//    EPWM_setEmulationMode(EPWM1_BASE, EPWM_EMULATION_FREE_RUN);
//    EPWM_setCounterCompareValue(MPPT_DCDC_HIGH_FREQ_PWM_A_BASE, EPWM_COUNTER_COMPARE_A,
//                                    (0.5f*(MPPT_DCDC_PWM_PERIOD >> 1)));
    while(1)
    {
//        if(PI_flag == 0)
//        {
//            EPWM_setCounterCompareValue(MPPT_DCDC_HIGH_FREQ_PWM_A_BASE, EPWM_COUNTER_COMPARE_A,
//                                                (comp * (MPPT_DCDC_PWM_PERIOD >> 1)));
//
//            EPWM_setCounterCompareValue(MPPT_DCDC_HIGH_FREQ_PWM_B_BASE, EPWM_COUNTER_COMPARE_A,
//                                                        (comp * (MPPT_DCDC_PWM_PERIOD >> 1)));
//        }
    }
}

uint32_t idx;
uint16_t sample, CAN_test_cnt = 0;
uint8_t num_lowpass = 0;
uint16_t data_sci[2];
float DAC_Sine;
uint8_t Sin_Finish_flag = 0;
float DAC_max_float = DAC_MAX;
uint16_t DAC_Temp = 0x8888;
//1kHz��Sin������
interrupt void cpu_timer0_isr(void)
{
//    IER &= ~M_INT13;
    EALLOW;
    //HWREGH(CPUTIMER0_BASE + CPUTIMER_O_TCR) |= CPUTIMER_TCR_TIF;


    //sample = sine_table[idx & (TABLE_SIZE - 1)];
//    DAC_Temp = sample & 0xffff;
////    DAC_Temp = 0x8888;
//    data_sci[0] = 0x0000 + ((DAC_Temp & 0xff00) >> 8);
//    data_sci[1] = 0x0000 + ((DAC_Temp & 0x00ff) << 8);
//    SPI_send_data(data_sci, 2);


    sample = sine_table[idx & (TABLE_SIZE - 1)];//(float)sample / DAC_max_float * 0.100f;
    //DACA_setVoltage(DAC_Sine, 3.3f);
    DAC_setShadowValue(DACA_BASE, sample);
    idx++;
    if(idx >= TABLE_SIZE)
    {
        idx = 0;
    }
    CPUTimer_clearOverflowFlag(CPUTIMER0_BASE);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
//    IER |= M_INT13;
    EDIS;
}

uint16_t ADC_dex = 0;
double ADC_Cal;
double ADC_Cal_temp;
//������㷨��ADC������1kHz
interrupt void cpu_timer1_isr(void)
{
    uint16_t i = 0;
    //HWREGH(CPUTIMER1_BASE + CPUTIMER_O_TCR) |= CPUTIMER_TCR_TIF;
    CPUTimer_clearOverflowFlag(CPUTIMER1_BASE);
    //SPI_send_data(send_data_3,1);


    //Rev_data = SPI_readDataNonBlocking(SPIA_BASE);
    //get_AD(Rev_data);

    //��ȡADC�ź�
    HWREGH(myADC0_BASE + ADC_O_SOCFRC1) = ADC_FORCE_SOC1 | ADC_FORCE_SOC2;
    ADC_forceMultipleSOC(myADC0_BASE,  (ADC_FORCE_SOC1 | ADC_FORCE_SOC2));
    //ADC_forceSOC(myADC0_BASE,  (ADC_FORCE_SOC0 | ADC_FORCE_SOC1));// | ADC_FORCE_SOC1));
        //
        // Wait for ADCA to complete, then acknowledge flag
        //
    Rev_data = 0;
    while(((HWREGH(myADC0_BASE + ADC_O_INTFLG) & (1U << (uint16_t)ADC_INT_NUMBER2)) != 0U) == false)//ADC_getInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1)
    {
        Rev_data++;
        if(Rev_data > 1000)
        {
            Rev_data = 0;
            break;
        }
    }
    HWREGH(myADC0_BASE + ADC_O_INTFLGCLR) = (uint16_t)1U << (uint16_t)ADC_INT_NUMBER1;
    //ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);

    adc_Get = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);

    adc_Get1 = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2);
//    if((Get_AD < min_ad) && (Get_AD > 400))
//    {
//        min_ad = Get_AD;
//    }
//    if((Get_AD > Max_ad) && (Get_AD > 400))
//    {
//        Max_ad = Get_AD;
//    }
    if(Sin_Finish_flag == 0)
    {
        ADC_UAT[ADC_dex] = (float)adc_Get / 4096.0f * 3.3f;
        ADC_EAT[ADC_dex] = (float)adc_Get1 / 4096.0f * 3.3f;
        ADC_dex++;
        if(ADC_dex >= ADC_Fre)
        {
            Sin_Finish_flag = 1;
            ADC_dex = 0;
        }
    }
    //���л������?
    else if(Sin_Finish_flag == 1)
    {
        for(i = 0; i < ADC_Fre; i++)
        {
            ADC_Cal_temp += ADC_UAT[ADC_dex] * ADC_EAT[ADC_dex];
        }
        ADC_Cal = ADC_Cal_temp;
        ADC_Cal_temp = 0;
        Sin_Finish_flag = 0;
    }
}



