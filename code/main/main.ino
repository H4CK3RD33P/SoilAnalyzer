#include <ESPAsyncWebServer.h> //importing the Asynchronous server header
#include <ESPmDNS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

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

    DynamicJsonDocument doc(200); 
    DeserializationError error = deserializeJson(doc,message); 
    //Deserialize the JSON data into doc object
    //Store the error into error object (if any)
    if (error){ //if there is any error, print it
      Serial.print("Deserialization failed");
      Serial.println(error.c_str());
      return;
    }
  }
}

void setup() {
  Serial.begin(115200); //baud rate
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
    <!DOCTYPE html>
<html>
    <head>
        <title>Home</title>
        <style>
            input{
                margin: 5px 5px 5px 5px;
                display: inline;
            }

            button{
                height: 50px;
                font-style: italic;
                font-size: 20px;
            }
        </style>
    </head>
    <body>
        <h1>Soil Analyzer</h1>
        <h4>Enter the following parameters:</h4>
        <form action="#" id="form">
            <div id="temperature">
                Minimum temperature: <input type="text" name="mintemp">
                Maximum temperature: <input type="text" name="maxtemp">
            </div>
            <br>
            <div id="light">
                Minimum light: <input type="text" name="minlight">
                Maximum light: <input type="text" name="maxlight">
            </div>
            <br>
            <div id="moisture">
                Minimum moisture: <input type="text" name="minmois">
                Maximum moisture: <input type="text" name="maxmois">
            </div>
            <br>
        </form>
        <button id="submit" onclick="captureSend()">Test Soil</button>
        <script>
        var connection = new WebSocket('ws://'+location.hostname+':81/');
        function captureSend(){
            var formdata = document.getElementById('form');
            var jsondata = '{"mintemp":'+formdata["mintemp"].value+',"maxtemp":'+formdata["maxtemp"].value+',"minlight":'+formdata["minlight"].value+',"maxlight":'+formdata["maxlight"].value+',"minmois":'+formdata["minmois"].value+',"maxmois":'+formdata["maxmois"].value+'}';
            connection.send(jsondata);
        }
        </script>
    </body>
</html>
  )=====";
  //send_P -> sends the webpage saved in flash memory
    request->send_P(200,"text/html",webpage);
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
