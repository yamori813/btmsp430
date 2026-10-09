/*
 */

#include <msp430f5507.h>

int irctl = 0;

int irtype;
int irbitlen;
char irpat[32];
int curpos;
int repate;

void wait(void)          //delay function
{
  volatile int i;        //declare i as volatile int
  for(i=0;i<32000;i++);  //repeat 32000 times
}

void ir(int sw)
{
	if(sw == 1) {
		P2SEL |= BIT0;
		TA1CCR0 = 25;
		TA1CCTL1 = OUTMOD_7;
		TA1CCR1 = 8;
		TA1CTL = TASSEL_2 + MC_1 + TACLR;
	} else {
		TA1CCR0 = 0;
		P2SEL &= ~BIT0;
	}
}

#if 0
int main(void)
{
char buf[32];

	/* Init watchdog timer to off */
	WDTCTL = WDTPW|WDTHOLD;

	/* Init Output ports to GND */
	P6DIR = BIT0 | BIT1 | BIT2;

	/* Turn off all LED */ 
	P6OUT = BIT0 | BIT1 | BIT2;

	/* Interrupts on Power button */
	P1IES  = BIT1;
	P1IE   = BIT1;

	/* BT controle */
	P4DIR |= BIT2;
	P4OUT &= ~BIT2;

	uart_init();


	/* IR output port with PWM */
	P2DIR |= BIT0;

	for (;;) _BIS_SR(CPUOFF);	// into LPM0
}
#endif

void start_timer(int usec)
{
	TA0CTL = TASSEL_2 | MC_3 | TACLR;
	TA0CCR0 = usec;

	TA0CCTL0 = CCIE;
}

int bitpos;

#define SETTIMER(x) TA0CCR0 = x

#define AEHA_T	367

void aehair()
{
	int curbit;
	
	if (curpos == 0) {
		ir(1);
		bitpos = 0;
		++curpos;
		SETTIMER(AEHA_T * 8);
	} else if (curpos == 1) {
		ir(0);
		++curpos;
		SETTIMER(AEHA_T * 4);
	} else if (curpos - 2 == irbitlen) {
		ir(1);
		++curpos;
		SETTIMER(AEHA_T);
	} else if (curpos - 3 == irbitlen) {
		ir(0);
		// STOP or REPETE
		if (repate == 0) {
			P6OUT |= BIT1;
			P6OUT &= ~BIT0;
			TA0CCTL0 &= ~CCIE;
		} else {
			SETTIMER(AEHA_T * 50);
			curpos = 0;
			--repate;
		}
	} else {
		curbit = irpat[(curpos - 2) / 8];
		curbit = (curbit >> (7 - ((curpos - 2) % 8))) & 1;
		if (bitpos == 0) {
			ir(1);
			SETTIMER(AEHA_T);
			bitpos = 1;
		} else {
			ir(0);
			if (curbit == 0)
				SETTIMER(AEHA_T);
			else
				SETTIMER(AEHA_T * 3);
			bitpos = 0;
			++curpos;
		}
	}
	TA0CTL = TASSEL_2 | MC_3 | TACLR;	
}

void necir()
{
	int curbit;
	
	if (curpos == 0) {
		ir(1);
		bitpos = 0;
		++curpos;
		SETTIMER(9000);
	} else if (curpos == 1) {
		ir(0);
		++curpos;
		SETTIMER(4500);
	} else if (curpos - 2 == irbitlen) {
		ir(1);
		++curpos;
		SETTIMER(560);
	} else if (curpos - 3 == irbitlen) {
		ir(0);
		// STOP or REPETE
		if (repate == 0) {
			P6OUT |= BIT1;
			P6OUT &= ~BIT0;
			TA0CCTL0 &= ~CCIE;
		} else {
			SETTIMER(25100);
			curpos = 0;
			irbitlen = -1;
			--repate;
		}
	} else {
		curbit = irpat[(curpos - 2) / 8];
		curbit = (curbit >> (7 - ((curpos - 2) % 8))) & 1;
		if (bitpos == 0) {
			ir(1);
			SETTIMER(560);
			bitpos = 1;
		} else {
			ir(0);
			if (curbit == 0)
				SETTIMER(560);
			else
				SETTIMER(1690);
			bitpos = 0;
			++curpos;
		}
	}
	TA0CTL = TASSEL_2 | MC_3 | TACLR;	
}

