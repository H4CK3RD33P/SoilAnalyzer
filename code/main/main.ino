#include <ESPAsyncWebServer.h> //importing the Asynchronous server header

AsyncWebServer server(80); //server listening at port 80 i.e HTTP port

//defining function for invalid requests
void notFound(AsyncWebServerRequest *request){
  request->send(404,"text/plain","Not found");
}

void setup() {
  Serial.begin(115200); //baud rate
  WiFi.softAP("esp32",""); //hotspot with SSID and password is empty
  Serial.println("IP: "); 
  Serial.println(WiFi.softAPIP()); //IP of the microcontroller will be printed on serial monitor

}

void loop() {
  // put your main code here, to run repeatedly:

}
