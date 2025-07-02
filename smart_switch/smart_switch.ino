#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

const char* ssid = "SLC_Deco";
const char* password = "89706888";

const char* mqtt_server = "192.168.50.144";
const int mqtt_port = 1883;
const char* pump_control_topic = "pump/balcony";
const char* pump_status_topic = "pump/balcony/status";

const int BUTTON_PIN = D5;
const int RELAY_PIN = 10;  // SD3
const int LED_PIN = D3;

bool pump_state = false;

WiFiClient espClient;
PubSubClient client(espClient);

void updateState(bool state) {
    pump_state = state;
    digitalWrite(RELAY_PIN, state ? LOW : HIGH);
    digitalWrite(LED_PIN, state ? HIGH : LOW);

    client.publish(pump_status_topic, state ? "on" : "off");

    EEPROM.write(0, state ? 1 : 0);
    EEPROM.commit();
}

void handleButtonPress() {
    static unsigned long lastPressTime = 0;
    static bool lastButtonState = HIGH;

    unsigned long currentTime = millis();
    bool buttonState = digitalRead(BUTTON_PIN);

    // debounced
    if (buttonState != lastButtonState && currentTime - lastPressTime > 50) {
        lastPressTime = currentTime;

        if (buttonState == LOW) {
            updateState(!pump_state);
        }
    }

    lastButtonState = buttonState;
}

void callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    if (String(topic) == pump_control_topic) {
        if (message == "on") {
            updateState(true);
        } else if (message == "off") {
            updateState(false);
        } else {
            Serial.println("Invalid MQTT message");
        }
    }
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("pump_controller")) {
            Serial.println("connected");
            client.subscribe(pump_control_topic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);

    EEPROM.begin(1);
    pump_state = EEPROM.read(0);
    updateState(pump_state);

    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    reconnect();
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    handleButtonPress();
}