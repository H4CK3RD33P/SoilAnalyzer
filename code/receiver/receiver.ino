// #include <HardwareSerial.h>
// HardwareSerial Serial2(2);
#include <ArduinoJson.h>

void setup() {
  // put your setup code here, to run once:
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.begin(115200);
  Serial.println("RECEIVER: ");

}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial2.available()){
    String str = Serial2.readString();
    delay(2000);
    Serial.println(str);
    Serial.print("\n");
    DynamicJsonDocument doc(200); 
    DeserializationError error = deserializeJson(doc,str); 
    if (error){ //if there is any error, print it
      Serial.print("Deserialization failed");
      Serial.println(error.c_str());
      return;
    }
    int lightdata = doc["lightdata"];
    int moisdata = doc["moisdata"];
    float tempdata = doc["tempdata"];
    Serial.println(lightdata);
    Serial.println(moisdata);
    Serial.println(tempdata);
    delay(1000);
  }

}
