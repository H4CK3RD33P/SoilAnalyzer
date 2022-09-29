#include <ESPAsyncWebServer.h> //importing the Asynchronous server header
#include <ESPmDNS.h>
#define LED1 13
AsyncWebServer server(80); //server listening at port 80 i.e HTTP port

//defining function for invalid requests
void notFound(AsyncWebServerRequest *request){
  request->send(404,"text/html","Not found");
}

void setup() {
  Serial.begin(115200); //baud rate
  pinMode(LED1,OUTPUT);
  WiFi.softAP("esp32",""); //hotspot with SSID and password is empty
  Serial.println("IP: "); 
  Serial.println(WiFi.softAPIP()); //IP of the microcontroller will be printed on serial monitor
  server.onNotFound(notFound); //calls the notFound() function upon requesting invalid page

  
  //route
  server.on("/",[](AsyncWebServerRequest *request){
    //this webpage will be sent upon receiving the request at root
    //R"=====()=====" is raw string
    //location.hostname -> hostname of the server
    //window.location -> location redirection
    //PROGMEM -> saves the array into flash memory
    char webpage[700] PROGMEM = R"=====(
    <DOCTYPE! html>
    <html>
      <head>
        <title>Home</title>
      </head>
      <body>
        <h3>LED control</h3>
        <button onclick="window.location='http://'+location.hostname+'/led/on'">ON</button>
        <button onclick="window.location='http://'+location.hostname+'/led/off'">OFF</button>
      </body>
    </html>
  )=====";
  //send_P -> sends the webpage saved in flash memory
    request->send_P(200,"text/html",webpage);
  });

  server.on("/led/on",[](AsyncWebServerRequest *request){
    digitalWrite(LED1,HIGH);
    //request->send(200,"text/html","LED ON");
    request->redirect("/");
  });

  server.on("/led/off",[](AsyncWebServerRequest *request){
    digitalWrite(LED1,LOW);
    //request->send(200,"text/html","LED OFF");
    request->redirect("/");
  });
  
  
  //start servers
  server.begin(); //start the web server
  MDNS.begin("soilanalyzer"); //set up local domain name server with hostname -> soilanalyzer.local
}

void loop() {
  // put your main code here, to run repeatedly:

}
