// #include <HardwareSerial.h>
// HardwareSerial Serial2(2);
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
    delay(1000);
    Serial.println(str);
    Serial.print("\n");       
  }

}
