/*
 * CAN_User.c
 *
 *  Created on: 2025年11月25日
 *      Author: pc
 */
#include "CAN_User.h"
unsigned char YC_Data[100];

uint32_t txMsgCount = 0;
uint32_t rxMsgCount = 0;
uint32_t errorFlag = 0;
uint16_t txMsgData[8];
uint16_t rxMsgData[8];

void CAN_Init(void)
{
    union CAN_EXTID CAN_rx_Extid;
//    GPIO_setPinConfig(GPIO_30_CANA_RX);
//    GPIO_setPinConfig(GPIO_31_CANA_TX);
    GPIO_setPinConfig(GPIO_31_CANA_TX);
    GPIO_setPinConfig(GPIO_30_CANA_RX);

    //GPIO_setPinConfig(GPIO_30_CANA_RX);
//    GPIO_setDirectionMode(30, GPIO_DIR_MODE_IN);
//    GPIO_setPadConfig(30, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(30, GPIO_QUAL_ASYNC);


    //GPIO_setPinConfig(GPIO_31_CANA_TX);
//    GPIO_setDirectionMode(31, GPIO_DIR_MODE_OUT);
//    GPIO_setPadConfig(31, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(31, GPIO_QUAL_ASYNC);

    Interrupt_register(INT_CANA0, &canISR);
    //
    // Initialize the CAN controller
    //
    CAN_initModule(CANA_BASE);

    //
    // Set up the CAN bus bit rate to 500kHz
    // Refer to the Driver Library User Guide for information on how to set
    // tighter timing control. Additionally, consult the device data sheet
    // for more information about the CAN module clocking.
    //
    CAN_setBitRate(CANA_BASE, DEVICE_SYSCLK_FREQ / 5, 500000, 20);
    //
    // Enable interrupts on the CAN peripheral.
    //
    Interrupt_enable(INT_CANA0);
    CAN_enableInterrupt(CANA_BASE, CAN_INT_IE0 | CAN_INT_ERROR |
                        CAN_INT_STATUS);

    CAN_enableGlobalInterrupt(CANA_BASE, CAN_GLOBAL_INT_CANINT0);
    CAN_clearGlobalInterruptStatus(CANA_BASE, CAN_GLOBAL_INT_CANINT0);

    //
    // Enable CAN test mode with external loopback
    //
    //CAN_enableTestMode(CANA_BASE, CAN_TEST_EXL);

    //
    // Initialize the transmit message object used for sending CAN messages.
    // Message Object Parameters:
    //      Message Object ID Number: 1
    //      Message Identifier: 0x1
    //      Message Frame: Standard
    //      Message Type: Transmit
    //      Message ID Mask: 0x0
    //      Message Object Flags: Transmit Interrupt
    //      Message Data Length: 4 Bytes
    //
    CAN_rx_Extid.extid.Addr = 0x10;
    CAN_rx_Extid.extid.Data_Type = 0x07;
    CAN_rx_Extid.extid.Farme_cnt = 0x30;
    CAN_rx_Extid.extid.Farme_flag = 0x3;
    CAN_rx_Extid.extid.LT = 1;
    CAN_rx_Extid.extid.S_addr = 0x08;
    CAN_rx_Extid.extid.Priority = 0x4;
//
    CAN_setupMessageObject(CANA_BASE, TX_MSG_OBJ_ID, CAN_rx_Extid.data_all, CAN_MSG_FRAME_EXT,
                           CAN_MSG_OBJ_TYPE_TX, 0, CAN_MSG_OBJ_TX_INT_ENABLE,
                           MSG_DATA_LENGTH);
    CAN_setupMessageObject(CANA_BASE, RX_MSG_OBJ_ID, 0x1, CAN_MSG_FRAME_EXT,
                               CAN_MSG_OBJ_TYPE_RX, 0x0000000, (CAN_MSG_OBJ_RX_INT_ENABLE |
                                       CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER),
                               MSG_DATA_LENGTH);
    CAN_startModule(CANA_BASE);

}
uint16_t Rx_LEN;
__interrupt void canISR(void)
{
    uint32_t status;
    unsigned char msg_sum;
    uint16_t msg_count = 0, i, Rx_DLC;
    union CAN_EXTID CAN_rx_Extid;
    CAN_MsgFrameType CAN_RX_TYPE = CAN_MSG_FRAME_EXT;
    //
    // Read the CAN interrupt status to find the cause of the interrupt
    //
    status = CAN_getInterruptCause(CANA_BASE);

    //
    // If the cause is a controller status interrupt, then get the status
    //
    if(status == CAN_INT_INT0ID_STATUS)
    {
        //
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors.  Error processing
        // is not done in this example for simplicity.  Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.
        //
        status = CAN_getStatus(CANA_BASE);
        if(((status  & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 7) &&
           ((status  & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 0))
        {
            //
            // Set a flag to indicate some errors may have occurred.
            //
            errorFlag = 1;
        }
    }

    //
    // Check if the cause is the transmit message object 1
    //
    else if(status == TX_MSG_OBJ_ID)
    {

        CAN_clearInterruptStatus(CANA_BASE, TX_MSG_OBJ_ID);


        errorFlag = 0;
    }

    //
    // Check if the cause is the receive message object 2
    //
    else if(status == RX_MSG_OBJ_ID)
    {
        //Send_YC();
        //
        // Get the received message
        //
        //CAN_readMessage(CANA_BASE, RX_MSG_OBJ_ID, rxMsgData);
        CAN_readMessageWithID(CANA_BASE, RX_MSG_OBJ_ID, CAN_RX_TYPE, &CAN_rx_Extid.data_all, rxMsgData);
        //
        Rx_DLC = Get_DLC();
        msg_count = CAN_rx_Extid.extid.Farme_cnt;
        switch(CAN_rx_Extid.extid.Farme_flag)
        {
            case 0x00:
            case 0x01:
                if(msg_count == 1)
                {
                    Rx_LEN = (rxMsgData[0] << 8) + rxMsgData[1];
                    for(i = 0; i < 6; i++)
                    {
                        YC_Data[i] = rxMsgData[i];
                    }
                }
                break;
            case 0x02:
                for(i = 0; i < 8; i++)
                {
                    YC_Data[(msg_count - 2) * 8 + i + 8] = rxMsgData[i];
                }
                break;
            case 0x03:
                for(i = 0; i < Rx_DLC - 1; i++)
                {
                    YC_Data[(msg_count - 2) * 8 + i + 8] = rxMsgData[i];
                }
                msg_sum = Check_Sum(YC_Data , Rx_LEN + 1);
                if(rxMsgData[Rx_DLC - 1] == msg_sum)
                {
                    Reply_TELE_UART(0xff);
                }
                break;
        }
        // Getting to this point means that the RX interrupt occurred on
        // message object 2, and the message RX is complete.  Clear the
        // message object interrupt.
        //
        CAN_clearInterruptStatus(CANA_BASE, RX_MSG_OBJ_ID);

        //
        // Increment a counter to keep track of how many messages have been
        // received. In a real application this could be used to set flags to
        // indicate when a message is received.
        //
        rxMsgCount++;

        //
        // Since the message was received, clear any error flags.
        //
        errorFlag = 0;
    }

    //
    // If something unexpected caused the interrupt, this would handle it.
    //
    else
    {
        //
        // Spurious interrupt handling can go here.
        //
    }

    //
    // Clear the global interrupt flag for the CAN interrupt line
    //
    CAN_clearGlobalInterruptStatus(CANA_BASE, CAN_GLOBAL_INT_CANINT0);

    //
    // Acknowledge this interrupt located in group 9
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

void farme_process(void)
{

}

//该校验和为SUM=(DT+W0+W1)%256；
unsigned char Check_Sum(unsigned char * data, unsigned char num)
{
    uint16_t sum = 0, i;
    for(i = 0; i < num; i++)
    {
        sum += data[i];
    }
    return (sum % 256);
}

unsigned char Get_DLC(void)
{
    uint32_t msgCtrl;
    msgCtrl = HWREGH(CANA_BASE + CAN_O_IF2MCTL);
    return (msgCtrl & CAN_IF2MCTL_DLC_M);
}

void Send_YK(uint16_t cmd)
{
    union CAN_EXTID CAN_rx_Extid;
    uint16_t Send_data[8], i;
    for(i = 0 ; i < 8; i++)
    {
        Send_data[i] = 0;
    }
    Send_data[0] = cmd;
    CAN_rx_Extid.data_all = 0;
    CAN_rx_Extid.extid.Addr = 0x10;
    CAN_rx_Extid.extid.Data_Type = 0x02;
    CAN_rx_Extid.extid.Farme_cnt = 0x01;
    CAN_rx_Extid.extid.Farme_flag = 0x0;
    CAN_rx_Extid.extid.LT = 0;
    CAN_rx_Extid.extid.S_addr = 0x00;
    CAN_rx_Extid.extid.Priority = 0x3;
//
    CAN_setupMessageObject(CANA_BASE, TX_MSG_OBJ_ID, CAN_rx_Extid.data_all, CAN_MSG_FRAME_EXT,
                           CAN_MSG_OBJ_TYPE_TX, 0, CAN_MSG_OBJ_TX_INT_ENABLE,
                           MSG_DATA_LENGTH);

    CAN_sendMessage_updateDLC(CANA_BASE, TX_MSG_OBJ_ID, 8, Send_data);
}


void Send_YC(void)
{
    union CAN_EXTID CAN_rx_Extid;
    uint16_t Send_data[8], i;
    for(i = 0 ; i < 8; i++)
    {
        Send_data[i] = 0;
    }
    CAN_rx_Extid.data_all = 0;
    Send_data[0] = 0xFF;
    CAN_rx_Extid.extid.Addr = 0x10;
    CAN_rx_Extid.extid.Data_Type = 0x00;
    CAN_rx_Extid.extid.Farme_cnt = 0x01;
    CAN_rx_Extid.extid.Farme_flag = 0x0;
    CAN_rx_Extid.extid.LT = 0;
    CAN_rx_Extid.extid.S_addr = 0x00;
    CAN_rx_Extid.extid.Priority = 0x3;
//
    CAN_setupMessageObject(CANA_BASE, TX_MSG_OBJ_ID, CAN_rx_Extid.data_all, CAN_MSG_FRAME_EXT,
                           CAN_MSG_OBJ_TYPE_TX, 0, CAN_MSG_OBJ_TX_INT_ENABLE,
                           MSG_DATA_LENGTH);

    CAN_sendMessage_updateDLC(CANA_BASE, TX_MSG_OBJ_ID, 8, Send_data);
}

//
////回复寄存器数据函数
//void Reply_Read_REG(unsigned char * data)
//{
//    Uint16 Reg_Addr;
//    Uint32 Data_Send[2] = {0}, Send_ID;
//    union CAN_EXTID ExtId;
//    unsigned char Reg_data[4], Data_Send_u8[12], sum_data[7], i;
//    //初始化数组
//    for(i = 0; i < 12; i++)
//    {
//        Data_Send_u8[i] = 0;
//    }
//    if((data[0] == 2) && (data[1] == 0x2F))
//    {
//        //读取寄存器地址
//        Reg_Addr = (data[2] << 8) + data[3];
//        //读取寄存器内容，这里需要修改
//        Reg_data[0] = (unsigned char)((Wr_Data_CAN >> 24) & 0xff);
//        Reg_data[1] = (unsigned char)((Wr_Data_CAN >> 16) & 0xff);
//        Reg_data[2] = (unsigned char)((Wr_Data_CAN >> 8) & 0xff);
//        Reg_data[3] = (unsigned char)(Wr_Data_CAN & 0xff);
//
//        //发送ID
//        ExtId.extid.LT = BUS_CAN_B;                 //CAN总线类型
//        ExtId.extid.MT = CAN_READ_REG_R;            //命令种类
//        ExtId.extid.DT = CAN_DT_READ_REG_R;         //数据种类
//        ExtId.extid.S_ADDr = CAN_ID;                //信息源地址
//        ExtId.extid.ADDr = 0;                       //信息接受地址
////        Send_ID = ((Uint32)BUS_CAN_A << 28);
////        Send_ID += ((Uint32)CAN_READ_REG_R << 24);
////        Send_ID += ((Uint32)0x3F << 16);
////        Send_ID += ((Uint32)CAN_ID << 8);
//        //发送数据
//        Data_Send_u8[0] = 0;                //Seq 帧序号
//        Data_Send_u8[1] = 0x06;             //有效字节
//        Data_Send_u8[2] = 0x3F;             //数据类型
//        Data_Send_u8[3] = data[2];          //寄存器地址
//        Data_Send_u8[4] = data[3];          //寄存器地址
//        Data_Send_u8[5] = Reg_data[0];      //寄存器内容
//        Data_Send_u8[6] = Reg_data[1];      //寄存器内容
//        Data_Send_u8[7] = Reg_data[2];      //寄存器内容
//        Data_Send_u8[8] = 1;                //Seq 帧序号，这里为第二帧
//        Data_Send_u8[9] = Reg_data[3];      //寄存器内容
//
//        //校验和所需要的数据
//        sum_data[0] = 0x3F;                 //0x3F为数据种类
//        for(i = 0; i < 5; i++)
//        {
//            sum_data[1 + i] = Data_Send_u8[3 + i];      //写入第一帧所需要校验和的数据
//        }
//        sum_data[6] = Reg_data[3];
//        Data_Send_u8[10] = Check_Sum(sum_data, 7);      //寄存器内容
//
//        //将第一帧所需要的8位变成两个32位
//        Data_Send[0] = UINT8_TO_UINT32(&Data_Send_u8[0]);
//        Data_Send[1] = UINT8_TO_UINT32(&Data_Send_u8[4]);
//
//        eCanSendData(15, 8, ExtId.data_all, Data_Send);
//        //这里做一些延时，防止两帧过近
//        DELAY_US(500);
//
//        //发送第二帧
//        Data_Send[0] = UINT8_TO_UINT32(&Data_Send_u8[8]);
//        eCanSendData(15, 3, ExtId.data_all, Data_Send);
//    }
//}

//void Reply_TELE(unsigned char * data)
//{
//    Uint16 Reg_Addr, i, j;
//    Uint32 Data_Send[2] = {0}, Send_ID;
//    Uint32 CAN_SEND_TELE_NUM = 60;
//    union CAN_EXTID ExtId;
//    Uint16 cmd_tele = 0, line_num;
//    unsigned char Reg_data[4], Data_Send_u8[320], sum_data[280];
//    //初始化数组
//    for(i = 0; i < 280; i++)
//    {
//        Data_Send_u8[i] = 0;
//        sum_data[i] = i & 0xff;
//    }
//    sum_data[4] = 0x61;
//    sum_data[5] = 0xA8;
//    sum_data[20] = 0X76;
//    sum_data[21] = 0x0E;
//    sum_data[22] = 0x76;
//    sum_data[23] = 0x0E;
//    sum_data[24] = 0x76;
//    sum_data[25] = 0x7D;
//    sum_data[26] = 0x73;
//    sum_data[27] = 0xD7;
//    sum_data[28] = 0x73;
//    sum_data[29] = 0x73;
//    if((data[0] == 0x03) && (data[1] == 0x5F))
//    {
//        //
//        switch(data[2])
//        {
//            case 0xff:
//                CAN_SEND_TELE_NUM = 60;
//                break;
//            case 0x1f:
//                CAN_SEND_TELE_NUM = 128;
//                break;
//            case 0x2f:
//                CAN_SEND_TELE_NUM = 255;
//                break;
//        }
//        //
//        if((data[3] == 0xA1) && (data[4] == 0xB2))
//        {
//            tele_cmd_count++;
//            //认证通过，进行处理
//            cmd_tele = data[2];
////            switch(data[2])
////            {
////                case 0xFF:
//                    ExtId.extid.LT = BUS_CAN_B;                 //CAN总线类型
//                    ExtId.extid.MT = CAN_SEND_TELE;            //命令种类
//                    ExtId.extid.DT = CAN_DT_SEND_TELE;         //数据种类
//                    ExtId.extid.S_ADDr = CAN_ID;                //信息源地址
//                    ExtId.extid.ADDr = 0;                       //信息接受地址
//                    Data_Send_u8[0] = 0x00;
//                    Data_Send_u8[1] = 0x00;
//                    Data_Send_u8[2] = 0xFF;
//                    Data_Send_u8[3] = data[2];
//                    Data_Send_u8[4] = tele_cmd_count;
//                    Data_Send_u8[5] = (unsigned char)(((CMD_count & 0x0f) << 4) + (Err_CMD_count & 0x0f));
//                    Data_Send_u8[6] = 0x00;
//                    Data_Send_u8[7] = 0x00;
//                    line_num = CAN_SEND_TELE_NUM / 7;
//                    for(i = 0; i < line_num; i++)
//                    {
//                        Data_Send_u8[i * 8] = i;
//
//                        for(j = 0; j < 7; j++)
//                        {
//                            Data_Send_u8[i * 8 + 9 + j] = sum_data[i * 7 + j];
//                        }
//                    }
//                    Data_Send_u8[i * 8] = i;
//                    Data_Send_u8[i * 8 + 8] = i + 1;
//                    for(j = 0; j < (CAN_SEND_TELE_NUM % 7); j++)
//                    {
//                        Data_Send_u8[i * 8 + 9 + j] = sum_data[i * 7 + j];
//                    }
//                    Data_Send_u8[i * 8 + 9 + CAN_SEND_TELE_NUM % 7] = Check_Sum(sum_data, CAN_SEND_TELE_NUM);
//                    for(i = 0; i < (CAN_SEND_TELE_NUM / 7 + 1); i++)
//                    {
//
//                        Data_Send[0] = UINT8_TO_UINT32(&Data_Send_u8[i * 8]);
//                        Data_Send[1] = UINT8_TO_UINT32(&Data_Send_u8[i * 8 + 4]);
//
//                        eCanSendData(15, 8, ExtId.data_all, Data_Send);
//                        //这里做一些延时，防止两帧过近
//                        DELAY_US(500);
//                    }
//                    Data_Send[0] = UINT8_TO_UINT32(&Data_Send_u8[i * 8]);
//                    Data_Send[1] = UINT8_TO_UINT32(&Data_Send_u8[i * 8 + 4]);
//
//                    //最后一帧
//                    eCanSendData(15, (CAN_SEND_TELE_NUM % 7 + 2), ExtId.data_all, Data_Send);
////                    break;
////            }
//        }
//        else
//            Err_CMD_count++;
//    }
//}


//void Reply_CONTROL(unsigned char * data)
//{
//    Uint16 Reg_Addr, Con_checksum;
//    Uint32 Data_Send[2] = {0}, Send_ID;
//    static Uint16 Zs_Index = 0;
//    static Uint16 ZS_COM1, ZS_COM2, ZS_COM3;
//    union CAN_EXTID ExtId;
//    static Uint16 CAN_SEND_ZS_NUM = 21;
//    Uint16 Tele_CMD;
//    unsigned char Reg_data[4], Data_Send_u8[12], sum_data[7], i;
//    //初始化数组
//    for(i = 0; i < 12; i++)
//    {
//        Data_Send_u8[i] = 0;
//    }
//    //先确认第一帧，后续有该标志位直接接收数据
//    if(Zs_flag == 0)
//    {
//        Zs_count = 0;
//        Zs_Index = 0;
//        //先判断芯片地址  //
//        if(data[0] == 0xA0)
//        {
//
//            Tele_CMD = (data[2] << 8) + data[3];
//            ZS_COM3 = data[1];
//            ZS_COM2 = data[2];
//            ZS_COM1 = data[3];
//            //判断是否为间接指令
//            if(data[1] == 0x8F)
//            {
//
//                switch(Tele_CMD)
//                {
//                    case 0x0001:
//                        break;
//                    case 0x0002:
//                        break;
//                }
//                Con_checksum = Check_Sum(&data[0], 4);
//                if(Con_checksum == data[4])
//                {
//                    Zs_flag = 2;
//                }
//                else
//                {
//                    Zs_flag = 4;
//                    Err_CMD_count++;
//                }
//            }
//            //判断是否为注数指令
//            if((data[1] <= 0x7F) && (data[1] >= 0x1F))
//            {
//                switch(ZS_COM3)
//                {
//                    case 0x2F:
//                        CAN_SEND_ZS_NUM = 21;
//                        break;
//                    case 0x38:
//                        CAN_SEND_ZS_NUM = 32;
//                        break;
//                    case 0x1F:
//                        CAN_SEND_ZS_NUM = 56;
//                        break;
//                }
//                switch(Tele_CMD)
//                {
//                    case 0x0001:
//                        break;
//                }
//                if((data[4] == 0x68) && (data[5] == 0xC8))
//                {
//                    Zs_flag = 1;
//                    Zs_Data[0] = data[6];
//                    Zs_Data[1] = data[7];
//                    return;
//                }
//            }
//        }
//    }
//    if(Zs_flag == 1)
//    {
//        Zs_count++;
//        for(i = 0; i < 8; i++)
//        {
//            Zs_Data[2 + i + 8 * Zs_Index] = data[i];
//        }
//        Zs_Index++;
//        if(Zs_count >= ((CAN_SEND_ZS_NUM - 2) / 8) + 1)
//        {
//            Con_checksum = Check_Sum(Zs_Data, CAN_SEND_ZS_NUM);
//            if(Zs_Data[CAN_SEND_ZS_NUM] == Con_checksum)
//            {
//                CMD_count++;
//                Zs_flag = 2;
//            }
//            else
//            {
//                Err_CMD_count++;
//                Zs_flag = 4;
//            }
//        }
//    }
//    if(Zs_flag >= 2)
//    {
//
//        ExtId.extid.LT = BUS_CAN_B;                 //CAN总线类型
//        ExtId.extid.MT = CAN_SEND_CONTROL;            //命令种类
//        ExtId.extid.DT = CAN_DT_SEND_CONTROL;         //数据种类
//        ExtId.extid.S_ADDr = CAN_ID;                //信息源地址
//        ExtId.extid.ADDr = 0;                       //信息接受地址
//        Data_Send_u8[0] = 0x05;
//        Data_Send_u8[1] = CAN_DT_SEND_CONTROL;
//        Data_Send_u8[2] = ZS_COM3;
//        Data_Send_u8[3] = ZS_COM2;
//        Data_Send_u8[4] = ZS_COM1;
//        Data_Send_u8[5] = Zs_flag - 2;
//        Data_Send_u8[6] = 0x00;
//        Data_Send_u8[7] = Check_Sum(&Data_Send_u8[1], 6);
//
//        Data_Send[0] = UINT8_TO_UINT32(&Data_Send_u8[0]);
//        Data_Send[1] = UINT8_TO_UINT32(&Data_Send_u8[4]);
//
//        //最后一帧
//        eCanSendData(15, 8, ExtId.data_all, Data_Send);
//        Zs_count = 0;
//        Zs_Index = 0;
//        Zs_flag = 0;
//    }
//
//}



//8位转化位32位
uint32_t UINT8_TO_UINT32(unsigned char * data)
{
    uint32_t ret_data = 0;
    ret_data += (uint32_t)data[0] << 24;
    ret_data += (uint32_t)data[1] << 16;
    ret_data += data[2] << 8;
    ret_data += data[3];
    return ret_data;
}
