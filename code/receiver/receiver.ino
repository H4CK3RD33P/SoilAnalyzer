// #include <HardwareSerial.h>
// HardwareSerial Serial2(2);
#include <ArduinoJson.h>
#include <ThingSpeak.h>
#include <WiFi.h>
#define CHANNEL_ID 2002484
#define CHANNEL_WRITE_API_KEY "HB1XUG2HIXVI9HCR"

char ssid[] = "Subhodeep-wireless-2.4";
char pass[] = "700391239903"; 

WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.begin(115200);
  Serial.println("RECEIVER: ");
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  // put your main code here, to run repeatedly:

    // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  while(Serial2.available()){
    String str = Serial2.readString();
    delay(2000);
    Serial.print("\n");
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
    Serial.print("Lightdata: ");
    Serial.println(lightdata);
    Serial.print("Moisdata: ");
    Serial.println(moisdata);
    Serial.print("Tempdata: ");
    Serial.println(tempdata);
    delay(1000);
    ThingSpeak.setField(1,moisdata);
    ThingSpeak.setField(2,tempdata);
    ThingSpeak.setField(3,lightdata);
    ThingSpeak.writeFields(CHANNEL_ID,CHANNEL_WRITE_API_KEY);
    delay(15000);
  }

}
