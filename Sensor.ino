
#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <SFE_BMP180.h>

#define WIFI_SSID "Esso"
#define WIFI_PASSWORD "esso2020"
#define API_KEY "AIzaSyDPoPpYQjPjqw2UjpPhZCdlsYmGP2sBOLE"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://aeroorgans-70146-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
SFE_BMP180 bmp180;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase sign up successful");
    signupOK = true;
  }
  else {
    Serial.printf("Firebase sign up failed: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  dht.begin();
  bmp180.begin();
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    double pressure;
    double temperatureBMP;

    if (bmp180.getPressure(pressure, temperatureBMP)) {
      Serial.println("Pressure data retrieved");
    }
    else {
      Serial.println("Failed to retrieve pressure data");
    }

    if (Firebase.RTDB.setFloat(&fbdo, "sensor/temperature", temperature)) {
      Serial.println("Temperature data sent to Firebase");
    }
    else {
      Serial.println("Failed to send temperature data to Firebase");
    }

    if (Firebase.RTDB.setFloat(&fbdo, "sensor/humidity", humidity)) {
      Serial.println("Humidity data sent to Firebase");
    }
    else {
      Serial.println("Failed to send humidity data to Firebase");
    }

    if (Firebase.RTDB.setDouble(&fbdo, "sensor/pressure", pressure)) {
      Serial.println("Pressure data sent to Firebase");
    }
    else {
      Serial.println("Failed to send pressure data to Firebase");
    }
  }
}