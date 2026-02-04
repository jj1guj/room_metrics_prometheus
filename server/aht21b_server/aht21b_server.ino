/*
 WiFi Web Server get temperature and humidity
 */

#include <WiFi.h>
#include "aht21b.h"
#include "config.h"

#define USER_LED_PIN 15

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const String label = ROOM_NAME;
int a = 0;

NetworkServer server(80);
Sensor sensor;

// グローバル変数としてタスク管理用のものを追加
volatile bool fetchRequested = false;
volatile bool fetchDone = false;

// fetch用タスク
void fetchTask(void *param) {
  while (true) {
    if (fetchRequested) {
      sensor.fetch();
      fetchDone = true;
      fetchRequested = false;
    }
    delay(10);
  }
}

// "192.168.1.7" のような文字列をIPAddressに変換する
IPAddress parseIP(const String &ipStr) {
  int parts[4] = {0};
  sscanf(ipStr.c_str(), "%d.%d.%d.%d", &parts[0], &parts[1], &parts[2], &parts[3]);
  return IPAddress(parts[0], parts[1], parts[2], parts[3]);
}

void setup() {
  sensor.begin();
  xTaskCreate(fetchTask, "FetchTask", 4096, NULL, 1, NULL);

  Serial.begin(115200);
  pinMode(USER_LED_PIN, OUTPUT);  // set the LED pin mode
  digitalWrite(USER_LED_PIN, LOW);

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);


  String ip = WIFI_IP;
  String gateway = WIFI_GATEWAY;
  String subnet = WIFI_SUBNET;
  if (ip != "" && gateway != "" && subnet != "") {
    WiFi.config(parseIP(ip), parseIP(gateway), parseIP(subnet));
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client

    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println();

            fetchDone = false;
            fetchRequested = true;
            while (!fetchDone) {
              delay(10);
            }

            // the content of the HTTP response follows the header:
            String json = "{";
            json += "\"temp\":" + String(sensor.temp) + ", ";
            json += "\"hum\":" + String(sensor.hum) + ", ";
            json += "\"label\":\"" + label + "\"";
            json += "}";
            client.print(json);
            ++a;

            // The HTTP response ends with another blank line:
            // client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
