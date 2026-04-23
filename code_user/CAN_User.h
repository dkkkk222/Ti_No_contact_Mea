/*
 * CAN_User.h
 *
 *  Created on: 2025年11月25日
 *      Author: pc
 */

#ifndef CODE_USER_CAN_USER_H_
#define CODE_USER_CAN_USER_H_

#include "driverlib.h"
#include "device.h"

struct ExtId
{
    //uint32_t                :3;
    uint32_t Farme_cnt      :8;     //帧计数
    uint32_t Farme_flag     :2;     //单/复帧标识 00：单帧 01：多帧的第一帧 10：多帧的中间帧 11：多帧的最后一帧
    uint32_t S_addr         :5;     //源地址
    uint32_t Addr           :5;     //目的地址
    uint32_t Data_Type      :5;     //数据类型
    uint32_t LT             :1;     //总线标识
    uint32_t Priority       :3;     //优先级
};

union CAN_EXTID
{
    uint32_t data_all;
    struct ExtId extid;
};


#define MSG_DATA_LENGTH    8
#define TX_MSG_OBJ_ID      1
#define RX_MSG_OBJ_ID      2


extern uint32_t txMsgCount;
extern uint32_t rxMsgCount;
extern uint32_t errorFlag;
extern uint16_t txMsgData[8];
extern uint16_t rxMsgData[8];

extern unsigned char YC_Data[100];

void CAN_Init(void);
__interrupt void canISR(void);
void Send_YK(uint16_t cmd);
void Send_YC(void);
#endif /* CODE_USER_CAN_USER_H_ */
