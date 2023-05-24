#include <ESPAsyncWebServer.h> //importing the Asynchronous server header
#include <ESPmDNS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//#include <WiFiManager.h> //This is incompatible with AsyncTCP library
#include <ESPAsyncWiFiManager.h> //This the alternative for WiFimanager
#include <ThingSpeak.h>
#define MOISTURE_SENSOR 34
#define MOISTURE_LIGHT 15
#define TEMP_SENSOR 2
#define TEMP_LIGHT 4
#define PHOTO_SENSOR 33
#define PHOTO_LIGHT 5
#define CHANNEL_ID 2002484
#define CHANNEL_WRITE_API_KEY "HB1XUG2HIXVI9HCR"

const int light_low = 4096;
const int light_high = 0;
const int mois_low = 4095;
const int mois_high = 550;
int timeout = 180;
char thingspeakserver[] = "www.thingspeak.com";

WiFiClient client;
AsyncWebServer server(80); //server listening at port 80 i.e HTTP port
WebSocketsServer websockets(81); //web sockets server listening at port 81
Ticker timer;
OneWire onewire(TEMP_SENSOR);
DallasTemperature tempsensor(&onewire);
DNSServer dns;

void sendSensorVal(); //this function will be called after an interval of 17 second in loop [declared below]
void ondemandap();

int mintemp,maxtemp,minlight,maxlight,minmois,maxmois;
String moistest,lighttest,temptest;
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
    mintemp = doc["mintemp"];
    maxtemp = doc["maxtemp"];
    minlight = doc["minlight"];
    maxlight = doc["maxlight"];
    minmois = doc["minmois"];
    maxmois = doc["maxmois"];
  }
}

void setup() {
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.begin(115200); //baud rate
  WiFi.mode(WIFI_AP_STA);
  ondemandap();
  server.onNotFound(notFound); //calls the notFound() function upon requesting invalid page
  pinMode(MOISTURE_LIGHT, OUTPUT);
  pinMode(TEMP_LIGHT,OUTPUT);
  pinMode(PHOTO_LIGHT,OUTPUT);
  tempsensor.begin();
  

  
  //route
  //home page
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
                box-shadow: inset 0 0 0 0 purple;
                margin: 10px 10px 10px 10px;
                height: 40px;
                border-radius: 12px;
                transition: ease-out 0.5s;
                font-size: 15px;
                text-transform: uppercase;
                letter-spacing: 3px;
                font-weight: bolder;
                color:black
            }

            button:hover{
	            box-shadow: inset 300px 0 0 0 skyblue;
            }	

            h1{
                padding-bottom: 10px;
                background: -webkit-linear-gradient(#cb356b, #ffd608);
                -webkit-background-clip: text;
                -webkit-text-fill-color: transparent;
            }

            h4{
                padding-bottom: 10px;
                background: -webkit-linear-gradient(#b24592, #ffd608);
                -webkit-background-clip: text;
                -webkit-text-fill-color: transparent;
            }
            #container{
                background-color: rgba(32, 29, 29, 0.7);
                color: white;
                padding: 30px 30px 30px 30px;
                text-align: left;
                border-radius: 20px;
                border: 3px solid orange;
                font-size:110%;
                display: block;
                align-items: center;
                justify-content: center;
                width: 850px;
                top: 150px;
                bottom:100px;
                left: 0;
                right: 0;
                margin: auto;
                margin-top: 50px;
            }
            
            #bg{
                background: rgb(2,0,36);
                background: linear-gradient(90deg, rgba(2,0,36,1) 0%, rgba(98,9,121,1) 35%, rgba(0,212,255,1) 100%);
                background-size: cover;
                height: 95vh;
                background-position:absolute;
                padding: 10px 10px 10px 10px;
            }
            
            input{
                border-radius: 8px;
                border:3px solid yellowgreen
            }

            .desc{
                font-style: italic;
                color:#ffd608;
                font-weight: 600;
            }
        </style>
    </head>
    <body>
        <div id="bg">
            <div id="container">
                <h1>Soil Analyzer</h1>
                <h4>Enter the following parameters:</h4>
                <form action="#" id="form">
                    <div id="temperature">
                        Minimum temperature (in *C): <input type="text" name="mintemp">
                        Maximum temperature (in *C): <input type="text" name="maxtemp">
                        <br>
                        <small class="desc">(temperature must be between 1*C and 99*C)</small>
                    </div>
                    <br>
                    <div id="light">
                        Minimum light (in %): <input type="text" name="minlight">
                        Maximum light (in %): <input type="text" name="maxlight">
                        <br>
                        <small class="desc">(light must be between 0% and 100%)</small>
                    </div>
                    <br>
                    <div id="moisture">
                        Minimum moisture (in %): <input type="text" name="minmois">
                        Maximum moisture (in %): <input type="text" name="maxmois">
                        <br>
                        <small class="desc">(moisture must be between 1% and 100%)</small>
                    </div>
                    <br>
                </form>
                <button id="submit" onclick="captureSend()">Test Soil</button>
                <h1 id="error"></h1>
            </div>
        </div>
        <script>
        var connection = new WebSocket('ws://'+location.hostname+':81/');
        function captureSend(){
            var formdata = document.getElementById('form');
            if(formdata["mintemp"].value>0 && formdata["maxtemp"].value<100 && formdata["minmois"].value > 0 && formdata["maxmois"].value <=100 && formdata["minlight"].value >=0 && formdata["maxlight"].value <=100){
              var jsondata = '{"mintemp":'+formdata["mintemp"].value+',"maxtemp":'+formdata["maxtemp"].value+',"minlight":'+formdata["minlight"].value+',"maxlight":'+formdata["maxlight"].value+',"minmois":'+formdata["minmois"].value+',"maxmois":'+formdata["maxmois"].value+'}';
              formdata["mintemp"].value = "";
              formdata["maxtemp"].value = "";
              formdata["minlight"].value = "";
              formdata["maxlight"].value= "";
              formdata["minmois"].value = "";
              formdata["maxmois"].value = "";
              connection.send(jsondata);
              window.location = "http://"+location.hostname+"/results";
            }
            else{
              var errormsg = document.getElementById("error");
              errormsg.innerText = "Invalid parameters";
            }
        }
        </script>
    </body>
