
#define ir_leds D0
#defene burst_len1 1000
#define burst_len2 2000


void setup()
{
   pinMode(ir_leds,1); 

  last=millis();

   while((millis()-last)<=3000)
   {
     for(int i=1;i<=burst_len1;i++)
      {
         digitalWrite(ir_leds,1);
         delayMicroseconds(13);  
         digitalWrite(ir_leds,0);
         delayMicroseconds(13);  
         
      }
    
      
      for(int i=1;i<=burst_len2;i++)
      {
         digitalWrite(ir_leds,1);
         delayMicroseconds(13);  
         digitalWrite(ir_leds,0);
         delayMicroseconds(13);  
         
      }
    }

}

void loop()
{
  
    
  
}
