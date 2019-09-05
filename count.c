/*
 * count.c
 *
 *  Created on: 2019Äê9ÔÂ3ÈÕ
 *      Author: sangj
 */

#include "count.h"

extern int speed;
extern int speed_e;
extern char dis_safe_flag;

void sound_stop(int distance,int last0,int last1){
    if(last1 > 35)return;
    if(last0 > 30)return;
    if(distance > 20)return;
    else{
    	dis_safe_flag = 0;
        P2OUT &= ~(BIT2 + BIT5);
        speed = 0;
        speed_e = 0;
    }
}


