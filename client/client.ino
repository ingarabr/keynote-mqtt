#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

const char* ssid = wifi_ssid;
const char* password = wifi_pwd;
const char* mqtt_server = mqtt_broker;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback); //todo add create callback method
}

void setup_wifi() {
  delay(10);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("keynote-client")) {
      Serial.println("connected");

      client.publish("ingar/msg", "Connected");      
//      client.subscribe("ingar/msg");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
   if (now - lastMsg > 2000) {
    Serial.print("Publish message: next\n");
     lastMsg = now;
     client.publish("ingar/keynote", "next");
   }
}
