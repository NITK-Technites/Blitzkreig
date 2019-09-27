

#define ir_leds 3
#define id 1

unsigned long burst_len=0;

void setup()
{
   pinMode(ir_leds,1); 

    switch(id)
    {
       case 1:
        burst_len=1000;
        break;
       case 2:
        burst_len=2000000;
        break; 
    }

}

void loop()
{
  
  for(unsigned long i=1;i<=burst_len/26;i++)
  {
    digitalWrite(ir_leds,1);
    delayMicroseconds(13);
    digitalWrite(ir_leds,0);
    delayMicroseconds(13);
  }
  
  delay(1000);
  
}
