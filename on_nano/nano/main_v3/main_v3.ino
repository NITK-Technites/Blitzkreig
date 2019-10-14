#include<EEPROM.h>

#define from_jacket 2
#define motor_led_flash 7
#define from_trigger 4
#define irleds 9

//  pwm pins for health and ammo display 
#define R_HEALTH_DISPLAY 1
#define G_HEALTH_DISPLAY 1
#define B_HEALTH_DISPLAY 1
#define R_AMMO_DISPLAY 1
#define G_AMMO_DISPLAY 1
#define B_AMMO_DISPLAY 1

// macros for display id
#define HEALTH_DISPLAY 0
#define AMMO_DISPLAY 1

void falling_edge();
void rising_edge();
void getting_shot();
void fire();
void shoot_ir(unsigned );
void update_display(unsigned char , unsigned char , unsigned char ,unsigned char );  

// Player id: 1-> Team 1 ; 2-> Team 2;
const char id=2,damage=10;
volatile unsigned long start=0,stopp=0,delta=0,ml_start=0,shoot=0;
unsigned int jacket_burstLen=0,gun_burstLen=0,health=255,ammo=255,god_burst_len=15,hit=0;
unsigned char red=0,green=0,blue=0;
unsigned long health_display_start=0,ammo_display_start=0;

void setup() {
  // put your setup code here, to run once:

  pinMode(from_jacket,INPUT_PULLUP);
  pinMode(from_trigger,INPUT_PULLUP);

  Serial.begin(115200);

  health=EEPROM.read(0x00);
  ammo=EEPROM.read(0x10);

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
  
  // Display Ammo and Health
  update_display(255-health,health,0,HEALTH_DISPLAY);
  update_display(255-ammo,ammo,0,AMMO_DISPLAY);
  

}

void loop() {
  // put your main code here, to run repeatedly:

  
  if(shoot!=0)
  {
     detachInterrupt(digitalPinToInterrupt(from_trigger)); 
     detachInterrupt(digitalPinToInterrupt(from_jacket));

     shoot=0; 
     
     ammo=ammo-damage;
     update_display(255-ammo,ammo,0,AMMO_DISPLAY);
     EEPROM.update(0x10,ammo);
     
     shoot_ir(gun_burstLen);
     attachInterrupt(digitalPinToInterrupt(from_trigger),fire,FALLING);
     attachInterrupt(digitalPinToInterrupt(from_jacket),falling_edge,FALLING);

  }
  else if(start!=0)
  {

    detachInterrupt(digitalPinToInterrupt(from_trigger));    
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
      health=255;
      ammo=255;
      
      Serial.println("God gun replinished my health");
      
      //update eeprom
      EEPROM.update(0x00,health);

      //update health bar
      update_display(0,255,0,0);
      update_display(0,255,0,1);
      
    }
    else if((delta>jacket_burstLen-1 && delta<jacket_burstLen+1) && health!=0)
    {      
      // Enemy fire ; Reduce health

      health=health-damage;
      // reduce health bar
      update_display(255-health,health,0,HEALTH_DISPLAY);
      
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
    attachInterrupt(digitalPinToInterrupt(from_trigger),fire,FALLING);


  }

  if(health==0)
  {
     // Make led strip blink 
     if((millis()-health_display_start)>300)
     {
       digitalWrite(R_HEALTH_DISPLAY,!digitalRead(R_HEALTH_DISPLAY));
       health_display_start=millis();
     }
     //****
     Serial.println("Health zero!!");
     //****
  }
  else if(ammo==0)
  {
    // Make led strip blink 
    
   if((millis()-ammo_display_start)>300)
   {
     digitalWrite(R_AMMO_DISPLAY,!digitalRead(R_AMMO_DISPLAY));
     ammo_display_start=millis();
   }
    
   //****
     Serial.println("Ammo zero!!");
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

void update_display(unsigned char r,unsigned char g, unsigned char b,unsigned char disp)     // disp takes value '0' when refering to health display, and '1' when refering to ammo display
{
  
  if(disp==0)
  {
      analogWrite(R_HEALTH_DISPLAY,r);
      analogWrite(G_HEALTH_DISPLAY,g);
      analogWrite(B_HEALTH_DISPLAY,b);
  }
  else if(disp==1)
  {
     analogWrite(R_AMMO_DISPLAY,r);
     analogWrite(G_AMMO_DISPLAY,g);
     analogWrite(B_AMMO_DISPLAY,b);
  }
  
}
