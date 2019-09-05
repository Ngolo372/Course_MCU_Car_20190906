#include <msp430.h> 

#include <init.h>
#include <uart.h>
#include <count.h>
#include <mpu6050.h>
#include <q4.h>

//PWM SETTINGS
#define period 2000

//distance
int count_up = 0,count_down = 0;
int distance = 0.0;
int last_distance[2] = {25,25};
char dis_flag = 1;//距离保护模式
char dis_safe_flag = 1;
extern char distance_char[13];

//yaw_angle
float Yaw_Angle;
int Yaw_Angle_int;
int Yaw_Angle_int_last = 0;
int Yaw_Angle_goal_int = 0;
int Yaw_Angle_Error = 0;
int Yaw_Angle_Error_A = 0;
int Yaw_Angle_Error_B = 0;
char Yaw_Char[8]={"Y: 000\r\n"};
char str_flag = 1;

//speed
extern int speed;
extern int speed_e;
char front_flag = 0; //0:front 1:back
char speed_parameter_flag = 0;

#define kp 86
#define ki_div 54
#define kd 4

//error_handle
#define period_e 0.233
unsigned int it = 0;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	msp_init();
	MPU6050Init();

	_EINT();

	int ax,ay,az,gx,gy,gz;

	while(1){
				ax = GetAccelX();
				ay = GetAccelY();
				az = GetAccelZ();
				gx = GetAnguX();
				gy = GetAnguY();
				gz = GetAnguZ();
				Yaw_Angle = IMUupdate( gx,  gy,  gz,  ax,  ay,  az);
				Yaw_Angle = Yaw_Angle + (period_e) * it;
				Yaw_Angle_int = (int)Yaw_Angle;
				it++;
				while(Yaw_Angle_int < -180){Yaw_Angle_int += 360;}
				while(Yaw_Angle_int > 180){Yaw_Angle_int -= 360;}

				Yaw_Angle_Error_B = Yaw_Angle - Yaw_Angle_int_last;
				if((!front_flag) && (!str_flag)){
						Yaw_Angle_Error = Yaw_Angle_int - Yaw_Angle_goal_int;
						if(Yaw_Angle_Error > 180)Yaw_Angle_Error-= 180;
						if(Yaw_Angle_Error < -180)Yaw_Angle_Error += 180;
						if(Yaw_Angle_Error > 3 || Yaw_Angle_Error < -3){
							Yaw_Angle_Error_A += Yaw_Angle_Error;
							speed_e = Yaw_Angle_Error * kp / 10 + Yaw_Angle_Error / ki_div + Yaw_Angle_Error_B * kd;
						if(speed_e > 400)speed_e = 400;
						if(speed_e < -400)speed_e = -400;
						speed = 1200;
						}
						else speed_e = 0;
				}
				Yaw_Angle_int_last = Yaw_Angle;

				Yaw_Char[2] = (Yaw_Angle_int < 0) ? '-':'+';
				Yaw_Angle_int = (Yaw_Angle_int < 0) ? (Yaw_Angle_int * (-1)) : Yaw_Angle_int;
				Yaw_Char[3] = (Yaw_Angle_int / 100) + 48;
				Yaw_Char[4] = ((Yaw_Angle_int / 10) % 10) + 48;
				Yaw_Char[5] = (Yaw_Angle_int % 10) + 48;
				USARTA0_send(Yaw_Char, 8);

				if(!speed){
					TA1CCR1 = 0;
					TA1CCR2 = 0;
				}
				else if(!front_flag){
					if(!speed_parameter_flag){
						TA1CCR1 = speed + speed_e + speed_error_front_2;
						TA1CCR2 = speed - speed_e - speed_error_front_2;
						speed_parameter_flag ++;
					}
					else{
						TA1CCR1 = speed + speed_e + speed_error_front;
						TA1CCR2 = speed - speed_e - speed_error_front;
						if(speed_parameter_flag == 2)speed_parameter_flag = 0;
						else speed_parameter_flag ++;
					}

				}
				else{
					if(!speed_parameter_flag){
						TA1CCR1 = speed + speed_e + speed_error_back_2;
						TA1CCR2 = speed - speed_e - speed_error_back_2;
						speed_parameter_flag ++;
					}
					else{
						TA1CCR1 = speed + speed_e + speed_error_back;
						TA1CCR2 = speed - speed_e - speed_error_back;
						if(speed_parameter_flag == 2)speed_parameter_flag = 0;
						else speed_parameter_flag ++;					}
				}

	};

}


#pragma vector = USCIAB0RX_VECTOR
__interrupt void uscia0_isr(){
    uart_command();
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void timer0_isr(){
    P1OUT |= BIT3;
    __delay_cycles(50);
    P1OUT &= ~BIT3;
    //P1OUT ^= BIT3;   //for debug
}

#pragma vector = PORT1_VECTOR
__interrupt void port1_isr(){
    if(P1IFG & BIT4){
        P1IFG &= ~BIT4;
        switch(P1IN & BIT4){
        case 0:
            P1IES &= ~BIT4;
            count_down = TA0R;
            if(last_distance[0])last_distance[1] = last_distance[0];
            if(distance)last_distance[0] = distance;
            distance = (count_down - count_up) ;
            if(distance < 100 && 0 <= distance){
                distance_char[3] = (distance) % 10 + 48;
                distance_char[2] = (distance/10) % 10 + 48;
            }
            USARTA0_send(distance_char, 6);
            if((!dis_flag) && ((!front_flag) || (!speed))){
            	dis_safe_flag = 1;
                sound_stop(distance, last_distance[0], last_distance[1]);
            }
            break;
        case BIT4:
            P1IES |= BIT4;
            count_up = TA0R;
            break;
        default:
            break;
        }
    }
}
