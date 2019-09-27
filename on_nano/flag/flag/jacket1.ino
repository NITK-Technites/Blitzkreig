#include<ESP8266WiFi.h>

volatile unsigned char flag=0;

void setup()
{
   attachInterrupt(digitalPinToInterrupt(D0),isr1,FALLING); 
}

void loop()
{
    if(flag==1)
    {
       flag=0;
       
       if(health>0)
       {
         health=health-damage;
         
         // Sound alarm
       
         // Decrease health on display
         
         // Send details to server
         
         healthCheck();
        }
        else 
        {
           healthCheck(); 
        }

    }
    
    if((millis()-last)>=1000)
    {
       last=millis();
      
       // write health into eeprom 
    }
  
}

void healthCheck()
{
   if(health<=0)
   {
       // Display player dead: blink llights etc.
  
       // Send details to server     
    
   }
 
}

void isr1()
{
   flag=1;
}
