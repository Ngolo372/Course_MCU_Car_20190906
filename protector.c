#include "protector.h"

void ProtectorInit() //P1.0 LED open when some problem happened.
{
	P1SEL &= ~BIT0;
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
}
void Require()
{
	P1OUT |= BIT0;
}
void Release()
{
	P1OUT &= ~BIT0;
}
