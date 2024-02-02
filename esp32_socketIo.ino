#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebSocketsClient.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SocketIoClient.h>

WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
SocketIOclient socketIO;

#define USE_SERIAL Serial

const int redPin = 13;    // Red LED pin
const int greenPin = 12;  // Green LED pin
const int bluePin = 14;   // Blue LED pin

//WiFi SSID and password
const char *SSID = "Infinity";
const char *password = "password01";

void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case sIOtype_DISCONNECT:
      USE_SERIAL.printf("[IOc] Disconnected!\n");
      break;
    case sIOtype_CONNECT:
      USE_SERIAL.printf("[IOc] Connected to url: %s\n", payload);

      // join default namespace (no auto join in Socket.IO V3)
      socketIO.send(sIOtype_CONNECT, "/");
      break;
    case sIOtype_EVENT:
      {
        char *sptr = NULL;
        int id = strtol((char *)payload, &sptr, 10);
        USE_SERIAL.printf("[IOc] get event: %s id: %d\n", payload, id);
        if (id) {
          payload = (uint8_t *)sptr;
        }
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload, length);
        if (error) {
          USE_SERIAL.print(F("deserializeJson() failed: "));
          USE_SERIAL.println(error.c_str());
          return;
        }

        String eventName = doc[0];
        USE_SERIAL.printf("[IOc] event name: %s\n", eventName.c_str());

        if (eventName == "esp32-event") {
          // Access the nested JSON object
          JsonObject data = doc[1];

          // Extract RGB values from JSON
          int red = data["red"];
          int green = data["green"];
          int blue = data["blue"];

          // Control the LED color
          setColor(red, green, blue);
        }
      }
      break;
    case sIOtype_ACK:
      USE_SERIAL.printf("[IOc] get ack: %u\n", length);
      break;
    case sIOtype_ERROR:
      USE_SERIAL.printf("[IOc] get error: %u\n", length);
      break;
    case sIOtype_BINARY_EVENT:
      USE_SERIAL.printf("[IOc] get binary: %u\n", length);
      break;
    case sIOtype_BINARY_ACK:
      USE_SERIAL.printf("[IOc] get binary ack: %u\n", length);
      break;
  }
}


void setup() {
  //USE_SERIAL.begin(921600);
  USE_SERIAL.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  //Serial.setDebugOutput(true);
  USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  USE_SERIAL.println("Connecting Wifi...");
  WiFiMulti.addAP(SSID, password);

  //WiFi.disconnect();
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  String ip = WiFi.localIP().toString();
  USE_SERIAL.printf("[SETUP] WiFi Connected %s\n", ip.c_str());

  // server address, port and URL
  //socketIO.begin("10.11.100.100", 8880, "/socket.io/?EIO=4");
  //socketIO.beginSSL("esp32wscolorpicker-dev-fhdt.4.us-1.fl0.io", 443, "/socket.io/?EIO=4");
  socketIO.beginSSL("esp32-socketio.onrender.com", 443, "/socket.io/?EIO=4");

  // event handler
  socketIO.onEvent(socketIOEvent);
}

unsigned long messageTimestamp = 0;
void loop() {
  socketIO.loop();

  uint64_t now = millis();

  if (now - messageTimestamp > 2000) {
    messageTimestamp = now;

    // creat JSON message for Socket.IO (event)
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();

    // add evnet name
    // Hint: socket.on('event_name', ....
    array.add("esp32-event");

    // add payload (parameters) for the event
    JsonObject param1 = array.createNestedObject();
    param1["now"] = (uint32_t)now;

    // JSON to String (serializion)
    String output;
    serializeJson(doc, output);

    // Send event
    socketIO.sendEVENT(output);

    // Print JSON for debugging
    USE_SERIAL.println(output);
  }
}

void setColor(int red, int green, int blue) {
  Serial.printf("Setting color: R=%d, G=%d, B=%d\n", red, green, blue);
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}
