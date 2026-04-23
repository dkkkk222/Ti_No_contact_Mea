/*
 * SCI_User.c
 *
 *  Created on: 2025年11月26日
 *      Author: pc
 */

#include "SCI_User.h"
unsigned char rxBuf_SCIA[RX_BUF_SIZE];
unsigned char rx_count_SCIA;

unsigned char Rec_Succ_SCIA;
unsigned char Rec_Flag_SCIA;


unsigned char rxBuf_SCIB[RX_BUF_SIZE];
unsigned char rx_count_SCIB;

unsigned char Rec_Succ_SCIB;
unsigned char Rec_Flag_SCIB;
uint16_t SCI_OUTTIME_A = 0;
uint16_t SCI_OUTTIME_B = 0;

void SCIB_Init(void)
{
    GPIO_setPinConfig(GPIO_11_SCIB_RX);
    GPIO_setDirectionMode(11U, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(11U, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(11U, GPIO_QUAL_ASYNC);

    //
    // SCI Tx pin configuration.
    //
    GPIO_setPinConfig(GPIO_40_SCIB_TX);
    GPIO_setDirectionMode(40U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(40U, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(40U, GPIO_QUAL_ASYNC);

    SCI_setConfig(SCIB_BASE, DEVICE_LSPCLK_FREQ, 921600, (SCI_CONFIG_WLEN_8 |
                                                            SCI_CONFIG_STOP_ONE |
                                                            SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCIB_BASE);
    //SCI_enableLoopback(SCIA_BASE);
    SCI_resetChannels(SCIB_BASE);
    SCI_enableFIFO(SCIB_BASE);

    //
    // RX and TX FIFO Interrupts Enabled
    //
    SCI_enableInterrupt(SCIB_BASE, (SCI_INT_RXFF));
    SCI_disableInterrupt(SCIB_BASE, SCI_INT_RXERR);

    //
    // The transmit FIFO generates an interrupt when FIFO status
    // bits are less than or equal to 2 out of 16 words
    // The receive FIFO generates an interrupt when FIFO status
    // bits are greater than equal to 2 out of 16 words
    //
    SCI_setFIFOInterruptLevel(SCIB_BASE, SCI_FIFO_TX2, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCIB_BASE);

    SCI_resetTxFIFO(SCIB_BASE);
    SCI_resetRxFIFO(SCIB_BASE);

    Interrupt_register(INT_SCIB_RX, scibRXFIFOISR);
    //Interrupt_register(INT_SCIA_TX, sciaTXFIFOISR);

    Interrupt_enable(INT_SCIB_RX);
    //Interrupt_enable(INT_SCIA_TX);
}


void SCI_Init(void)
{
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCIRXDA);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCIRXDA, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCIRXDA, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCIRXDA, GPIO_QUAL_ASYNC);

    //
    // SCI Tx pin configuration.
    //
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCITXDA);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCITXDA, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCITXDA, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCITXDA, GPIO_QUAL_ASYNC);

    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, 115200, (SCI_CONFIG_WLEN_8 |
                                                            SCI_CONFIG_STOP_ONE |
                                                            SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCIA_BASE);
    //SCI_enableLoopback(SCIA_BASE);
    SCI_resetChannels(SCIA_BASE);
    SCI_enableFIFO(SCIA_BASE);

    //
    // RX and TX FIFO Interrupts Enabled
    //
    SCI_enableInterrupt(SCIA_BASE, (SCI_INT_RXFF));
    SCI_disableInterrupt(SCIA_BASE, SCI_INT_RXERR);

    //
    // The transmit FIFO generates an interrupt when FIFO status
    // bits are less than or equal to 2 out of 16 words
    // The receive FIFO generates an interrupt when FIFO status
    // bits are greater than equal to 2 out of 16 words
    //
    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX2, SCI_FIFO_RX2);
    SCI_performSoftwareReset(SCIA_BASE);

    SCI_resetTxFIFO(SCIA_BASE);
    SCI_resetRxFIFO(SCIA_BASE);

    Interrupt_register(INT_SCIA_RX, sciaRXFIFOISR);
    //Interrupt_register(INT_SCIA_TX, sciaTXFIFOISR);

    Interrupt_enable(INT_SCIA_RX);
    //Interrupt_enable(INT_SCIA_TX);
}

