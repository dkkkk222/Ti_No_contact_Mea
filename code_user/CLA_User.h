/*
 * CLA_User.h
 *
 *  Created on: 2025年9月12日
 *      Author: pc
 */

#ifndef CODE_USER_CLA_USER_H_
#define CODE_USER_CLA_USER_H_



#ifdef __cplusplus
extern "C" {
#endif

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
//#include "ePWM_User.h"

//
// Defines
//

#define myCLA0_BASE CLA1_BASE


//
// Globals
//
//Task 1 (C) Variables

//Task 2 (C) Variables

//Task 3 (C) Variables

//Task 4 (C) Variables

//Task 5 (C) Variables

//Task 6 (C) Variables

//Task 7 (C) Variables

//Task 8 (C) Variables

//Common (C) Variables


//#pragma DATA_SECTION(gClaToCpu, "Cla1ToCpuMsgRAM")

//volatile struct {
//float x; // 输入/参数（示例）
//    uint16_t cmd; // 命令/控制位
//    uint16_t seq; // 递增序列号(握手)
//} gCpuToCla;

//volatile struct {
//float y; // 结果
//    uint16_t done; // 完成标志
//    uint16_t seq; // 回传的序列号
//} gClaToCpu;



// Function Prototypes
//
__attribute__((interrupt))  void Cla1Task1();
__attribute__((interrupt))  void Cla1Task2();
__attribute__((interrupt))  void Cla1Task3();
__attribute__((interrupt))  void Cla1Task4();
__attribute__((interrupt))  void Cla1Task5();
__attribute__((interrupt))  void Cla1Task6();
__attribute__((interrupt))  void Cla1Task7();
__attribute__((interrupt))  void Cla1Task8();


void CLA_Init();
void CLA_init_config();
void MEMCFG_init();
void initCLA(void);
void setupProfileGpio(void);



#ifdef __cplusplus
}
#endif // extern "C"

//
// End of File
//




#endif /* CODE_USER_CLA_USER_H_ */
