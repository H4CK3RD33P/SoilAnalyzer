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
  char buf[100];
  int moisdata = 50;
  double tempdata = 33.33;
  int lightdata = 70;
  String JSON_data = "{\"moisdata\":";
          JSON_data+= moisdata;
          JSON_data+=",\"tempdata\":";
          JSON_data+= tempdata;  
          JSON_data+=",\"lightdata\":";
          JSON_data+= lightdata;
          JSON_data+="}";
  
  JSON_data.toCharArray(buf, JSON_data.length()+1);
  Serial2.write(buf);
  Serial.write(buf);
  Serial.println();
  delay(2000);
}
