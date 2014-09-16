#define GREEN 29
#define RED   28
#define BLUE  27
#define AMBAR 26

void setup() {                
  pinMode(GREEN, OUTPUT);     
  pinMode(RED, OUTPUT);     
  pinMode(BLUE, OUTPUT);     
  pinMode(AMBAR, OUTPUT);     
}

void loop() {
  digitalWrite(RED, HIGH);   // turn the RED LED on (HIGH is the voltage level)
  digitalWrite(BLUE, HIGH);   // turn the BLUE LED on (HIGH is the voltage level)
  digitalWrite(AMBAR, HIGH);   // turn the AMBAR LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(RED, LOW);    // turn the RED LED off by making the voltage LOW
  digitalWrite(BLUE, LOW);    // turn the BLUE LED off by making the voltage LOW
  digitalWrite(AMBAR, LOW);    // turn the AMBAR LED off by making the voltage LOW
  delay(1000);               // wait for a second
}
