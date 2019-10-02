#include<EEPROM.h>

#define from_jacket 2
#define motor_led_flash 7
#define from_trigger 4
#define irleds 9

void falling_edge();
void rising_edge();
void getting_shot();
void fire();
void shoot_ir(unsigned );

// Player id: 1-> Team 1 ; 2-> Team 2;
const char id=2,damage=10;
volatile unsigned long start=0,stopp=0,delta=0,ml_start=0,shoot=0;
unsigned int jacket_burstLen=0,gun_burstLen=0,health=100,god_burst_len=15,hit=0;

void setup() {
  // put your setup code here, to run once:

  pinMode(from_jacket,INPUT_PULLUP);

  Serial.begin(115200);

  health=EEPROM.read(0x00);

  attachInterrupt(digitalPinToInterrupt(from_jacket),falling_edge,FALLING);
  attachInterrupt(digitalPinToInterrupt(from_trigger),fire,FALLING);

  switch(id)
  {
    case 1:
      jacket_burstLen=7;  // from calibration
      gun_burstLen=2;
      break;
    case 2:
      jacket_burstLen=3;   
      gun_burstLen=5;
      break;
  }

}

void loop() {
  // put your main code here, to run repeatedly:

  
  if(shoot!=0)
  {
        Serial.println("--------------------");

    
     detachInterrupt(digitalPinToInterrupt(from_trigger)); 
     shoot=0; 
     shoot_ir(gun_burstLen);
     attachInterrupt(digitalPinToInterrupt(from_trigger),fire,FALLING);
  }
  else if(start!=0)
  {

    
    detachInterrupt(digitalPinToInterrupt(from_jacket));

    while(digitalRead(from_jacket)==0)
    {
      ;
    }
    stopp=micros();
    
    delta=(stopp-start)/1000;

    
    Serial.println(start);
    Serial.println(stopp);
    Serial.println(delta);
    Serial.println();


    start=0;
    stopp=0;

    
    if(delta>god_burst_len-1 && delta<god_burst_len+1)
    {
      //God gun firing
      health=100;
      
      Serial.println("God gun replinished my health");
      
      //update eeprom
      EEPROM.update(0x00,health);

      //update health bar
      
    }
    else if((delta>jacket_burstLen-1 && delta<jacket_burstLen+1) && health!=0)
    {      
      // Enemy fire ; Reduce health

      health=health-damage;
      // reduce health bar
      //******************************
      Serial.print("Health displayed on health bar:");
      Serial.println(health);
      //******************************
      
      // update eeprom
      EEPROM.update(0x00,health);
      
      // vibrate and notify getting shot
      Serial.println("Motor vibrated");
      hit=1;
    }

    attachInterrupt(digitalPinToInterrupt(from_jacket),falling_edge,FALLING);

  }

  if(health==0)
  {
     // Make led strip blink 
     //****
     Serial.println("Health zero!!");
     //****
  }

  if(hit==1)
  {
    ml_start=millis();
    hit=0;
    digitalWrite(motor_led_flash,1);
  }

  if(ml_start!=0 && (millis()-ml_start)>500)
  {
    ml_start=0;
    digitalWrite(motor_led_flash,0);
  }

}

void falling_edge()
{
  // ********************************************** check if millis() works better ******************************************  
  start=micros();  
}

void fire()
{
   shoot=1; 
}

void shoot_ir(unsigned gun_burstLen)
{
    for(int i=1;i<=((gun_burstLen*1000)/26);i++)
    {
       digitalWrite(irleds,1);
       delayMicroseconds(13);
       digitalWrite(irleds,0);
       delayMicroseconds(13);  
    }
}