__interrupt void sciaRXFIFOISR(void)
{
    uint16_t i;
    uint16_t rData[16];

    //
    // Check received data
    //
    if(rx_count_SCIA < RX_BUF_SIZE)
    {
        SCI_OUTTIME_A = 0;
        if(rx_count_SCIA == 0)
        {
            //SCI开始接收标志，超时标志位清0
            Rec_Flag_SCIA = 1;
        }
        SCI_readCharArray(SCIA_BASE, rData, 2);
        //接收数据
        for(i = 0; i < 2; i++)
        {
            rxBuf_SCIA[rx_count_SCIA++] = rData[i];
        }
    }

    //rDataPointA = (rDataPointA + 1) & 0x00FF;

    SCI_clearOverflowStatus(SCIA_BASE);

    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
    //
    // Issue PIE ack
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}


__interrupt void scibRXFIFOISR(void)
{
    uint16_t i;
    uint16_t rData[16];

    //
    // Check received data
    //
    if(rx_count_SCIB < RX_BUF_SIZE)
    {
        SCI_OUTTIME_B = 0;
        if(rx_count_SCIB == 0)
        {
            //SCI开始接收标志，超时标志位清0
            Rec_Flag_SCIB = 1;
        }
        SCI_readCharArray(SCIB_BASE, rData, 1);
        //接收数据
        for(i = 0; i < 1; i++)
        {
            YC_Data[rx_count_SCIB++] = rData[i];
        }
    }

    //rDataPointA = (rDataPointA + 1) & 0x00FF;

    SCI_clearOverflowStatus(SCIB_BASE);

    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_RXFF);
    //
    // Issue PIE ack
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void sciaTXFIFOISR(void)
{
    uint16_t i;

    //SCI_writeCharArray(SCIA_BASE, sDataA, 2);

    //
    // Increment send data for next cycle
    //


    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);

    //
    // Issue PIE ACK
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}
void process_frame_SCIB(void)
{
    uint16_t i, Cmd_Type = 0, crc16 = 0, len = 0, crc16_send, Read_Reg_Addr;
    unsigned char cmd;
    //Uint32 test_data[4] = {0xA3A4A1A2, 0x01, 0xAA, 0x22};
    if(Rec_Succ_SCIB == 1)
    {
        Rec_Succ_SCIB = 0;
        Reply_TELE_UART(0xFF);
    }

}

