/*
 * SCI_User.h
 *
 *  Created on: 2025年11月26日
 *      Author: pc
 */

#ifndef CODE_USER_SCI_USER_H_
#define CODE_USER_SCI_USER_H_

#include "driverlib.h"
#include "device.h"

#define UART_Baud  115200

#define RX_BUF_SIZE 300      //SCI最长数据


#define Cmd_Restart     0x0000
#define Cmd_ReadReg     0x000A
#define Cmd_WriteReg    0x000F
#define Cmd_SendReg     0x0014

#define Cmd_Control         0x000A
#define Cmd_Reply_Control   0x000F
#define Cmd_Betting         0x0014
#define Cmd_Reply_Bet       0x0019
#define Cmd_Tele            0x001E
#define Cmd_Reply_Tele      0x0023

extern unsigned char rx_count_SCIB;
extern unsigned char Rec_Succ_SCIB;
extern unsigned char Rec_Flag_SCIB;

extern unsigned char rxBuf_SCIA[RX_BUF_SIZE];
extern unsigned char rx_count_SCIA;
extern unsigned char Rec_Succ_SCIA;
extern unsigned char Rec_Flag_SCIA;

extern unsigned char YC_Data[100];

extern uint16_t SCI_OUTTIME_A;
extern uint16_t SCI_OUTTIME_B;
void SCIB_Init(void);
__interrupt void scibRXFIFOISR(void);
__interrupt void sciaRXFIFOISR(void);
__interrupt void sciaTXFIFOISR(void);
void SCI_Init(void);
void Reply_TELE_UART(unsigned char data);
void process_frame_SCIA(void);

#endif /* CODE_USER_SCI_USER_H_ */
