#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Wi-Fi and MQTT Broker details
const char* ssid = "AIRCON1";
const char* password = "LT123456";
const char* mqtt_broker = "broker.emqx.io";
const int mqtt_port = 1883;
//const char* mqtt_topic = "Aartikanwar/sensors/dht";

// DHT Sensor setup
#define DHTPIN 4       // GPIO pin connected to DHT sensor
#define DHTTYPE DHT11  // Replace with DHT11 if using that sensor
DHT dht(DHTPIN, DHTTYPE);

// MQTT client setup
WiFiClient espClient;
PubSubClient client(espClient);

// Reconnect to MQTT broker if disconnected
void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();
  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected");

  // Connect to MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  String macAddress = WiFi.macAddress();
  String mqtt_topic1 = "Aartikanwar/sensors/status/" + macAddress; // Topic for status

  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature(); // Celsius
  float humidity = dht.readHumidity();

  // Check if reading is valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  delay(200);
  // Create JSON object
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  //doc["mac_id"] = macAddress.c_str();
  doc["timestamp"] = millis(); // Use system time or add RTC if available

  // Serialize JSON to a string
  char jsonBuffer[200];
  serializeJson(doc, jsonBuffer);

  // Publish JSON payload to MQTT topic
  client.publish(mqtt_topic1.c_str(), jsonBuffer);

  // Print JSON payload to Serial Monitor
  Serial.println("Published JSON payload:");
  Serial.println(jsonBuffer);

  delay(10000); // Wait for 10 seconds before sending the next reading
}
