/* test code for sound
P1.2 = speaker pwm out
P1.6 = data in
P1.7 = data latch

Sounds:
0x01 = Powerup 
0x02 = Shot
0x03 = Hit
0x04 = Reload
0x05 = Death
0x06 = Overheat
*/

#include  <msp430g2211.h>
char sound=0,bit=0;
void main()
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
  DCOCTL = CALDCO_1MHZ;
  TACTL = TASSEL_2 + MC_1;   				// SMCLK, upmode
  P1DIR |= 0x3F;                            // P1.2 output, P1.6,P1.7 
  P1OUT = 0;								// Clear P1OUT
  P1SEL |= 0x04;                            // P1.2 TA options
  P1REN |= 0x80;							// P1.7 pulldown enable
  P1IES |= 0x80;							// P1.7 hi to low edge
  P1IE |= 0x80;							    // P1.7 interrupt enable
  CCR0 = 10000;                             // PWM Period
  CCTL1 = OUTMOD_7;                         // CCR1 reset/set
  CCR1 = 0;                                 // CCR1 PWM duty cycle
  while(1)
  {
  	_BIS_SR(LPM4_bits + GIE);       	// Enter LPM4 w/ interrupt
  	if(sound==1)
  	{
  		CCR0=10000;
		while(CCR0)
		{
			--CCR0;
			if(CCR0<1800)
			{
				_delay_cycles(20000);
				CCR0=0;
			}
			CCR1 = CCR0/2;
			_delay_cycles(50);
		}
  	}
  	if(sound==2)
  	{
  		CCR0 = 0;
  		while(CCR0<8000)
  		{
  			++CCR0;
  			if(CCR0>4000)
  				CCR0=CCR0+7;
    		CCR1 = CCR0/2;
    		_delay_cycles(50);
  		}
  	}
  	if(sound==3)
  	{
  		CCR0 = 6000;
    	CCR1 = 3000;
    	_delay_cycles(100000);
  	  	CCR0 = 4000;
    	CCR1 = 2000;
    	_delay_cycles(100000);
    	CCR0 = 8000;
    	CCR1 = 4000;
    	_delay_cycles(100000);
    	CCR1 = 0;
  	}
	if(sound==4)
	{
		CCR0 = 3000;
    	CCR1 = 1500;
    	_delay_cycles(150000);
  	  	CCR0 = 2000;
    	CCR1 = 1000;
    	_delay_cycles(100000);
    	CCR0 = 1000;
    	CCR1 = 500;
    	_delay_cycles(50000);
    	CCR1 = 0;
	}
	if(sound==5)
	{
		CCR0 = 500;
  	  	CCR1 = 250;
  	  	_delay_cycles(1000000);
  	  	CCR1 = 0;
	}
	if(sound==6)
	{
		CCR0 = 2000;
    	CCR1 = 1000;
    	_delay_cycles(80000);
		CCR1 = 0;
		_delay_cycles(80000);
  	  	CCR0 = 2000;
    	CCR1 = 1000;
    	_delay_cycles(80000);
		CCR1 = 0;
		_delay_cycles(80000);
    	CCR0 = 2000;
    	CCR1 = 1000;
    	_delay_cycles(80000);
    	CCR1 = 0;
		_delay_cycles(80000);
		CCR0 = 2000;
    	CCR1 = 1000;
    	_delay_cycles(80000);
		CCR1 = 0;
	}
  	CCR0 = 10000;
    CCR1 = 0;
  }
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	sound=sound>>1;
	if(P1IN&0x40)
		sound|=0x04;
	else
		sound&=~0x04;
	++bit;
	P1IFG &= 0;								// Clear P1IFG
	if(bit>2)
	{
		bit=0;
		__bic_SR_register_on_exit(LPM4_bits);   // Clear LPM4_bits bit from 0(SR)
	}
}

