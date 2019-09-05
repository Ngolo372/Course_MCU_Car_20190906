/*
 * uart.c
 *
 *  Created on: 2019年9月2日
 *      Author: sangj
 */

#include <msp430.h>
#include <uart.h>

//command
char command_code;
char command[6] = "c:00\r\n";
char speed_code;
int speed;
int speed_e = 0;
unsigned int j;

char led7_open[7]={0xa5,0x5a,0x05,0xe7,0x01,0xed,0xaa};
char led7_close[7]={0xa5,0x5a,0x05,0xe7,0x00,0xec,0xaa};
char led9_open[7]={0xa5,0x5a,0x05,0xe8,0x01,0xee,0xaa};
char led9_close[7]={0xa5,0x5a,0x05,0xe8,0x00,0xed,0xaa};

//distance
extern char dis_flag;
char distance_char[6]="d:00\r\n";
extern char dis_safe_flag;
extern char front_flag;

extern char str_flag;

extern int Yaw_Angle_int_last;
extern int Yaw_Angle_goal_int;

void  USARTA0_send(char*word,int n)
{
    unsigned int j = 0;
    for(; j < n; j ++)
    {   while((IFG2&UCA0TXIFG)==0);     //检测发送缓冲是否空
        UCA0TXBUF=word[j];       //取一个数据发送
        };
}

void uart_command(){
    while((IFG2&UCA0RXIFG)==0);     //检测接收缓冲器是否满
    command_code = UCA0RXBUF;       //接收一个数据并保存

    command[2] = command_code;
    //command[3] = (char)(speed / 200 + 48);

    USARTA0_send(command, 6);

    switch(command_code){
    case '7':
            dis_flag = (dis_flag) ? 0 : 1;
            USARTA0_send(((dis_flag) ? led7_close : led7_open) , 7);
            dis_safe_flag = (dis_flag) ? 1 : 0;
            break;
    case '9':
    		str_flag = (str_flag) ? 0 : 1;
            USARTA0_send(((str_flag) ? led9_close : led9_open) , 7);
    		Yaw_Angle_goal_int = Yaw_Angle_int_last;
    		break;
    case '5':
        	front_flag = 1;
            speed_e = 0;
            speed = 0;
            P2OUT &= ~(BIT2 + BIT5);

            break;
    case '2':
            speed_e = 0;
            speed = 0;
            if(!dis_safe_flag)break;
            P2OUT |= BIT2;
            P2OUT &= ~BIT5;
            front_flag = 0;
            speed = 1200;
    		Yaw_Angle_goal_int = Yaw_Angle_int_last;
            break;
    case '8':
    		speed_e = 0;
    		speed = 0;
            front_flag = 1;
            P2OUT &= ~BIT2;
            P2OUT |= BIT5;
            speed = 1200;
            break;
    case '1':
    		speed_e = 0;
    		speed += 200;
    		if(speed  > 1600)speed -= 200;
    		if(speed < 800){
    			speed_e = 0;
    			speed = 800;
    		}
    		break;
    case '3':
    		speed_e = 0;
    		speed -= 200;
    		if(speed < 800){
    			speed_e = 0;
    			speed = 0;
            	front_flag = 1;
    		}
    		break;
    case '6':
    		speed_e += 200;
    		if(speed_e + speed > 1600 || speed - speed_e < 400)speed_e -= 200;
    		break;
    case '4':
    		speed_e -= 200;
    		if(speed - speed_e > 1600 || speed + speed_e < 400)speed_e += 200;
    		break;
    default :
            break;
    }
    //TA1CCR1 = speed + speed_e;
    //TA1CCR2 = speed - speed_e;
}
