//#include<EEPROM.h>
//#include<ESP8266WiFi.h>
//#incldue<ESP8266WiFiMulti.h>

#define from_jacket D6
#define motor_led_flash D7

void falling_edge();
void rising_edge();
void getting_shot();

// Player id: 1-> Team 1 ; 2-> Team 2;
const char id=1,damage=10;
volatile unsigned long start=0,stopp=0,delta=0,ml_start=0;
unsigned int burst_len=0,health=100,god_burst_len=10,hit=0;
const char* ssid="";
const char* password="";

//ESP8266WiFiMulti wifimulti;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  
  /*wifimulti.addAP(ssid,password);
  
  Serial.println("Connecting ...");
  
  while(wifimulti.run() != WL_CONNECTED)
  {
      delay(500);
      Serial.print(".");
  }
  Serial.println();
  
  Serial.print("SSID:");
  Serial.println(WiFi.SSID());
  Serial.print("IP:");
  Serial.println(WiFi.localIP());*/


  //health=EEPROM.read(0x00);

  attachInterrupt(digitalPinToInterrupt(from_jacket),falling_edge,FALLING);
//  attachInterrupt(digitalPinToInterrupt(from_jacket),rising_edge,RISING);

  switch(id)
  {
    case 1:
      burst_len=3;
      break;
    case 2:
      burst_len=1000;    
      break;
  }

}

void loop() {
  // put your main code here, to run repeatedly:

  if(start!=0)
  {
    detachInterrupt(digitalPinToInterrupt(from_jacket));

    while(digitalRead(D6)==0)
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


    
    if(delta>god_burst_len && delta<god_burst_len)
    {
      //God gun firing
      health=100;

      //update eeprom
      //EEPROM.update(0x00,health);

      //update health bar

      //update server
      
    }
    else if((delta>burst_len-1 && delta<burst_len+1) && health!=0)
    {      
      // Enemy fire ; Reduce health

      health=health-damage;
      // reduce health bar
      //******************************
      Serial.print("Health displayed on health bar:");
      Serial.println(health);
      //******************************
      
      // update eeprom
      //EEPROM.update(0x00,health);
      
      // update server
      //******************************
      Serial.println("Health updated on server");
      //******************************
      
      // vibrate and notify getting shot
      Serial.println("Motor vibrated");
      hit=1;
    }

    attachInterrupt(digitalPinToInterrupt(from_jacket),falling_edge,FALLING);
    //attachInterrupt(digitalPinToInterrupt(from_jacket),rising_edge,RISING);
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
  
  
//  attachInterrupt(digitalPinToInterrupt(from_jacket),falling_edge,FALLING);
//  attachInterrupt(digitalPinToInterrupt(from_jacket),rising_edge,RISING);  

}

void falling_edge()
{
  start=micros();  
}

void rising_edge()
{
  stopp=micros();
}



