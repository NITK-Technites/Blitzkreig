void setup() {
  // put your setup code here, to run once:

  pinMode(6,1);

}

void loop() {
  // put your main code here, to run repeatedly:

  for(int i=1;i<=2000/26;i++){
  digitalWrite(6,1);
  delayMicroseconds(13);
  digitalWrite(6,0);
  delayMicroseconds(13);}

//  digitalWrite(6,1);
//  delayMicroseconds(13);
//  digitalWrite(6,0);
//  delayMicroseconds(13);

  delay(1000);

}
