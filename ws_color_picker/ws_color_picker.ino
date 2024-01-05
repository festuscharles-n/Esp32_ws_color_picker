#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

const int redPin = 13;    // Red LED pin
const int greenPin = 12;  // Green LED pin
const int bluePin = 14;   // Blue LED pin

//WiFi SSID and password
const char *SSID = "Infinity";
const char *password = "password";

void setup() {
  Serial.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  WiFiMulti.addAP(SSID, password);
  Serial.println("Connecting Wifi...");
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  if (WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  //webSocket.begin("192.168.63.99", 4000, "/");  // Replace with your actual server IP and port
  //webSocket.begin("esp32wscolorpicker-dev-fhdt.4.us-1.fl0.io", 4000, "/");
  webSocket.beginSSL("esp32wscolorpicker-dev-fhdt.4.us-1.fl0.io", 443, "/", "", "wss");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void loop() {
  webSocket.loop();
  delay(10);
}

void setColor(int red, int green, int blue) {
  Serial.printf("Setting color: R=%d, G=%d, B=%d\n", red, green, blue);
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Disconnected!");
      break;
    case WStype_CONNECTED:
      Serial.println("[WSc] Connected to WebSocket server");
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] Received text: %s\n", payload);
      
      // Parse JSON payload
      DynamicJsonDocument doc(256);
      deserializeJson(doc, (const char *)payload);

      // Extract RGB values from JSON
      int red = doc["red"];
      int green = doc["green"];
      int blue = doc["blue"];

      // Control the LED color
      setColor(red, green, blue);
      break;
  }
}
