#include <AntaresESPHTTP.h>    // Include the AntaresESP8266HTTP library
#include <Arduino.h>
#include <PubSubClient.h>

String mqttBroker = "broker.emqx.io";
WiFiClient client;
PubSubClient mqtt(client);


#define ACCESSKEY "03fd039bd6554101:c3be4620d72ab8bb"    // Replace with your Antares account access key
#define WIFISSID "Bandits of Bandwidth"      // Replace with your Wi-Fi SSID
#define PASSWORD "qwerty123"  // Replace with your Wi-Fi password

#define projectName "smartGateParking"     // Antares project name
// #define deviceNameSensor "YOUR-DEVICE-NAME-1"   // Name of the device sending sensor data
#define deviceNamePostman "sensorDIgital"  // Name of the device receiving data

AntaresESPHTTP antares(ACCESSKEY);  // Initialize AntaresESP8266HTTP with the access key

String testData;       // String to hold received data from Antares
String lastData = "";  // String to store last received data


void connect_mqtt();
void setup() {
  Serial.begin(115200);                        // Initialize serial communication
  antares.setDebug(true);                      // Enable Antares library debug mode
  antares.wifiConnection(WIFISSID, PASSWORD);  // Connect to WiFi using provided 
  mqtt.setServer(mqttBroker.c_str(), 1883);
}

void loop() {



    antares.get(projectName, deviceNamePostman);  // Get data from Antares
      testData = antares.getString("user");  // Get the "Test Data" field from the response

    if (antares.getSuccess()) {                   // Check if data retrieval was successful

      if (testData == "Agus")  // Check if the received data is different from the last one
      {
          if (!mqtt.connected())
          {
              connect_mqtt();
              Serial.println("MQTT Connected");
              mqtt.publish("smart/parking", "Mobil  Pak Agus Masuk Parkiran!");
              return;
          }
          mqtt.loop();
          mqtt.publish("smart/parking", "Mobil Pak Agus Masuk Parkiran!");
          delay(4000);
      
      }else if (testData == "reksa")  // Check if the received data is different from the last one
      {
          if (!mqtt.connected())
          {
              connect_mqtt();
              Serial.println("MQTT Connected");
              mqtt.publish("smart/parking", "Mobil Kak Reksa Masuk Parkiran!");
              return;
          }
          mqtt.loop();
          mqtt.publish("smart/parking", "Mobil Kak Reksa Masuk Parkiran!");
          delay(4000);
      
      }else if (testData == "Algi")  // Check if the received data is different from the last one
      {
          if (!mqtt.connected())
          {
              connect_mqtt();
              Serial.println("MQTT Connected");
              mqtt.publish("smart/parking", "Mobil Kak Algi Masuk Parkiran!");
              return;
          }
          mqtt.loop();
          mqtt.publish("smart/parking", "Mobil Kak Algi Masuk Parkiran!");
          delay(4000);
      
      }else if(testData == "Unknow"){
          if (!mqtt.connected())
          {
              connect_mqtt();
              Serial.println("MQTT Connected");
              mqtt.publish("smart/parking", "Ada Orang Yang Bukan Karyawan Yang Ingin Mencoba Masuk!!");
            return;
          }
          mqtt.loop();
          mqtt.publish("smart/parking", "Ada Orang Yang Bukan Karyawan Yang Ingin Mencoba Masuk!!");
          delay(4000);
        
      }
    }
}


void connect_mqtt()
{
    while (!mqtt.connected())
    {
        Serial.println("Connecting MQTT...");
        if (mqtt.connect("ESP8266Client-"))
        {
            mqtt.subscribe("smart/parking");
        }
    }
}
