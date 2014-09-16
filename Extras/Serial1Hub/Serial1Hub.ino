/*
  SerialHub
  ATMEGA works like a hub between WiFly and PC through serial ports:
  Serial for PC and Serial1 for Wifly.
 
 This example code is in the public domain.
 */

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communications
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Start hub program");
}

// the loop routine runs over and over again forever:
void loop() {
  
  //re-transmit all the byte received from PC
  while (Serial.available()) {
    Serial1.write(Serial.read());
  }
  //re-transmit all the byte received from Wifly
  while (Serial1.available()) 
    Serial.write(Serial1.read());

  delay(1);  
}
