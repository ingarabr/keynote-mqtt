#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Servo.h>

#include "config.h"

const char* ssid = wifi_ssid;
const char* password = wifi_pwd;
const char* mqtt_server = mqtt_broker;

#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

WiFiClient espClient;
PubSubClient client(espClient);

Servo btnServo;

int BTN_NEXT = D6;
int BTN_PREV = D7;
int SERVO_PIN = D3;
int SERVO_BASE = 94;

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

  setupServo();
  
  setupDoneMsg();
}

void setupWifiMsg() {
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("\nSetting up wifi.");
  display.print("SSID: ");
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
  display.println("\nWe're ready now.");
  display.println("Enjoy the Itera conf!");
  display.display();
}

void iteraMsg() {
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("\n  Itera conference");
  display.println("        Enjoy!");
  display.display();
}

void slideMsg(const char *slideDirection) {
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("\n Change slide:\n     ");
  display.print(slideDirection);
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

void setupServo() {
  btnServo.attach(SERVO_PIN);
  btnServo.write(SERVO_BASE);
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

boolean handleButton(int *state, int buttonId, const char *msg) {
  long currentState = digitalRead(buttonId);
  if (currentState != *state) {
    *state = currentState;
    Serial.print("new  state: "); Serial.println(*state);
    if (currentState == HIGH) {
      slideMsg(msg);
      Serial.print("Button pushed, publish message: ");
      Serial.println(msg);
      client.publish("ingar/keynote", msg);
      return true;
    } else {
      btnServo.write(SERVO_BASE);
      iteraMsg();
    }
  }
  return false;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (handleButton(&nextButtonState, BTN_NEXT, "next")) {
    delay(500);
    btnServo.write(SERVO_BASE + 51);
  } else if (handleButton(&prevButtonState, BTN_PREV, "prev")) {
    delay(500);
    btnServo.write(SERVO_BASE - 51);
  }
}
