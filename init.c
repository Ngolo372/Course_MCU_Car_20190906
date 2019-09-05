/*
 * init.c
 *
 *  Created on: 2019年9月2日
 *      Author: sangj
 */

#include <init.h>

#define period 2000

void port_1_2_init(){
    //P1.1/P1.2 : usart
    P1SEL |= BIT1+BIT2;         //置P1.1、P1.2为串行接口收、发引脚功能
    P1SEL2 |= BIT1+BIT2;        //

    //P1.3/1.4 : HR-04
    P1SEL &= ~(BIT3 + BIT4);
    P1SEL2 &= ~(BIT3 + BIT4);
    P1DIR |= BIT3;
    P1DIR &= ~BIT4;
    P1OUT &= ~BIT3;
    P1IE |= BIT4;
    P1IES &= ~BIT4;
    P1IFG &= ~BIT4;

    //P2.1/P2.4 : PWM     P2.2/P2.5:IN1/IN2      P2.3:STBY
    P2SEL |= (BIT1 + BIT4);
    P2SEL &= ~(BIT2 + BIT3 + BIT5);
    P2SEL2 &= ~(BIT2 + BIT3 + BIT5);
    P2DIR |= (BIT1 + BIT2 + BIT3 + BIT4 + BIT5);
    P2OUT &= ~(BIT2 + BIT5);
    P2OUT |= BIT3;
}

void uart_init(){
    //数据格式选用上电复位设置：无校验，8位数据，1个停止位，异步串行通信
    UCA0CTL1 |= UCSWRST;         //置软件复位位swrst为1
    UCA0CTL1 |= UCSSEL1+UCRXEIE; //波特率时钟选择SMCLK，不论对错均收
    UCA0BR0 = 104;              //波特率9600
    UCA0BR1 = 0;
    UCA0MCTL=UCBRF_0+UCBRS_1;
    UCA0CTL1 &=~UCSWRST;        //置软件复位位swrst为0，串口设置完毕
    IE2 |= UCA0RXIE;
}

void timer0_init(){
    BCSCTL1 |= DIVA_0;
    BCSCTL3 |= LFXT1S_2;
    TA0CCTL0 = CCIE;
    TA0CTL = TASSEL_1 + MC_1;
    TA0CCR0 = 3000;
}

void timer1_init(){
    TA1CTL = TASSEL_2 + MC_1 + ID_2;
    TA1CCR0 = period;
    TA1CCTL1 = OUTMOD_7;
    TA1CCTL2 = OUTMOD_7;
    TA1CCR1 = 0;
    TA1CCR2 = 0;
}

void msp_init(){
    port_1_2_init();
    uart_init();
    timer0_init();
    timer1_init();
}

