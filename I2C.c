/*
 * By vrqq (vrqq3118@163.com)
 */
#include "I2C.h"

uchar I2CSendBuffer[2],I2CRecvBuffer;

uchar com = '0';

int I2CSendPtr=0;
void I2CWriteInit();
void I2CReadInit();
void I2CInit (uchar SlaveAddr)
{
	P1SEL |= BIT6+BIT7;                       // Assign I2C pins to USCI_B0
	P1SEL2|= BIT6+BIT7;                       // Assign I2C pins to USCI_B0
	UCB0CTL1 |= UCSWRST;                      // Enable SW reset
	UCB0CTL0 = UCMST+UCMODE_3+UCSYNC;         // I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2+UCSWRST;              // Use SMCLK, keep SW reset
	UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
	UCB0BR1 = 0;
	UCB0I2COA = 0x01A5;
	UCB0I2CSA = SlaveAddr;                    // Set slave address
	UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
//	IE2 |= UCB0RXIE+UCB0TXIE;                 // Enable TX&RX interrupt
}

void I2CWriteInit()
{
	UCB0CTL1 |= UCTR;               // UCTR=1 => Transmit Mode (R/W bit = 0)
    IFG2 &= ~UCB0TXIFG;
    IE2 &= ~UCB0RXIE;               // disable Receive ready interrupt
    IE2 |= UCB0TXIE;                // enable Transmit ready interrupt
}
void I2CReadInit()
{
	UCB0CTL1 &= ~UCTR;              // UCTR=0 => Receive Mode (R/W bit = 1)
    IFG2 &= ~UCB0RXIFG;
    IE2 &= ~UCB0TXIE;               // disable Transmit ready interrupt
    IE2 |= UCB0RXIE;                // enable Receive ready interrupt
}

void I2C_Write(uchar address,uchar data)
{
	I2CSendPtr = 1;
	I2CSendBuffer[1] = address;
	I2CSendBuffer[0] = data;

	while (UCB0STAT & UCBUSY);          // wait until I2C module has finished all operations.
    I2CWriteInit();
    _disable_interrupt();
    UCB0CTL1 |= UCTXSTT;
	__bis_SR_register(CPUOFF + GIE);    // Waiting for Sending.
    UCB0CTL1 |= UCTXSTP;                // I2C stop condition
    while (UCB0CTL1 & UCTXSTP);         // Ensure stop condition got sent
}

uchar I2C_Read(uchar address)
{
	P1IE &= ~BIT4;
	IE2 &= ~UCA0RXIE;
	TA0CCTL0 &= ~CCIE;
	while (UCB0STAT & UCBUSY);          // wait until I2C module has finished all operations
	//Change to Write Mode.
	I2CSendBuffer[0] = address;
	I2CSendPtr = 0;
    I2CWriteInit();
    _disable_interrupt();
    UCB0CTL1 |= UCTXSTT;                 // start condition generation
                                         // => I2C communication is started
    __bis_SR_register(CPUOFF + GIE);     // Enter LPM0 w/ interrupts

	//Change to Read Mode.
    I2CReadInit();
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
    while (UCB0CTL1 & UCTXSTT);             // Start condition sent?
    UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
    __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0 w/ interrupts
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    P1IE |= BIT4;
    IE2 |= UCA0RXIE;
    TA0CCTL0 |= CCIE;
    return I2CRecvBuffer;
}

/*----------------------------------------------------------------------------*/
// Description:
//   Acknowledge Polling. The EEPROM will not acknowledge if a write cycle is
//   in progress. It can be used to determine when a write cycle is completed.
/*----------------------------------------------------------------------------*/
void I2C_AckPolling(void)
{
    while (UCB0STAT & UCBUSY) {             // wait until I2C module has
    }                                       // finished all operations

    do {
        UCB0STAT = 0x00;                    // clear I2C interrupt flags
        UCB0CTL1 |= UCTR;                   // I2CTRX=1 => Transmit Mode (R/W bit = 0)
        UCB0CTL1 &= ~UCTXSTT;
        UCB0CTL1 |= UCTXSTT;                // start condition is generated
        while (UCB0CTL1 & UCTXSTT) {        // wait till I2CSTT bit was cleared
            if (!(UCNACKIFG & UCB0STAT)) {  // Break out if ACK received
                break;
            }
        }
        UCB0CTL1 |= UCTXSTP;                // stop condition is generated after
                                            // slave address was sent => I2C communication is started
        while (UCB0CTL1 & UCTXSTP) {        // wait till stop bit is reset
        }
        __delay_cycles(500);                // Software delay
    } while (UCNACKIFG & UCB0STAT);
}


// USCI_B0 Data ISR
// Notice : UCSIAB0RX_ISR should be handle with UCSIAB0TX_ISR
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
	if (UCB0TXIFG & IFG2)                           // TX
	{
		UCB0TXBUF = I2CSendBuffer[I2CSendPtr];      // Load TX buffer
		I2CSendPtr--;                               // Decrement TX byte counter
		if (I2CSendPtr < 0) {
			while (!(IFG2 & UCB0TXIFG));            // wait for tx complete
			IE2 &= ~UCB0TXIE;                       // disable interrupts.
			IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
			__bic_SR_register_on_exit(LPM0_bits);   // Exit LPM0
		}
	}
	else if (UCB0RXIFG & IFG2)                      // RX
	{
		I2CRecvBuffer = UCB0RXBUF;                  // store received data in buffer
		__bic_SR_register_on_exit(LPM0_bits);       // Exit LPM0
	}


}

