/*
 * SPI_User.h
 *
 *  Created on: 2025Äê9ÔÂ9ÈÕ
 *      Author: pc
 */

#ifndef CODE_USER_SPI_USER_H_
#define CODE_USER_SPI_USER_H_

#include "driverlib.h"
#include "device.h"


#define Volt_Ratio (205 / 5)



//
// SPIA_PICO - GPIO Settings
//
//#define GPIO_PIN_SPIA_PICO 16
//#define SPIA_peripheral_SPIPICO_GPIO 16
//#define SPIA_peripheral_SPIPICO_PIN_CONFIG GPIO_16_SPIA_SIMO
////
//// SPIA_POCI - GPIO Settings
////
//#define GPIO_PIN_SPIA_POCI 17
//#define SPIA_peripheral_SPIPOCI_GPIO 17
//#define SPIA_peripheral_SPIPOCI_PIN_CONFIG GPIO_17_SPIA_SOMI
////
//// SPIA_CLK - GPIO Settings
////
//#define GPIO_PIN_SPIA_CLK 18
//#define SPIA_peripheral_SPICLK_GPIO 18
//#define SPIA_peripheral_SPICLK_PIN_CONFIG GPIO_18_SPIA_CLK
////
//// SPIA_PTE - GPIO Settings
////
//#define GPIO_PIN_SPIA_PTE 19
//#define SPIA_peripheral_SPIPTE_GPIO 19
//#define SPIA_peripheral_SPIPTE_PIN_CONFIG GPIO_19_SPIA_STE

#define GPIO_PIN_SPIA_PICO 8
#define SPIA_peripheral_SPIPICO_GPIO 8
#define SPIA_peripheral_SPIPICO_PIN_CONFIG GPIO_8_SPIA_SIMO
//
// SPIA_POCI - GPIO Settings
//
#define GPIO_PIN_SPIA_POCI 10
#define SPIA_peripheral_SPIPOCI_GPIO 10
#define SPIA_peripheral_SPIPOCI_PIN_CONFIG GPIO_10_SPIA_SOMI
//
// SPIA_CLK - GPIO Settings
//
#define GPIO_PIN_SPIA_CLK 9
#define SPIA_peripheral_SPICLK_GPIO 9
#define SPIA_peripheral_SPICLK_PIN_CONFIG GPIO_9_SPIA_CLK
//
// SPIA_PTE - GPIO Settings
//
#define GPIO_PIN_SPIA_PTE 11
#define SPIA_peripheral_SPIPTE_GPIO 11
#define SPIA_peripheral_SPIPTE_PIN_CONFIG GPIO_11_SPIA_STE

// Interrupt Settings for INT_SPIA_peripheral_RX
// ISR need to be defined for the registered interrupts
#define INT_SPIA_peripheral_RX INT_SPIA_RX
#define INT_SPIA_peripheral_RX_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP6
extern __interrupt void spiaRxFIFOISR(void);

// Interrupt Settings for INT_SPIA_peripheral_TX
// ISR need to be defined for the registered interrupts
#define INT_SPIA_peripheral_TX INT_SPIA_TX
#define INT_SPIA_peripheral_TX_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP6
extern __interrupt void INT_SPIA_peripheral_TX_ISR(void);

extern int AD_Rev;
extern int Get_AD;

#define SPIA_peripheral_BASE SPIA_BASE
#define SPIA_peripheral_BITRATE   1000000
#define SPIA_peripheral_DATAWIDTH 16
void SPIA_peripheral_init();





void SPI_Init(void);
void SPIA_GPIO_Init(void);
void SPIA_peripheral_init(void);
void INT_spi_init();
void SPI_send_data(uint8_t *data, uint16_t length);
void get_AD(uint16_t input_data);




#endif /* CODE_USER_SPI_USER_H_ */
