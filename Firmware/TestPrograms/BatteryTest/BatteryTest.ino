/*
  BatteryTest
  This example converts battery voltage values to battery level in %.
 
  This example code is in the public domain.
*/

#define BATTERY_PIN  A0

void setup() {

  Serial.begin(9600);
  Serial.println("Start BatteryTest program");
}

void loop() {

  int sensorValue = analogRead(BATTERY_PIN);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.3V)
  float voltage = sensorValue * (3.2 / 1023.0);
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.print("V.");
  // Convert the analog reading (witch goes from 825 - 1000) to battery level (0 - 100%)
  float percent = (sensorValue - 825) * (100.0 / 175.0);
  Serial.print("\tBattery Level: ");
  Serial.print(percent);
  Serial.println("%");
  
  delay(1000);
}
