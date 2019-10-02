void setup() {
  // put your setup code here, to run once:

  pinMode(2,INPUT_PULLUP);
  Serial.begin(115200);

//  attachInterrupt(
}

unsigned long start=0,stopp=0;

void loop() {
  // put your main code here, to run repeatedly:

  if(digitalRead(2)==0)
  {
    start=millis();
    while(digitalRead(2)==0)
    {
      ;
    }
    stopp=millis();

        Serial.println(stopp-start);

  }
  

}