</html>
  )=====";
  //send_P -> sends the webpage saved in flash memory
    request->send_P(200,"text/html",webpage);
  });

  //results page
  server.on("/results",[](AsyncWebServerRequest *request){ 
      char resultpage[] PROGMEM = R"=====(
        <!DOCTYPE html>
<html>
    <head>
        <title>Results</title>
        <style>
            #moisval,#moistest,#tempval,#temptest,#lightval,#lighttest{
                display: inline;
                margin: 10px;    
            }
            
            #bg{
                background: rgb(2,0,36);
                background: linear-gradient(90deg, rgba(2,0,36,1) 0%, rgba(98,9,121,1) 35%, rgba(0,212,255,1) 100%);
                background-size: cover;
                height: 150vh;
                background-position:absolute;
                padding: 10px 10px 10px 10px;
            }

            #container{
                width: 750px;
                background-color: rgba(32, 29, 29, 0.7);
                color: white;
                padding: 30px 30px 30px 30px;
                text-align: center;
                border-radius: 20px;
                border: 3px solid orange;
                font-size:110%;
                display: block;
                align-items: center;
                justify-content: center;
                width: 800px;
                top: 150px;
                bottom:100px;
                left: 0;
                right: 0;
                margin: auto;
                margin-top: 50px;
            }

            #container div{
                width: 280px;
                background-color: rgba(78, 35, 180, 0.575);
                color: white;
                padding: 30px 30px 30px 30px;
                text-align: center;
                border-radius: 20px;
                border: 3px dashed rgba(255, 0, 200, 0.384);
                font-size:110%;
                display: block;
                align-items: center;
                justify-content: center;
                top: 150px;
                bottom:100px;
                left: 0;
                right: 0;
                margin: auto;
                margin-top: 50px;
            }

            h1{
                padding-bottom: 10px;
                background: -webkit-linear-gradient(#cb356b, #ffd608);
                -webkit-background-clip: text;
                -webkit-text-fill-color: transparent;
            }
        </style>
    </head>
    <body>
        <div id="bg">
            <div id="container">
                <h1>Results</h1>
                <div id="mois">
                  <h3>Soil Moisture: </h3>
                  <meter id="moismtr" value="100" min="0" max="100"></meter>
                  <h3 id="moisval">100</h3>%
                  <h3 id="moistest">FAIL</h3>
                </div>
                <div id="temp">
                    <h3>Soil Temperature: </h3>
                    <meter id="tempmtr" value="100" min="0" max="100"></meter>
                    <h3 id="tempval">100</h3>*C
                    <h3 id="temptest">FAIL</h3>
                </div>
                <div id="light">
                    <h3>Atmospheric Light: </h3>
                    <meter id="lightmtr" value="100" min="0" max="100"></meter>
                    <h3 id="lightval">100</h3>%
                    <h3 id="lighttest">FAIL</h3>
                </div>
            </div>
        </div>
        <script>

            // {"moisdata":60,"moistest":"PASS","tempdata":30,"temptest":"FAIL","lightdata":80,"lighttest":"PASS"}

            var connection = new WebSocket('ws://'+location.hostname+':81/');
            var moisdata = 0;
            var moistest = "";

            var tempdata = 0;
            var temptest = "";

            var lightdata = 0;
            var lighttest = "";
            

            connection.onmessage = function(event){
                var fulldata = event.data; //{"moisdata":60,"moistest":"PASS","tempdata":30,"temptest":"FAIL","lightdata":80,"lighttest":"PASS"}
                console.log(fulldata);
                var data = JSON.parse(fulldata);

                moisdata = data.moisdata;
                moistest = data.moistest;

                tempdata = data.tempdata;
                temptest = data.temptest;

                lightdata = data.lightdata;
                lighttest = data.lighttest;

                document.getElementById("moismtr").value = moisdata;
                document.getElementById("moisval").innerHTML = moisdata;
                document.getElementById("moistest").innerHTML = moistest;

                document.getElementById("tempmtr").value = tempdata;
                document.getElementById("tempval").innerHTML = tempdata;
                document.getElementById("temptest").innerHTML = temptest;

                document.getElementById("lightmtr").value = lightdata;
                document.getElementById("lightval").innerHTML = lightdata;
                document.getElementById("lighttest").innerHTML = lighttest;
            }
        </script>
    </body>
</html>
      )=====";

    request->send_P(200,"text/html",resultpage);
  });
  
  websockets.onEvent(webSocketEvent); //call the webSocketEvent() function on any websocket event 
  timer.attach(17,sendSensorVal); //attach timer with sendSensorVal with interval of 17 [this function will be called after every 17 second]
  //start servers
  server.begin(); //start the web server
  MDNS.begin("soilanalyzer"); //set up local domain name server with hostname -> soilanalyzer.local
  websockets.begin(); //start the websockets server
  WiFi.softAP("SoilAnalyzer",""); //hotspot with SSID and password is empty
  Serial.println("IP: "); 
  Serial.println(WiFi.softAPIP()); //IP of the microcontroller will be printed on serial monitor
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  // put your main code here, to run repeatedly:
  websockets.loop(); //continuously monitor 

}

