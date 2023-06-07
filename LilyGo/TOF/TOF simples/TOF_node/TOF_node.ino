#include <Wire.h>
#include <VL53L1X.h>

unsigned long prevmillis = 0;

// branco - gnd, preto - sda, vermelho - scl, verde - interrupt2, amarelo - interrupt1, azul - 5V

VL53L1X sensor;

void setup() {
  Wire.begin(21,22);
  Wire.setClock(400000); // use 400 kHz I2C
  sensor.setTimeout(50);

  if (!sensor.init(false))
  {
    Serial.println("Failed to detect and initialize sensor!");
  }
  else{
    sensor.setDistanceMode(VL53L1X::Long);
    sensor.setMeasurementTimingBudget(50000);
    sensor.startContinuous(50);
  }
  delay(1000);
}

void loop() {
 long vl53l1x = sensor.read();
 if((millis() - prevmillis) >= 1000){
    prevmillis = millis();
	Serial.print(NTP.getTimeDateString());
    //Serial.print(": WiFi RSSI:");
    //Serial.print(WiFi.RSSI());
    Serial.print(", Entfernung VL53L1X: ");
    Serial.print(vl53l1x);
    Serial.print(" mm");
	}
}