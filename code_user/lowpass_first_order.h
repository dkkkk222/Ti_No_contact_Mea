/*
 * lowpass_first_order.h
 *
 *  Created on: 2025Äê9ÔÂ11ÈÕ
 *      Author: pc
 */

#ifndef CODE_USER_LOWPASS_FIRST_ORDER_H_
#define CODE_USER_LOWPASS_FIRST_ORDER_H_


#include "driverlib.h"
#include "device.h"

int16_t lpf1_q15_step(int16_t x_q15, uint16_t alpha_q15, int16_t* y_state_q15);

#endif /* CODE_USER_LOWPASS_FIRST_ORDER_H_ */
