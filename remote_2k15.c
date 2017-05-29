/*c code for gun

                              MSP430G2231
                           -----------------
                   Vcc -->|Vcc          Gnd |<-- Ground       
                          |                 |
	         Power LED <--|P1.0         P2.6|<--            
                          |                 |
                       -->|P1.1         P2.7|<-- 
                          |                 |
                IR LED <--|P1.2         TST |--> Open                 
                          |                 |
        Powerup Switch -->|P1.3         RST |<-- Pulled high     
                          |                 |
     Game Start switch -->|P1.4         P1.7|<-- Death switch
                          |                 |
	    Respawn Switch -->|P1.5         P1.6|--> Game Over switch
                           -----------------

~28ms for tx

IR Protocol Codes:
0x01 = Start Game
0x02 = Reset Life
0x03 = Got Ammo
0x04 = Got Shot
0x05 = Got Blown Up
0x06 = Game Over
0x07 = Death
0x08 = Rapidfire
0x09 = Laser Sight
0x0A = Invincibility
0x1X = Unit no

Sounds:
0x01 = Powerup 
0x02 = Shot
0x03 = Hit
0x04 = Reload
0x05 = Death
0x06 = Overheat

Code by:
Adarsh Hasandka
NITK Surathkal
Dept of ECE

 */
#include <msp430g2231.h>

//Constants
#define DELAY_IR            26				// IR delay for protocol [CHANGE]

void transmit(char msg)
{
	P1OUT &= ~0x01;				// Turn on LED
	char no_bits=7,no_pulses=10;
	while(no_bits)
	{
		--no_bits;
		for(no_pulses=15;no_pulses>0;--no_pulses)
		{
			P1OUT|=0x04;
			_delay_cycles(DELAY_IR);
			P1OUT&=~0x04;
			_delay_cycles(DELAY_IR);
		}
		_delay_cycles(DELAY_IR*40);
		if(msg&0x01)
		{
			for(no_pulses=30;no_pulses>0;--no_pulses)
			{
				P1OUT|=0x04;
				_delay_cycles(DELAY_IR);
				P1OUT&=~0x04;
				_delay_cycles(DELAY_IR);
			}
			_delay_cycles(DELAY_IR*60);
		}
		else
			_delay_cycles(DELAY_IR*120);
		msg=msg>>1;
	}
	P1OUT |= 0x01;		// Turn off LED
}



void main()
{					
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
    DCOCTL = CALDCO_1MHZ;
	TACTL = TASSEL_2 + MC_1;   				// SMCLK, upmode
	P1DIR |= 0x07;							// P1.0,P1.2 outputs
	P1IES |= 0xF8;							// P1.6,P1.7 hi to low edge
	P1OUT = 0xF9;							// Set pullup
	P1REN |= 0xF8;							// Pullup/Pulldown enabled
	P1IE = 0xF8;
	P2SEL &= 0;								// P2.6,P2.7 GPIO option
	P2OUT = 0;							// Set pullup
	P1IFG = 0;
	while(1)
	{
		_BIS_SR(LPM4_bits + GIE);       	// Enter LPM4 w/ interrupt
	}
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	_delay_cycles(100000);
	switch(P1IFG)
	{
		case 0x08: transmit(0x07);break;
		case 0x10: transmit(0x02);break;
		case 0x20:  _delay_cycles(300000);
					if(P1IN&0x10)
						transmit(0x09);
					else
						transmit(0x08);
					break;
		case 0x40: transmit(0x01);break;
		case 0x80: transmit(0x06);break;
	}
	P1IFG &= 0;								// Clear P1IFG
}
