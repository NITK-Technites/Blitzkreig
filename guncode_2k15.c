/*c code for gun

                              MSP430G2231
                           -----------------
                   Vcc -->|Vcc          Gnd |<-- Ground       
                          |                 |
	      Overheat LED <--|P1.0         P2.6|<-- Gun trigger          
                          |                 |
  Muzzle Flash control <--|P1.1         P2.7|<-- 
                          |                 |
        IR LED control <--|P1.2         TST |--> Open                 
                          |                 |
       sound shift pin <--|P1.3         RST |<-- Pulled high     
                          |                 |
        sound data pin <--|P1.4         P1.7|<-- Jacket receive bit
                          |                 |
	    Laser control  <--|P1.5         P1.6|--> Vibration control
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

char shotcount=0,busy=0;
unsigned int overheat=0;
char unitno=0,status=1;			
unsigned int laser=0,rapidfire=0;

void play_sound(char msg)
{
	char i;
	for(i=0;i<3;++i)
	{
		if(msg&0x01)
			P1OUT |= 0x10;
		else
			P1OUT &= ~0x10;
		msg=msg>>1;
		P1OUT |= 0x08;							// pull shift pin high
		_delay_cycles(5);
		P1OUT &= ~0x08;							// pull shift pin low
		_delay_cycles(5);
	}
}

void transmit()
{
	if(P1OUT&0x01)					// If still hot
	{
		play_sound(0x06);
	}
	else
	{
		if((status>0)&&(shotcount==0))
		{
			P1OUT |= 0x43;				// Turn on LEDs and Motor
			char msg=0x04+(unitno<<4);				// Append unit number
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
			if(rapidfire)
			{
				shotcount=5;					// triple fire rate
				overheat=overheat+10;
			}
			else
			{
				shotcount=15;					// normal fire rate (2.08 shots/sec)
				overheat=overheat+30;
			}
			P1OUT &= ~0x03;		// Turn off LED 
			play_sound(0x02);
		}
	}
}


char rec_jacket()
{
	busy=1;
	char value=0,no_bits=5;
	_delay_cycles(45);
	while(no_bits)
	{
		--no_bits;
		value=value>>1;
		if(P1IN&0x80)
			value|=0x10;
		else
			value&=~0x10;
		_delay_cycles(50);
	}
	busy=0;
	return value;
}

void main()
{					
	WDTCTL = WDT_MDLY_32;                   // Set Watchdog Timer interval to ~32ms
	BCSCTL1 = CALBC1_1MHZ;                  // Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;
	TACTL = TASSEL_2 + MC_1;   				// SMCLK, upmode
	P1DIR |= 0x7F;							// P1.0,P1.1,P1.2,P1.3,P1.4,P1.5,P1.6 outputs
	P1IES |= 0x80;							// P1.7 hi to low edge
	P1OUT = 0x80;							// Set pullup
	P1REN |= 0x80;							// Pullup enabled
	P2SEL &= 0;								// P2 GPIO option
	P2DIR |= 0x80;	 						// P2.6 inputs
	P2IES |= 0x40;							// P2.6 hi to low edge
	P2OUT = 0x40;							// Set pullup
	P2REN |= 0x40;							// Pullup enabled
	IE1 |= WDTIE;							// Enable watchdog interrupt
	P1IE = 0x80;
	P1IFG = 0;
	P2IE = 0x40;
	P2IFG = 0;
	while(1)
	{
		_BIS_SR(LPM1_bits + GIE);       	// Enter LPM1 w/ interrupt
	}
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	char data;
	if(P1IFG&0x80)
		data=rec_jacket();
	if(data>0)
	{
		if((data&0x10)==0x10)
			unitno=(data&0x07);				// obtain unit number
		switch(data)
		{
			case 0x01:
			case 0x03:	status=1;
						P2IE |= 0x40;      // Enable trigger
						break;
			case 0x04:	
			case 0x05:	play_sound(0x03);	// Play hit sound
						break;
			case 0x06:	play_sound(0x05);	// Play Death sound
						status=0;
						break;
			case 0x07:  if(status)
						{
							P2IE &= ~0x40;      // Disable trigger
							play_sound(0x05);	// Play Death sound
							status=0;
						}
						break; 
			case 0x08: 	if(rapidfire==0)
						{
							play_sound(0x01);   // Play Powerup sound
							rapidfire=9375;	// Get 5 min powerup
						}
						break;
			case 0x09: 	if(laser==0)
						{
							play_sound(0x01);   // Play Powerup sound
							laser=9375;		// Get 5 min powerup
						}
						break;
			case 0x0A: 	play_sound(0x01);   // Play Powerup sound
		}
	}
	P1IFG &= 0;								// Clear P1IFG
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	if(P2IFG&0x40)			// If trigger pressed
		transmit();
	P2IFG &= 0;								// Clear P2IFG
}

// Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
	if(((P1OUT&0x02)==0)&&(busy==0))		//if not busy
	{
		if(overheat>299)
			P1OUT |= 0x01; 			// Turn on Overheat LED
		if(overheat)
			--overheat;
		else
			P1OUT &= ~0x01; 		// Turn off Overheat LED
		if(shotcount)
			--shotcount;
		if(P1OUT&0x40)
			P1OUT &= ~0x40;						// Turn off motor
		if((P2IN&0x40)==0)	// If trigger held
			transmit();		
		if(rapidfire)
			--rapidfire;
		if((laser>0)&&(status>0))
		{
			P1OUT |= 0x20;						// turn on laser
			--laser;
		}
		else
			P1OUT &= ~0x20;						// turn off laser
	}
}
