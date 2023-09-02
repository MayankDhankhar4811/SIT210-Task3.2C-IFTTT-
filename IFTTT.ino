#include <WiFiNINA.h>
#include <BH1750.h>
#include <Wire.h>

// Wi-Fi network credentials
char ssid[] = "Redmi";        // Replace with your Wi-Fi SSID
char pass[] = "mayank2005";   // Replace with your Wi-Fi password

WiFiClient client;            // Create a Wi-Fi client object
BH1750 lightMeter;            // Create a BH1750 light sensor object

char HOST_NAME[] = "maker.ifttt.com"; // IFTTT server hostname
String PATH_NAME = "/trigger/lightsensor/with/key/giaxwJZI9ala1sWuIBEs4kg0BueWF_NRMOCaOK1PTyw"; // IFTTT webhook path
String queryString = "?value1=57&value2=25"; // Initial query parameters for the webhook

void setup() {
  Serial.begin(9600);         // Initialize serial communication for debugging
  while (!Serial);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, pass);
  Wire.begin();

  while (true) {
    if (client.connect(HOST_NAME, 80)) { // Attempt to connect to the IFTTT server on port 80 (HTTP)
      Serial.println("Connection established successfully with server on port 80....");
      break;
    } else {
      Serial.println("Connection not established");
    }
    delay(500);
  }

  lightMeter.begin();           // Initialize the BH1750 light sensor
  Serial.println("Connected......");
}

void loop() {
  Serial.print("Receiving input from light sensor.......");
  float lux = lightMeter.readLightLevel(); // Read the light level from the sensor

  queryString += "?value1=";        // Prepare the query string
  queryString += lux;               // Append the current light level to the query string
  Serial.println(queryString);

  if (lux > 500) {                  // Check if the light level exceeds the threshold
    // Make an HTTP GET request to the IFTTT webhook
    client.println("GET " + PATH_NAME + queryString + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();

    // Wait for a response from the server and print it to the serial monitor
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }

    client.stop();                // Disconnect from the server
    Serial.println();
    Serial.println("Client-Server Disconnected....");
  }
  queryString = "";               // Reset the query string
  delay(3000);                   // Wait for 3 seconds before looping
}
