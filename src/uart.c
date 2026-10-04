/* This program asks you for the password, and then tells
 * you if you typed it right.  You need to set the bluetooth
 * terminal to send \r\n, at the end of a send.  The app
 * I used on my Android phone is from:
 * https://github.com/Sash0k/bluetooth-spp-terminal
 *
 * Originally from the textbook, modified for the MSP430F5529
 * by Rob Frohne 7/21/2015.
 *
 * The HC-06 bluetoooth module I used to test this is connected
 * to the +5V and GND on the launchpad.  The RXD of the HC-06 is
 * connected to P3.3 and TXD of the HC-06 is connected to P3.4.
 *
 * Note the changes to the interrupt as compared to the G2 Launchpad.
 */

#include <msp430.h>

char input[64];
unsigned int RXByteCtr = 0;

void sendir(int type, int len, char *dat, int rep);

void uart_init(void)
{
	P4SEL = BIT4 + BIT5;		// P4.4,5 = USCI_A1 TXD/RXD
	UCA1CTL1 |= UCSWRST;		// **Put state machine in reset**
	UCA1CTL1 |= UCSSEL_1;		// SMCLK
	UCA1BR0 = 3;			// 9600 (see User's Guide)
	UCA1BR1 = 0;
	UCA1MCTL  = UCBRS_3;		// over sampling
	UCA1CTL1 &= ~UCSWRST;		// **Initialize USCI state machine**
	UCA1IE |= UCRXIE;		// Enable USCI_A0 RX interrupt
	__enable_interrupt();
}

// USCI A receiver interrupt
// The stuff immediately below is to make it compatible with GCC, TI or IAR

// 55 is preamble 
// second byte is length of data
// last byte is check sum

// 55 02 30 04 8b open
// 55 02 30 01 88 close
// 55 01 31 87 hart beat
// 55 07 b0 02 b3 02 0b a9 00 77 recive data

// custom protocol
// 55 (len) b0 [repeat] b3 [type] [len] <data> (sum)
// type: 0 = NEC, 1 = AEHA, 2 = SONY

void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void)
{
char data;

	data = UCA1RXBUF;
	if (RXByteCtr == 0) {
		if (data == 0x55) {
			++RXByteCtr;
			input[0] = data;
		}
	} else {
		input[RXByteCtr] = UCA1RXBUF;
		++RXByteCtr;
		if(RXByteCtr > 2 && RXByteCtr == input[1] + 3) {
			RXByteCtr = 0;
//			if(input[1] == 7 && input[5] == 2) {
			if(input[1] > 2) {
				/* Indicate LED */
				P6OUT |= BIT0;
				P6OUT &= ~BIT1;
				sendir(input[5], input[6], input + 7, input[3]);
			}
		}
	}
}

void transmit(const char *str) {
	while (*str != 0) {	//Do this during current element is not
		//equal to null character
		while (!(UCTXIFG & UCA1IFG))
			;
		//Ensure that transmit interrupt flag is set
		UCA1TXBUF = *str++;
		//Load UCA1TXBUF with current string element
	}		//then go to the next element
}

