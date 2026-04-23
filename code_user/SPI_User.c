/*
 * SPI_User.c
 *
 *  Created on: 2025年9月9日
 *      Author: pc
 */

#include "SPI_User.h"

uint16_t sData[2];                  // Send data buffer
uint16_t rData[2];                  // Receive data buffer
uint16_t rDataPoint = 0;
int Get_AD = 0;
int AD_Rev = 0;
uint16_t Err_times = 0;
uint16_t data_Up   = 14*1000;
uint16_t data_Down = 10*1000;

void SPI_Init(void)
{
    EALLOW;
    SPIA_GPIO_Init();
    SPIA_peripheral_init();
    INT_spi_init();
    EDIS;
}


void SPIA_GPIO_Init(void)
{
    GPIO_setPinConfig(SPIA_peripheral_SPIPICO_PIN_CONFIG);
    GPIO_setPadConfig(SPIA_peripheral_SPIPICO_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIA_peripheral_SPIPICO_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIA_peripheral_SPIPOCI_PIN_CONFIG);
    GPIO_setPadConfig(SPIA_peripheral_SPIPOCI_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIA_peripheral_SPIPOCI_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIA_peripheral_SPICLK_PIN_CONFIG);
    GPIO_setPadConfig(SPIA_peripheral_SPICLK_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIA_peripheral_SPICLK_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIA_peripheral_SPIPTE_PIN_CONFIG);
    GPIO_setPadConfig(SPIA_peripheral_SPIPTE_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIA_peripheral_SPIPTE_GPIO, GPIO_QUAL_ASYNC);
}


void SPIA_peripheral_init(void)
{
    SPI_disableModule(SPIA_peripheral_BASE);
    SPI_setConfig(SPIA_peripheral_BASE, DEVICE_LSPCLK_FREQ, SPI_PROT_POL1PHA0,
                  SPI_MODE_CONTROLLER, SPIA_peripheral_BITRATE, SPIA_peripheral_DATAWIDTH);
    SPI_setPTESignalPolarity(SPIA_peripheral_BASE, SPI_PTE_ACTIVE_LOW);

    SPI_enableFIFO(SPIA_peripheral_BASE);
    SPI_setFIFOInterruptLevel(SPIA_peripheral_BASE, SPI_FIFO_TX2, SPI_FIFO_RX1);

    SPI_clearInterruptStatus(SPIA_peripheral_BASE, SPI_INT_RXFF);
    SPI_enableInterrupt(SPIA_peripheral_BASE, SPI_INT_RXFF);
    SPI_disableLoopback(SPIA_peripheral_BASE);
    SPI_setEmulationMode(SPIA_peripheral_BASE, SPI_EMULATION_FREE_RUN);
    SPI_enableModule(SPIA_peripheral_BASE);
}


void INT_spi_init()
{

    // Interrupt Settings for INT_SPIA_peripheral_RX
    // ISR need to be defined for the registered interrupts
    Interrupt_register(INT_SPIA_peripheral_RX, &spiaRxFIFOISR);
    Interrupt_enable(INT_SPIA_peripheral_RX);

    // Interrupt Settings for INT_SPIA_peripheral_TX
    // ISR need to be defined for the registered interrupts
    Interrupt_register(INT_SPIA_peripheral_TX, &INT_SPIA_peripheral_TX_ISR);
    Interrupt_disable(INT_SPIA_peripheral_TX);


}




void SPI_send_data(uint16_t *data, uint16_t length)
{
    uint16_t i;
    for(i = 0; i < length; i++)
    {
        SPI_writeDataNonBlocking(SPIA_BASE, data[i]);
    }
}
 //
 // Enabled only for SysConfig functionality
 //
__interrupt void INT_SPIA_peripheral_TX_ISR(void)
{
    //
    // Issue ACK
    //
    Interrupt_clearACKGroup(INT_SPIA_peripheral_TX_INTERRUPT_ACK_GROUP);
}



static inline int u16_bits_to_int(uint16_t u)
{
    // 先按有符号16位解释，再转为int，完成符号扩展
    return (int)(int16_t)u;
}

void get_AD(uint16_t input_data)
{
    int temp_int16;

    temp_int16 = u16_bits_to_int(input_data);
    Get_AD = (float)(temp_int16 * 100.7) / 1000 * 5;                    //计算3.3V中的分压
    AD_Rev = (float)(temp_int16 * 100.7 * Volt_Ratio) / 1000;       //计算真实电压值
}


void Data_Monitor(void)
{
    if(AD_Rev > data_Up)
    {
        Err_times++;
    }
    if(AD_Rev > data_Down)
    {
        Err_times++;
    }
}


//
// SPI A Receive FIFO ISR
//

 __interrupt void spiaRxFIFOISR(void)
{
    uint16_t i;

    //
    // Read data
    //
    for(i = 0; i < 1; i++)
    {
        rData[i] = SPI_readDataNonBlocking(SPIA_BASE);
    }


    get_AD(rData[0]);
    //
    // Clear interrupt flag and issue ACK
    //
    SPI_clearInterruptStatus(SPIA_BASE, SPI_INT_RXFF);
    Interrupt_clearACKGroup(INT_SPIA_peripheral_RX_INTERRUPT_ACK_GROUP);
}







