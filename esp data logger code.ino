#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// Replace with your network credentials
const char* ssid = "ROOM302";
const char* password = "12345678";

// Replace with your Google Apps Script Web App URL
const char* serverName = "https://script.google.com/macros/s/AKfycbwX9gQZEhOurTYfzwq8fqNHH0yh1p2_m_EYUZW8A3QF1TSrXLjlY722Udh8_0YhDyR5/exec";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;  // Send data every 60 seconds (1 minute)

// DHT11 settings
#define DHTPIN 15     // DHT11 data pin connected to GPIO 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  
  // Initialize the DHT sensor
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Connected to Wi-Fi");
}

void loop() {
  // Send data every 1 minute
  if ((millis() - lastTime) > timerDelay) {
    // Reading temperature and humidity from DHT11
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Check if any reads failed
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Log the data to serial monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // If connected to Wi-Fi, send data
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      // Prepare the URL with the data to send
      String url = String(serverName) + "?temperature=" + String(temperature) + "&humidity=" + String(humidity);

      // Start the request
      http.begin(url.c_str());
      
      // Send the GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Data sent successfully");
        Serial.println(response);
      } else {
        Serial.print("Error sending data: ");
        Serial.println(httpResponseCode);
      }

      // End the HTTP connection
      http.end();
    } else {
      Serial.println("Wi-Fi not connected");
    }

    // Update the timer
    lastTime = millis();
  }
}
