#include <Arduino.h>
#include <NewPing.h>

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <WiFiClient.h>

const byte TRIGGER_PIN = D1;
const byte ECHO_PIN = D2;

const int MAX_DISTANCE = 400;
const int TEMPERATURE = 15;
const int HUMIDITY = 80;

// the speed of sound is 340 m/s when air temp is 15 degrees celsius
int SOUND_SPEED = 331.4 + (0.606 * TEMPERATURE) + (0.0124 * HUMIDITY);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
float duration, distance;

const char *ssid = "Ultrasonic Ruler";
const char *password = "thereisnospoon";

ESP8266WebServer server(80);

WebSocketsServer webSocket = WebSocketsServer(81);

char html_template[] PROGMEM = R"=====(
<html lang="en">
   <head>
      <meta charset="utf-8">
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <title>Ultrasonic Ruler</title>
      <script>
        socket = new WebSocket("ws:/" + "/" + location.host + ":81");
        socket.onopen = function(e) {  console.log("[socket] socket.onopen "); };
        socket.onerror = function(e) {  console.log("[socket] socket.onerror "); };
        socket.onmessage = function(e) {
            console.log("[socket] " + e.data);
            document.getElementById("distance_value").innerHTML = e.data;
        };
      </script>
   </head>
   <body style="max-width:400px;margin: auto;font-family:Arial, Helvetica, sans-serif;text-align:center">
      <div><h1><br />Distance</h1></div>
      <div style="font-size:4rem"><span id="distance_value"></span><span>&nbsp;cm</span></div>
<p>accurate between 2 and 400 cm</p>
   </body>
</html>
)=====";

void handleMain() { server.send_P(200, "text/html", html_template); }

void handleNotFound() {
  server.send(404, "text/html", "<html><body><h1>404</h1></body></html>");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t length) {

  switch (type) {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    break;

  case WStype_CONNECTED: {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0],
                  ip[1], ip[2], ip[3], payload);
    // send message to client
    webSocket.sendTXT(num, "0");
  } break;

  case WStype_TEXT:
    Serial.printf("[%u] get Text: %s\n", num, payload);
    // send message to client
    // webSocket.sendTXT(num, "message here");
    // send data to all connected clients
    // webSocket.broadcastTXT("message here");
    break;

  case WStype_BIN:
    Serial.printf("[%u] get binary length: %zu\n", num, length);
    hexdump(payload, length);
    // send message to client
    // webSocket.sendBIN(num, payload, length);
    break;
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleMain);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  duration = sonar.ping();
  distance = (duration / 2) * SOUND_SPEED / 10000;

  int distance_int = static_cast<int>(distance);
  String distanceString = (String)distance_int;

  server.handleClient();

  if (distance > 0) {
    webSocket.broadcastTXT(distanceString);
    Serial.println(distance);
  }
  delay(100);
}
