/*
 * DAC.c
 *
 *  Created on: 2025年9月8日
 *      Author: pc
 */
#include "DAC_User.h"



// Defines
//


//
// Globals
//
uint16_t cpuTimer0IntCount;
uint16_t cpuTimer1IntCount;
uint16_t cpuTimer2IntCount;

//
// Globals
//
uint16_t DataLog[DLOG_SIZE];
#pragma DATA_SECTION(DataLog, "DLOG");

uint32_t outputFreq_hz = 1000;
uint32_t maxOutputFreq_hz = 5000;
float waveformGain = 0.8003F; // Range 0.0 -> 1.0
float waveformOffset = 0;    // Range -1.0 -> 1.0
uint16_t lowLimit = 410;
uint16_t highLimit = 3686;

//#if SINEWAVE_TYPE==LOW_THD_SINE //initialize sine wave type
//SGENTI_1 sgen = SGENTI_1_DEFAULTS;
//#elif SINEWAVE_TYPE==HIGH_PRECISION_SINE
//SGENHP_1 sgen = SGENHP_1_DEFAULTS;
//#endif

uint16_t sgen_out = 0;
uint16_t ndx = 0;

uint16_t maxOutput_lsb = 0;
uint16_t minOutput_lsb = 0;
uint16_t pk_to_pk_lsb = 0;

//
// Function Prototypes
//


void DAC_Init()
{
    EALLOW;
    //
    // Set DAC reference voltage.
    //
    DAC_GPIO_Init();

    ASysCtl_disableTemperatureSensor();
    //
    // Set the analog voltage reference selection to external.
    //
    ASysCtl_setAnalogReferenceExternal( ASYSCTL_VREFHI );

    DAC_setReferenceVoltage(myDAC0_BASE, DAC_REF_VDAC);
    //
    // Set DAC load mode.
    //
    DAC_setLoadMode(myDAC0_BASE, DAC_LOAD_SYSCLK);
    //
    // Enable the DAC output
    //
    DAC_enableOutput(myDAC0_BASE);
    //
    // Set the DAC shadow output
    //
    DAC_setShadowValue(myDAC0_BASE, 0U);

    //
    // Delay for buffered DAC to power up.
    //
    DEVICE_DELAY_US(500);

    EDIS;
}

void DAC_1_Init()
{
    EALLOW;
    //
    // Set DAC reference voltage.
    //
    //DAC_GPIO_Init();

    ASysCtl_disableTemperatureSensor();
    //
    // Set the analog voltage reference selection to external.
    //
    ASysCtl_setAnalogReferenceExternal( ASYSCTL_VREFHI );

    DAC_setReferenceVoltage(myDAC1_BASE, DAC_REF_VDAC);
    //
    // Set DAC load mode.
    //
    DAC_setLoadMode(myDAC1_BASE, DAC_LOAD_SYSCLK);
    //
    // Enable the DAC output
    //
    DAC_enableOutput(myDAC1_BASE);
    //
    // Set the DAC shadow output
    //
    DAC_setShadowValue(myDAC1_BASE, 0U);

    //
    // Delay for buffered DAC to power up.
    //
    DEVICE_DELAY_US(500);

    EDIS;
}


void DAC_GPIO_Init(void)
{
    // Analog PinMux for A0/B15/C15/DACA_OUT
    GPIO_setPinConfig(GPIO_231_GPIO231);
    // AIO -> Analog mode selected
    GPIO_setAnalogMode(231, GPIO_ANALOG_ENABLED);

    GPIO_setPinConfig(GPIO_232_GPIO232);
        // AIO -> Analog mode selected
    GPIO_setAnalogMode(232, GPIO_ANALOG_ENABLED);
//    GPIO_setPinConfig(GPIO_231_GPIO231);
//    GPIO_setAnalogMode(231, GPIO_ANALOG_ENABLED);
//    GPIO_setPadConfig(GPIO_231_GPIO231, GPIO_PIN_TYPE_STD);  // Push-pull output or floating input
//    GPIO_setDirectionMode(GPIO_231_GPIO231, GPIO_DIR_MODE_OUT);

}
//
// dlog - Circular DataLog. DataLog[0] contains the next index to
//        be overwritten
//
void DACA_setVoltage(float vout, float vref)
{
    extern volatile int val_lowpass;
    uint16_t code = DAC_codeFromVolt(vout, vref);
    DAC_setShadowValue(DACA_BASE, code);

    // 若使用了非默认加载模式，可能需要等待一次加载事件（如 SYSCLK/PWM 同步）
    // 简单场景下这行即生效（默认按系统时钟加载）
}
static inline uint16_t DAC_codeFromVolt(float vout, float vref)
{
    float codef = (vout / vref) * 4095.0f + 0.5f;
    if (codef < 0.0f)
        codef = 0.0f;
    if (codef > 4095.0f)
        codef = 4095.0f;
    return (uint16_t)codef;
}


/* === 平台相关占位函数（需自行实现）=== /
void Board_Init(void); / 时钟/IO/外设基础初始化 /
void DAC_Init(void); / DAC初始化（参考电压、触发模式等） /
void Timer_Init(double sample_rate); / 配置定时器中断频率 = sample_rate /
void EnableIRQ(void); / 使能全局中断 /
void ClearTimerIntFlag(void); / 清除定时器中断标志 */


/* === 内部状态 === */
uint16_t sine_table[TABLE_SIZE];
volatile uint32_t phase_acc = 0;
volatile uint32_t phase_inc = 0;


/* 工具：夹紧到[0, DAC_MAX] */
static inline uint16_t clamp_u16(int32_t x)
{
    if (x < 0)
        return 0;
    if (x > (int32_t)DAC_MAX)
        return DAC_MAX;
    return (uint16_t)x;
}

/* 生成正弦查找表
amplitude: 0.0~1.0（相对满量程），建议<=0.95避免削顶
dc_offset: 0.0~1.0（相对满量程），0.5约为中点偏置（单电源DAC常用） */
void SineTable_Init(float amplitude, float dc_offset)
{
    uint16_t temp16;
    int i;
    double theta, s, y;
    if (amplitude < 0.0f)
        amplitude = 0.0f;
    if (amplitude > 1.0f)
        amplitude = 1.0f;
    if (dc_offset < 0.0f)
        dc_offset = 0.0f;
    if (dc_offset > 1.0f)
        dc_offset = 1.0f;

    for (i = 0; i < TABLE_SIZE; ++i)
    {
        theta = (2.0 * M_PI * i) / (double)TABLE_SIZE;
        s = sin(theta);  /* -1..+1 */
        y = (double)(0.1f * (dc_offset + 0.5 * amplitude * s));
        temp16 = (uint16_t)((y / 3.3f) * 4095.0f + 0.5f);
        /* 将 [-1,1] 映射到 DAC 码值 */
        //y = (double)(0.1f * (dc_offset + 0.5 * amplitude * s)) * 26214.4f;
        //int32_t code = (int32_t)lround(y);
        sine_table[i] = temp16;
        //clamp_u16(code);
    }
}

/* 设置输出频率（Hz）*/
void DDS_SetFrequency(double fout)
{
    if (fout < 0.0)
        fout = 0.0;
    /* phase_inc = fout * 2^PHASE_BITS / Fs */
    double inc = fout * (double)(1ULL << PHASE_BITS) / SAMPLE_RATE;
    if (inc < 0.0)
        inc = 0.0;
    if (inc > (double)0xFFFFFFFFu)
        inc = (double)0xFFFFFFFFu;
    phase_inc = (uint32_t)(inc + 0.5);
}

