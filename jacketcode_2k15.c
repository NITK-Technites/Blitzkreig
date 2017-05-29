/*c code for jacket

                              MSP430G2452
                           -----------------
                   Vcc -->|Vcc          Gnd |<-- Ground       
                          |                 |
	Jacket LED control <--|P1.0         P2.6|-->        
                          |                 |
                    Rx -->|P1.1         P2.7|<-- 
                          |                 |
                    Tx <--|P1.2         TST |--> Open                 
                          |                 |
  Equipment Signals Tx <--|P1.3         RST |<-- Pulled high     
                          |                 |
      Life display bit <--|P1.4         P1.7|<-- Unused
                          |                 |
	       Shift Clock <--|P1.5         P1.6|<-- Unused
                          |                 |
              Sensor_1 -->|P2.0         P2.5|<-- Sensor_6
                          |                 |
              Sensor_2 -->|P2.1         P2.4|<-- Sensor_5
                          |                 |
              Sensor_3 -->|P2.2         P2.3|<-- Sensor_4
                           -----------------

P1.0 = Jacket LED control
P1.1 = 
P1.2 = shift clock
P1.3 = Gun Signals Bit
P1.4 = Life display bit
P1.5 = shift clock
P1.6 = Display Button
P1.7 = 
P2.0-5 = Sensor Inputs
P2.7 = Status LED

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
0x1X = Unit no

Code by:
Adarsh Hasandka
NITK Surathkal
Dept of ECE

 */
#include <msp430g2452.h>

//Constants
#define DELAY_IR            26				// IR delay for protocol
char bit=0,timeout=0,data=0,busy=0,life=8,life_val=0,data_byte=0;
char sample[5];
void rec_ir(char pin)
{
	char b=0,sum=0;
	_delay_cycles(DELAY_IR*80);
	for(b=0;b<5;++b)
	{
		if(P2IN&pin)
			sample[b]=0;
		else
			sample[b]=1;
		_delay_cycles(DELAY_IR/2);
		sum += sample[b];
	}
	data=data>>1;
	if(sum>3)
		data |= 0x40;
	++bit;
}


void send_data(char command)
{
	busy=1;
	char no_bits=5;
	P1OUT|=0x08;							//send start bit
	P1OUT&=~0x08;							//send start bit
	_delay_cycles(30);
	while(no_bits)
	{
		--no_bits;
		if(command&0x01)
			P1OUT|=0x08;
		else
			P1OUT&=~0x08;
		command=command>>1;
		_delay_cycles(50);
	}
	busy=0;
}

display_data(char lfe)
{
	char display=0,i=0;
	if(life_val!=lfe)
	{
		for(i=0;i<8;++i)
		{
			display=display>>1;
			if(lfe)
			{
				display |= 0x80;
				--lfe;
			}
		}
		for(i=8;i>0;--i)
		{
			if(display&0x01)
				P1OUT |= 0x10;
			else
				P1OUT &= ~0x10;
			P1OUT |= 0x20;
			_delay_cycles(2);
			P1OUT &= ~0x20;
			_delay_cycles(2);
			display=display>>1;
		}
		life_val=lfe;
	}
}

flush_display()
{
	char i;
	P1OUT &= ~0x10;
	for(i=0;i<15;++i)
	{
		P1OUT |= 0x20;
		P1OUT &= ~0x20;
	}
	life_val=0;
}
void main()
{
	WDTCTL = WDT_MDLY_32;                   // Set Watchdog Timer interval to ~32ms
	BCSCTL1 = CALBC1_1MHZ;                  // Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;
	TACTL = TASSEL_2 + MC_1;   				// SMCLK, upmode
	P1DIR |= 0xFF;							// P1.1 input, rest outputs
	P1OUT = 0x01;							// Ready LED
	P2SEL = 0;
	P2DIR |= 0xC0;	 						// P2.6,P2.7 outputs
	P2IES |= 0x3F;							// P2.0 - P2.5 hi to low
	P2OUT |= 0x3F;							// P2.0 - P2.5 pulled high
	P2REN |= 0x3F;							// P2.0 - P2.5,P2.7 resistor enable
	P2IE |= 0x3F;							// P2.0 - P2.5,P2.7 interrupt enable
	IE1 |= WDTIE;							// Enable watchdog interrupt
	P2IFG = 0;
	while(1)
	{
		_BIS_SR(LPM1_bits + GIE);       	// Enter LPM1 w/ interrupt
	}
}


// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	P1OUT ^= 0x01; 							// P1.1 Blink, Data detected
	busy=1;
	rec_ir(P2IFG);							// Get Data
	busy=0;
	P1OUT ^= 0x01; 							// P1.1 Blink, Data Received
	if(bit>6)
	{
		timeout=0;
		switch(data)
		{
			case 0x00: break;
			case 0x01:	
			case 0x02:	if(life==0)
							life=8;		// reset life
						else
							data=0;			// Invalid reset
						data=0x01;
						P1OUT |= 0x01; 
						break;
			case 0x06:	
			case 0x07:  life=0;
						data=0x07;			// game has ended
						break;
			default: 	if(life)
						{
							--life;			// Decrease life
							data=0x04;
						}
						if(life==0)
						{
							P1OUT &= ~0x01;
							data=0x07;
						}
		}
		send_data(data);
		bit=0;
	}
	P2IFG = 0;								// Clear P1IFG
}

// Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
	if(busy)
	{
		if((timeout>0)&&(bit<7))
		{
			bit=8;
			data=0;
			if(P2IFG==0)
				P2IFG=0x01;
		}
		else
		{
			if((bit>0)&&(bit<7))
				++timeout;
		}
	}
	else
	{
		if(life_val!=life)
			display_data(life);
		if(life==0)
		{
			send_data(0x07);
			P1OUT &= ~0x01;
			flush_display();
		}			
	}
}