//function declaration of sendSensorVal
void sendSensorVal(){
  char buf[100];
  int moisdata_raw = analogRead(MOISTURE_SENSOR);
  int moisdata = map(moisdata_raw,mois_low,mois_high,0,100);
  if (moisdata>=minmois && moisdata<=maxmois && moisdata!=0){
    moistest = "PASS";
    digitalWrite(MOISTURE_LIGHT,HIGH);
  }
  else{
    moistest = "FAIL";
    digitalWrite(MOISTURE_LIGHT,LOW);
  }

  tempsensor.requestTemperatures();
  float tempdata  = tempsensor.getTempCByIndex(0);
  if(tempdata>=mintemp && tempdata<=maxtemp){
    temptest = "PASS";
    digitalWrite(TEMP_LIGHT,HIGH);
  }
  else{
    temptest = "FAIL";
    digitalWrite(TEMP_LIGHT,LOW);
  }

  int lightdata_raw = analogRead(PHOTO_SENSOR);
  int lightdata = map(lightdata_raw,light_low,light_high,0,100);

  if(lightdata>=minlight && lightdata<=maxlight){
    lighttest = "PASS";
    digitalWrite(PHOTO_LIGHT,HIGH);
  }
  else{
    lighttest = "FAIL";
    digitalWrite(PHOTO_LIGHT,LOW);
  }

  String JSON_data_min = "{\"moisdata\":";
          JSON_data_min+= moisdata;
          JSON_data_min+=",\"tempdata\":";
          JSON_data_min+= tempdata;  
          JSON_data_min+=",\"lightdata\":";
          JSON_data_min+= lightdata;
          JSON_data_min+="}";
  
  JSON_data_min.toCharArray(buf, JSON_data_min.length()+1);
  Serial2.write(buf);

  String JSON_data = "{\"moisdata\":";
          JSON_data+= moisdata;
          JSON_data+=",\"moistest\":";
          JSON_data+="\""+moistest+"\"";
          JSON_data+=",\"tempdata\":";
          JSON_data+= tempdata;  
          JSON_data+=",\"temptest\":";
          JSON_data+="\""+temptest+"\"";
          JSON_data+=",\"lightdata\":";
          JSON_data+= lightdata;
          JSON_data+=",\"lighttest\":";
          JSON_data+="\""+lighttest+"\"";
          JSON_data+="}";

          // {"moisdata":60,"moistest":"PASS","tempdata":30,"temptest":"FAIL","lightdata":80,"lighttest":"PASS"}
  
  Serial.println(JSON_data);
  websockets.broadcastTXT(JSON_data); //send the JSON data to all clients i.e broadcast      
  if(client.connect(thingspeakserver,80)){
      //internet is available
      //digitalWrite(THINGSPEAKLED,HIGH);
      //Set the fields one by one
      ThingSpeak.setField(1,moisdata);
      ThingSpeak.setField(2,tempdata);
      ThingSpeak.setField(3,lightdata);
      //POST all the data at once (i.e in a single HTTP request)
      ThingSpeak.writeFields(CHANNEL_ID,CHANNEL_WRITE_API_KEY);
      //ThingSpeak allows new data point after 15 seconds minimum
      //3 million requests are free annually
    }
    else{
      //digitalWrite(THINGSPEAKLED,LOW);
      Serial.println("Could not connect to server");
      //ondemandap();
    }
}

void ondemandap(){
  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.resetSettings();
  wifiManager.setTimeout(timeout);

  if (!wifiManager.startConfigPortal("SoilCloud")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
}