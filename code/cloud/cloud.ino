#include <WiFiManager.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <ThingSpeak.h>
#include <WiFi.h>
#define TRIGGER_PIN 15
#define CHANNEL_ID 2002484
#define CHANNEL_WRITE_API_KEY "HB1XUG2HIXVI9HCR"
WiFiClient client;
int timeout = 180;

void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.begin(115200);
  Serial.println("\n Starting SoilCloud");
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  MDNS.begin("soilcloud");
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  //Initially device will try to connect with the previous credentials automatically
  //If connection is successful the config panel will not start (Hotspot)
  //If that network is unavailable it will start the config panel (Hotspot)
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("SoilCloud"); // anonymous ap
  if(!res) {
      Serial.println("Failed to connect");
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
  }
}

void loop() {
  // is the push button is pressed or the exsisting network goes off, turn on the config panel i.e Hotspot
  //press the push button if you wish to connect to some other network
  if ( digitalRead(TRIGGER_PIN) == LOW or (WiFi.status() != WL_CONNECTED)) {
    ondemandap();
  }
  // put your main code here, to run repeatedly:
  
  while(Serial2.available() and (WiFi.status() == WL_CONNECTED)){ //Check if data is available on serial port and WiFi is connected
    String str = Serial2.readString(); //read the data as string
    delay(2000);
    Serial.print("\n");
    Serial.println(str);
    Serial.print("\n");
    DynamicJsonDocument doc(200); 
    DeserializationError error = deserializeJson(doc,str);
    //the string is in JSON format so deserialize it in doc object
    if (error){ //if there is any error, print it
      Serial.print("Deserialization failed");
      Serial.println(error.c_str());
      return;
    }
    //extract the data into variables
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
    //Set the fields one by one
    ThingSpeak.setField(1,moisdata);
    ThingSpeak.setField(2,tempdata);
    ThingSpeak.setField(3,lightdata);
    //POST all the data at once (i.e in a single HTTP request)
    ThingSpeak.writeFields(CHANNEL_ID,CHANNEL_WRITE_API_KEY);
    delay(15000);
    //ThingSpeak allows new data point after 15 seconds minimum
    //3 million requests are free annually
  }

}

void ondemandap(){
  WiFiManager wm;    
  //reset settings i.e remove all previous credentials
  wm.resetSettings();
  // set configportal timeout
  wm.setConfigPortalTimeout(timeout);
  if (!wm.startConfigPortal("SoilCloud")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
}