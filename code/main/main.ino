#include <ESPAsyncWebServer.h> //importing the Asynchronous server header
#include <ESPmDNS.h>
#include <WebSocketsServer.h>
#define LED1 13

AsyncWebServer server(80); //server listening at port 80 i.e HTTP port
WebSocketsServer websockets(81); //web sockets server listening at port 81

//defining function for invalid requests
void notFound(AsyncWebServerRequest *request){
  request->send(404,"text/html","Not found");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length){
  //num -> client number | type -> websocket type | payload -> data from client | length -> length of payload
  switch(type){
    case WStype_DISCONNECTED: //if the client gets disconnected
    Serial.printf("[%u] Disconnected!\n",num); //print client number
  break;
  case WStype_CONNECTED: { //When a new client gets connected
    IPAddress ip = websockets.remoteIP(num); //get the IP address of the client
    Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n",num,ip[0],ip[1],ip[2],ip[3],payload);
    //print connected client information
    //then send a message from server to that client
    websockets.sendTXT(num,"Connected");
  }
  break;
  case WStype_TEXT: //When the client sends data
    Serial.printf("[%u] get Text: %s\n",num,payload);
    String message = String((char*)(payload));
    Serial.println(message);

  if (message=="LED is on"){
    digitalWrite(LED1,HIGH);
  }
  if(message=="LED is off"){
    digitalWrite(LED1,LOW);
  }

  }
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
    //var connection establishes the connection for websocket from client to serveer at port 81
    //connection.send() sends message from client to server
    char webpage[] PROGMEM = R"=====(
    <DOCTYPE! html>
    <html>
    <script>
      var connection = new WebSocket('ws://'+location.hostname+':81/');
      function ledon(){
        console.log("LED is on");
        connection.send("LED is on");
      }

      function ledoff(){
        console.log("LED is off");
        connection.send("LED is off");
      }

    </script>
      <head>
        <title>Home</title>
      </head>
      <body>
        <h3>LED control</h3>
        <button onclick=ledon()>ON</button>
        <button onclick=ledoff()>OFF</button>
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

   websockets.onEvent(webSocketEvent); //call the webSocketEvent() function on any websocket event 
  
  
  //start servers
  server.begin(); //start the web server
  MDNS.begin("soilanalyzer"); //set up local domain name server with hostname -> soilanalyzer.local
  websockets.begin(); //start the websockets server
}

void loop() {
  // put your main code here, to run repeatedly:
  websockets.loop(); //continuously monitor 

}
