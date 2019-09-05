/*
 * init.h
 *
 *  Created on: 2019Äê9ÔÂ2ÈÕ
 *      Author: sangj
 */

#ifndef INIT_H_
#define INIT_H_

#include<msp430.h>

void msp_init();

//INITATION: PORT1_2 TIMER1
void port_1_2_init();
void uart_init();
void timer0_init();
void timer1_init();

#endif /* INIT_H_ */
