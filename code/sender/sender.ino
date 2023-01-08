// #include <HardwareSerial.h>

// HardwareSerial SerialPort(2);

void setup() {
  // put your setup code here, to run once:
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.begin(115200);
  Serial.println("SENDER: ");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial2.write("Hello from A");
  Serial.write("Sent: Hello from A\n");
  delay(2000);
}