void process_frame_SCIA(void)
{
    uint16_t i, Cmd_Type = 0, crc16 = 0, len = 0, crc16_send, Read_Reg_Addr;
    unsigned char cmd;
    //Uint32 test_data[4] = {0xA3A4A1A2, 0x01, 0xAA, 0x22};
    if(Rec_Succ_SCIA == 1)
    {
        Rec_Succ_SCIA = 0;
        //eCanSendData(15, 4, 0xAA, test_data);
        //中断处理
        for(i = 0; i < 2; i++)
        {
            //确定帧头
            if((rxBuf_SCIA[i] == 0xD2) && (rxBuf_SCIA[i + 1] == 0x8C))
            {
                //确定数据源和目标数据
                if((rxBuf_SCIA[i + 2] == 0x00) && (rxBuf_SCIA[i + 3] == 0x0A))
                {
                    //帧数据长度
                    len = (uint16_t)(rxBuf_SCIA[i + 14] << 8) | rxBuf_SCIA[i + 15];
                    //校验码
                    crc16 = (uint16_t)(rxBuf_SCIA[i + 16 + len] << 8) | rxBuf_SCIA[i + 17 + len];
                    //crc16_send = ModBusCRC16(&rxBuf_SCIA[i + 16], len);
                    crc16_send = crc16_ccitt_false(&rxBuf_SCIA[i + 16], len);
                    Cmd_Type = (uint16_t)(rxBuf_SCIA[i + 11] << 8) + rxBuf_SCIA[i + 12];
                    cmd = rxBuf_SCIA[i + 19];
                    if(rxBuf_SCIA[i + 12] == 0x1e)
                    {
                        Send_YC();
                    }
                    else
                    {
                        Send_YK(cmd);
                    }
//                    switch(Cmd_Type)
//                    {
////                            //SCI总线重启
////                            case Cmd_Restart:
////
////                                break;
////                            //读寄存器命令
////                            case Cmd_ReadReg:
////                                Read_Reg_Addr = (uint16_t)(rxBuf_SCIB[i + 16] << 8) | rxBuf_SCIB[i + 17];
////                                SCI_Send_Data(Read_Reg_Addr);
////                                break;
////                            //写寄存器命令
////                            case Cmd_WriteReg:
////                                Data_buf = 0;
////                                //这里临时存储相关的数据信息
////                                Data_buf += ((Uint32)rxBuf_SCIB[i + 18] << 24);
////                                Data_buf += ((Uint32)rxBuf_SCIB[i + 19] << 16);
////                                Data_buf += ((Uint32)rxBuf_SCIB[i + 20] << 8);
////                                Data_buf += ((Uint32)rxBuf_SCIB[i + 21] & 0xff);
////
//////                                Read_Reg_Addr = (uint16_t)(rxBuf_SCIB[i + 16] << 8) | rxBuf_SCIB[i + 17];
//////                                SCI_Send_Data(Read_Reg_Addr);
////                                break;
//
//                        case Cmd_Tele:
//                            if(crc16 == crc16_send)
//                            {
//                                Reply_TELE_UART(rxBuf_SCIB[i + 16]);
//                            }
//                            break;
//                        case Cmd_Control:
//                        case Cmd_Betting:
//                            //如果校验通过
//                            if(crc16 == crc16_send)
//                            {
//                                Reply_Bet_UART(Cmd_Type ,1);
//                            }
//                            else
//                                Reply_Bet_UART(Cmd_Type ,0);
//                            break;
//                    }
                    //校验通过，进入数据处理

                }
            }
        }

        Rec_Succ_SCIB = 0;
    }

}

void Reply_TELE_UART(unsigned char data)
{
    uint16_t  i,crc16;
    uint32_t CAN_SEND_TELE_NUM = 84;
    unsigned char Data_Send_u8[320], sum_data[280];
    //初始化数组

    switch(data)
    {
        case 0xff:
            CAN_SEND_TELE_NUM = 84;
            break;
        case 0x1f:
            CAN_SEND_TELE_NUM = 128;
            break;
        case 0x2f:
            CAN_SEND_TELE_NUM = 255;
            break;
    }
    Data_Send_u8[0] = 0xD2;
    Data_Send_u8[1] = 0x8C;
    Data_Send_u8[2] = 0x00;
    Data_Send_u8[3] = 0x0A;
    Data_Send_u8[4] = 0xFF;
    Data_Send_u8[5] = 0xFF;
    Data_Send_u8[6] = 0xFF;
    Data_Send_u8[7] = 0xFF;
    Data_Send_u8[8] = 0xFF;
    Data_Send_u8[9] = 0xFF;
    Data_Send_u8[10] = 0xFF;
    Data_Send_u8[11] = 0x00;
    Data_Send_u8[12] = 0x23;
    Data_Send_u8[13] = 0xFF;
    Data_Send_u8[14] = (CAN_SEND_TELE_NUM >> 8);
    Data_Send_u8[15] = CAN_SEND_TELE_NUM & 0x00ff;
    for(i = 0; i < CAN_SEND_TELE_NUM; i++)
    {
        Data_Send_u8[16 + i] = YC_Data[i + 3];
    }
    crc16 = crc16_ccitt_false(&Data_Send_u8[16], CAN_SEND_TELE_NUM);
    Data_Send_u8[16 + CAN_SEND_TELE_NUM] = (unsigned char)(crc16 >> 8);
    Data_Send_u8[16 + CAN_SEND_TELE_NUM + 1] = (unsigned char)(crc16 & 0x00ff);
    SCI_writeCharArray(SCIA_BASE, Data_Send_u8, (18 + CAN_SEND_TELE_NUM));
}


