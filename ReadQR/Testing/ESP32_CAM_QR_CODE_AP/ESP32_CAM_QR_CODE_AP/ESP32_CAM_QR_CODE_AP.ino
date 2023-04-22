#include <Arduino.h>
#include <ESP32QRCodeReader.h>

#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
//#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebSrv.h"

ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);

// Set these to your desired credentials.
//const char* ssid = "MobileRouter-0090";
//const char* password = "11639121";
const char* ssid = "ESP32 Acess Point";
const char* password = "123456789";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

String scan1 = "No Scan";
String scan2 = "No Scan";
String scan3 = "No Scan";

bool newQR = false;


// Initialize WiFi
void initWiFi() {
    // Wifi with no password
    //WiFi.softAP(ssid);
    // Wifi with password
    WiFi.softAP(ssid, password);
    //WiFi.begin(ssid, password);
    //WiFi.setSleep(false);
    /*
    delay(2000);
    // Set static IP
    IPAddress AP_LOCAL_IP(192, 168, 1, 1);
    IPAddress AP_GATEWAY_IP(192, 168, 1, 4);
    IPAddress AP_NETWORK_MASK(255, 255, 255, 0);
    if (!WiFi.softAPConfig(AP_LOCAL_IP, AP_GATEWAY_IP, AP_NETWORK_MASK)) {
        Serial.println("AP Config Failed");
        return;
    }
    */
    //Serial.println("WiFi AP setup");
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    /*
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    
    Serial.println(WiFi.localIP());
    */
}


String processor(const String& var){
  //getSensorReadings();
  //Serial.println(var);
  if(var == "scan1"){
    return String(scan1);
  }
  else if(var == "scan2"){
    return String(scan2);
  }
  else if(var == "scan3"){
    return String(scan3);
  }
  return String();
}


const char index_html[] PROGMEM = R"rawliteral(
</html><!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-image: linear-gradient(to bottom right, rgba(50, 127, 243, 0.5),rgba(176, 0, 216, 0.5), rgba(255, 2, 242, 0.6)); color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(400px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>FabLab Demo Project</h1>
    <h1>ESP32-CAM QR Code & AP WiFi demo</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-camera-retro" style="color:#05f92a;"></i> <i class="fas fa-qrcode" style="color:#05f92a;"></i> Latest Scan</p><p><span class="reading"><span id="s1">%SCAN1%</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-camera" style="color:#0105f1;"></i> <i class="fas fa-qrcode" style="color:#0105f1;"></i> Previous 1</p><p><span class="reading"><span id="s2">%SCAN2%</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-camera" style="color:#000000;"></i> <i class="fas fa-qrcode" style="color:#000000;"></i> Previous 2</p><p><span class="reading"><span id="s3">%SCAN3%</span></p>
      </div>
    </div>
  </div>
  <br>
  <br>
  <br>
  <b>Code Spliced together by FabLab technicaians from the following sources:</b>
  <p style="font-size:10pt">Rui Santos, Web server-sent events. Complete project details can be found <a href="https://RandomNerdTutorials.com/esp32-web-server-sent-events-sse/">here</a></p>
  <p style="font-size:10pt">Alvaro Viebrantz (alvarowolfx), ESP32 QR Code Reader project. Github project can be found <a href="https://github.com/alvarowolfx/ESP32QRCodeReader">here</a></p>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('scan1', function(e) {
  console.log("scan1", e.data);
  document.getElementById("s1").innerHTML = e.data;
 }, false);
 
 source.addEventListener('scan2', function(e) {
  console.log("scan2", e.data);
  document.getElementById("s2").innerHTML = e.data;
 }, false);
 
 source.addEventListener('scan3', function(e) {
  console.log("scan3", e.data);
  document.getElementById("s3").innerHTML = e.data;
 }, false);
}
</script>
</body>
</html>)rawliteral";


void onQrCodeTask(void *pvParameters)
{
  struct QRCodeData qrCodeData;

  while (true)
  {
    if (reader.receiveQrCode(&qrCodeData, 100))
    {
      Serial.println("Found QRCode");
      if (qrCodeData.valid)
      {
        String qrCodePayload = (const char *)qrCodeData.payload;
        Serial.print("Payload: ");
        Serial.println(qrCodePayload);  

        if (qrCodePayload != scan1)
        {
          scan3 = scan2;
          scan2 = scan1;
          scan1 = qrCodePayload;

          // Send Events to the Web Server with the Sensor Readings
          events.send("ping",NULL,millis());
          events.send(String(scan1).c_str(),"scan1",millis());
          events.send(String(scan2).c_str(),"scan2",millis());
          events.send(String(scan3).c_str(),"scan3",millis());
        }
              
      }
      else
      {
        Serial.print("Invalid: ");
        Serial.println((const char *)qrCodeData.payload);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  //your other setup stuff...
  
  // Init WiFi
  initWiFi();
  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();


  reader.setup();

  Serial.println("Setup QRCode Reader");

  reader.beginOnCore(1);

  Serial.println("Begin on Core 1");

  xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, NULL);
}

void loop()
{
  delay(100);
}
