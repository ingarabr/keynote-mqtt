#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "config.h"

const char* ssid = wifi_ssid;
const char* password = wifi_pwd;
const char* mqtt_server = mqtt_broker;

#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

WiFiClient espClient;
PubSubClient client(espClient);

int BTN_NEXT = D6;
int BTN_PREV = D7;

int nextButtonState = 0;
int prevButtonState = 0;

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  setupWifiMsg();
  setupWifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback); //todo add create callback method

  pinMode(BTN_NEXT, INPUT);
  pinMode(BTN_PREV, INPUT);
  
  setupDoneMsg();
}

void setupWifiMsg() {
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Setting up wifi.");
  display.print("Connectiong to ");
  display.print(ssid);
  display.display();
}

void setupDoneMsg() {
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("We're ready now.");
  display.println("Enjoy the Itera conference!");
  display.display();
}

void setupWifi() {
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

void handleButton(int *state, int buttonId, const char *msg) {
  long currentState = digitalRead(buttonId);
  if (currentState != *state) {
    *state = currentState;
    Serial.print("new  state: "); Serial.println(*state);
    if (currentState == HIGH) {
      Serial.print("Button pushed, publish message: ");
      Serial.println(msg);
      client.publish("ingar/keynote", msg);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  handleButton(&nextButtonState, BTN_NEXT, "next");
  handleButton(&prevButtonState, BTN_PREV, "prev");
  
  delay(1000);
}