void Reply_TELE_UART_REVERSE(unsigned char data)
{
    uint16_t  i,crc16;
    uint32_t CAN_SEND_TELE_NUM = 84;
    unsigned char Data_Send_u8[320], sum_data[280];
    //初始化数组

    switch(data)
    {
        case 0xff:
            CAN_SEND_TELE_NUM = 84;
            break;
        case 0x1f:
            CAN_SEND_TELE_NUM = 128;
            break;
        case 0x2f:
            CAN_SEND_TELE_NUM = 255;
            break;
    }
    Data_Send_u8[0] = 0xD2;
    Data_Send_u8[1] = 0x8C;
    Data_Send_u8[2] = 0x00;
    Data_Send_u8[3] = 0x0A;
    Data_Send_u8[4] = 0xFF;
    Data_Send_u8[5] = 0xFF;
    Data_Send_u8[6] = 0xFF;
    Data_Send_u8[7] = 0xFF;
    Data_Send_u8[8] = 0xFF;
    Data_Send_u8[9] = 0xFF;
    Data_Send_u8[10] = 0xFF;
    Data_Send_u8[11] = 0x00;
    Data_Send_u8[12] = 0x23;
    Data_Send_u8[13] = 0xFF;
    Data_Send_u8[14] = (CAN_SEND_TELE_NUM >> 8);
    Data_Send_u8[15] = CAN_SEND_TELE_NUM & 0x00ff;
    for(i = 0; i < CAN_SEND_TELE_NUM; i++)
    {
        Data_Send_u8[16 + i] = YC_Data[83 - i];
    }
    crc16 = crc16_ccitt_false(&Data_Send_u8[16], CAN_SEND_TELE_NUM);
    Data_Send_u8[16 + CAN_SEND_TELE_NUM] = (unsigned char)(crc16 >> 8);
    Data_Send_u8[16 + CAN_SEND_TELE_NUM + 1] = (unsigned char)(crc16 & 0x00ff);
    SCI_writeCharArray(SCIA_BASE, Data_Send_u8, (18 + CAN_SEND_TELE_NUM));
}

void Reply_Bet_UART(uint16_t cmd,unsigned char crc_if)
{
    unsigned char Data_Send_u8[320];
    uint16_t crc16;
    Data_Send_u8[0] = 0xD2;
    Data_Send_u8[1] = 0x8C;
    Data_Send_u8[2] = 0x00;
    Data_Send_u8[3] = 0x0A;
    Data_Send_u8[4] = 0xFF;
    Data_Send_u8[5] = 0xFF;
    Data_Send_u8[6] = 0xFF;
    Data_Send_u8[7] = 0xFF;
    Data_Send_u8[8] = 0xFF;
    Data_Send_u8[9] = 0xFF;
    Data_Send_u8[10] = 0xFF;
    Data_Send_u8[11] = 0x00;
    if(cmd == Cmd_Betting)
    {
        Data_Send_u8[12] = 0x19;
    }
    if(cmd == Cmd_Control)
    {
        Data_Send_u8[12] = 0x0F;
    }
    Data_Send_u8[13] = 0xFF;
    Data_Send_u8[14] = 0x00;
    Data_Send_u8[15] = 0x04;
    Data_Send_u8[16] = 0x00;
    Data_Send_u8[17] = 0x00;
    if(crc_if == 1)
    {
        Data_Send_u8[18] = 0xAA;
        Data_Send_u8[19] = 0xAA;
    }
    else
    {
        Data_Send_u8[18] = 0xFF;
        Data_Send_u8[19] = 0xFF;
    }
    crc16 = crc16_ccitt_false(&Data_Send_u8[16], 4);
    Data_Send_u8[20] = (unsigned char)(crc16 >> 8);
    Data_Send_u8[21] = (unsigned char)(crc16 & 0x00ff);
    SCI_writeCharArray(SCIA_BASE, Data_Send_u8, 22);
}


//crc16相关代码
uint16_t crc16_ccitt_false(const unsigned char *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    uint16_t i, j;
    for (i = 0; i < length; i++)
    {
        crc ^= (uint16_t) data[i] << 8;

        for (j = 0; j < 8; j++)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ 0x1021;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}


