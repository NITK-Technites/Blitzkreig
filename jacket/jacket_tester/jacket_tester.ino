

#define ir_leds 4
#define id 1

int burst_len=0;

void setup()
{
   pinMode(ir_leds,1); 

    switch(id)
    {
       case 1:
        burst_len=1000;
        break;
       case 2:
        burst_len=2000;
        break; 
    }

}

void loop()
{
  
  for(int i=1;i<=burst_len/26;i++)
  {
    digitalWrite(ir_leds,1);
    delayMicroseconds(13);
    digitalWrite(ir_leds,0);
    delayMicroseconds(13);
  }
  
///delay(100);
  
}