void melcoir()
{
	int curbit;
	
	if (curpos == 0) {
		ir(1);
		bitpos = 0;
		++curpos;
		SETTIMER(7900);
	} else if (curpos == 1) {
		ir(0);
		++curpos;
		SETTIMER(3900);
	} else if (curpos - 2 == irbitlen) {
		ir(1);
		++curpos;
		SETTIMER(560);
	} else if (curpos - 3 == irbitlen) {
		ir(0);
		// STOP or REPETE
		if (repate == 0) {
			P6OUT |= BIT1;
			P6OUT &= ~BIT0;
			TA0CCTL0 &= ~CCIE;
		} else {
			SETTIMER(18400);
			curpos = 0;
			irbitlen = -1;
			--repate;
		}
	} else {
		curbit = irpat[(curpos - 2) / 8];
		curbit = (curbit >> (7 - ((curpos - 2) % 8))) & 1;
		if (bitpos == -2) {
			ir(1);
			SETTIMER(500);
			bitpos = -1;
		} else if (bitpos == -1) {
			ir(0);
			SETTIMER(3900);
			bitpos = 0;
		} else if (bitpos == 0) {
			ir(1);
			SETTIMER(460);
			bitpos = 1;
		} else {
			ir(0);
			if (curbit == 0)
				SETTIMER(380);
			else
				SETTIMER(1400);
			bitpos = 0;
			++curpos;
			if (curpos > 2 && (curpos - 2) % 8 == 0)
				bitpos = -2;
			else
				bitpos = 0;
		}
	}
	TA0CTL = TASSEL_2 | MC_3 | TACLR;	
}

void sonyir()
{
int curbit;

	if (curpos == 0) {
		ir(1);
		bitpos = 0;
		++curpos;
		SETTIMER(2460);
	} else if (curpos == 1) {
		ir(0);
		++curpos;
		SETTIMER(525);
	} else {
		if (curpos - 2 == irbitlen) {
			ir(0);
			// STOP or REPETE
			if (repate == 0) {
				P6OUT |= BIT1;
				P6OUT &= ~BIT0;
				TA0CCTL0 &= ~CCIE;
			} else {
				SETTIMER(25100);
				curpos = 0;
				--repate;
			}
		} else {
			curbit = irpat[(curpos - 2) / 8];
			curbit = (curbit >> (7 - ((curpos - 2) % 8))) & 1;
			if (bitpos == 0) {
				ir(1);
				if (curbit == 0)
					SETTIMER(660);
				else
					SETTIMER(1245);
				bitpos = 1;
			} else {
				ir(0);
				if (curbit == 0)
					SETTIMER(540);
				else
					SETTIMER(540);
				bitpos = 0;
				++curpos;
			}
		}
	}
	TA0CTL = TASSEL_2 | MC_3 | TACLR;
}

void sendir(int type, int len, char *dat, int rep)
{
int i;

	irtype = type;
	irbitlen = len;
	repate = rep;
	for(i = 0; i < (len / 8) + 1; ++i)
		irpat[i] = dat[i];

	curpos = 0;
	start_timer(10);

	P6OUT |= BIT0;
	P6OUT &= ~BIT1;
}

void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TIMER0_A0_ISR (void)
{
	if (irtype == 3)
		sonyir();
	else if (irtype == 1)
		aehair();
	else if (irtype == 2)
		necir();
	else if (irtype == 4)
		melcoir();
	TA0CCTL0 &= ~CCIFG;
}

void __attribute__ ((interrupt(PORT1_VECTOR))) PORT1_ISR (void)
{
#if 0
	if (irctl == 0) {
		/* BT Enable */
		P4OUT |= BIT2;
		irctl = 1;
		/* LED On */
		P6OUT &= ~BIT0;
		wait();
	} else {
		/* BT Disable */
		P4OUT &= ~BIT2;
		irctl = 0;
		/* LED Off */
		P6OUT |= BIT0;
		wait();
	}
#endif

	P1IFG &= ~BIT1;
}
