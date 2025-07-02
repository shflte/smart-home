#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SLC_Deco";
const char* password = "89706888";

const char* mqtt_server = "192.168.50.144";
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";

const int STOP_PIN = D0;
const int CLOSE_PIN = D2;
const int OPEN_PIN = D3;

const int SLIDE_TIME = 33 * 1000;
const int PRESS_TIME = 1200;
const int CHILL_TIME = 500;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected\nIP address: \n");
  Serial.println(WiFi.localIP());
}

void pressButton(int pin) {
  digitalWrite(pin, LOW);
  delay(PRESS_TIME);
  digitalWrite(pin, HIGH);
  delay(CHILL_TIME);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (strcmp(topic, "gate/slide") != 0) {
    Serial.print("Invalid message\n");
  }

  if (message == "open") {
    pressButton(OPEN_PIN);
  } else if (message == "close") {
    pressButton(CLOSE_PIN);
  } else if (message == "stop") {
    pressButton(STOP_PIN);
  } else {
    Serial.print("Invalid message\n");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("gate_opener", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("gate/slide");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  pinMode(OPEN_PIN, OUTPUT);
  pinMode(STOP_PIN, OUTPUT);
  pinMode(CLOSE_PIN, OUTPUT);
  digitalWrite(OPEN_PIN, HIGH);
  digitalWrite(STOP_PIN, HIGH);
  digitalWrite(CLOSE_PIN, HIGH);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}