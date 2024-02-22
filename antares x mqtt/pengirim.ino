#include <lorawan.h>
#include <SPI.h>
#include <MFRC522.h>
#include <AntaresESPHTTP.h>    // Include the AntaresESP8266HTTP library




#define ACCESSKEY "03fd039bd6554101:c3be4620d72ab8bb"    // Replace with your Antares account access key
#define WIFISSID "Bandits of Bandwidth"      // Replace with your Wi-Fi SSID
#define PASSWORD "qwerty123"  // Replace with your Wi-Fi password

#define projectName "smartGateParking"     // Antares project name
#define deviceNameSensor "sensorDIgital"   // Name of the device sending sensor data
// #define deviceNamePostman "YOUR-DEVICE-NAME-2"  // Name of the device receiving data

#define RST_PIN         0          // Configurable, see typical pin layout above
#define SS_PIN          16         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
AntaresESPHTTP antares(ACCESSKEY);


//ABP Credentials
/*
  Notes:
  - select ABP Activation on ANTARES
  - select inherit to generate your keys
  - nwkSKey: 32 digit hex, you can put 16 first digits by first 16 digits your access key and add 16 digits with 0 (ex : abcdef01234567890000000000000000)
  - appSKey: 32 digit hex, put 16 first digits by 0 and put last 16 digits by last 16 digit your access key (ex : 0000000000000000abcdef0123456789)
*/
const char *devAddr = "89eb8336"; // Replace with the Device Address that you have in the Antares console
const char *nwkSKey = "73a7f04d15eecf7ca0143e1156a04eb8"; // Replace with the Network Session Key that you have in the Antares console
const char *appSKey = "392e8b3a561a2f806e36cf2d312a56f8"; // Replace with the Application Session Key that you have in the Antares console

const unsigned long interval = 10000;    // 10 s interval to send message
unsigned long previousMillis = 0;  // will store last time message sent
unsigned int counter = 0;     // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;
int channel;
String testData;       // String to hold received data from Antares
String lastData = "";  // String to store last received data

#if defined (ARDUINO_ESP32_DEV)
const sRFM_pins RFM_pins = {
  .CS = 5,    //LYNX32 to RFM NSS
  .RST = 0,   //LYNX32 to RFM RST
  .DIO0 = 27, //LYNX32 to RFM DIO0
  .DIO1 = 2,  //LYNX32 to RFM DIO1
};
#elif defined (ARDUINO_B_L072Z_LRWAN1)
const sRFM_pins RFM_pins = {
  .CS = PA15,   //STM32L0 to NSS
  .RST = PC0,   //STM32L0 to RST
  .DIO0 = PB4,  //STM32L0 to DIO0
  .DIO1 = PB1,  //STM32L0 to DIO1
};
#else
#error *** ONLY SUPPORTS LYNX32 AND B-L072Z-LRWAN1 ***
#endif

void setup() {

#if defined (ARDUINO_B_L072Z_LRWAN1)
  SPI.setMOSI(PA7);
  SPI.setMISO(PA6);
  SPI.setSCLK(PB3);
  SPI.begin();
#elif !defined (ARDUINO_ESP32_DEV)
  #error *** ONLY SUPPORTS LYNX32 AND B-L072Z-LRWAN1 ***
#endif

  // Setup loraid access
  Serial.begin(115200);
  delay(2000);
  if (!lora.init()) {
    Serial.println("RFM95 not detected");
    delay(5000);
    return;
  }

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_A);

  // Set Data Rate
  lora.setDataRate(SF10BW125);

  // set channel to random
  lora.setChannel(MULTI);

  // Set TxPower to 15 dBi (max)
  lora.setTxPower1(15);

  // Put ABP Key and DevAddress here
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);
  antares.setDebug(true);                      // Enable Antares library debug mode
  antares.wifiConnection(WIFISSID, PASSWORD);
  while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  // Check interval overflow
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == "53 41 03 AA"){
    antares.add("user", "reksa");                    // Add temperature data to 
    antares.send(projectName, deviceNameSensor);
    delay(2000);
    antares.add("user", "none");                    // Add temperature data to 
    antares.send(projectName, deviceNameSensor);
    delay(2000);
  }if (content.substring(1) == "53 77 8A 0D"){
    antares.add("user", "Agus");                    // Add temperature data to 
    antares.send(projectName, deviceNameSensor);
    delay(2000);
    antares.add("user", "none");                    // Add temperature data to 
    antares.send(projectName, deviceNameSensor);
    delay(2000);
  }if (content.substring(1) == "93 A9 2E 10"){
    antares.add("user", "Algi");                    // Add temperature data to 
    antares.send(projectName, deviceNameSensor);
    delay(2000);
    antares.add("user", "none");                    // Add temperature data to 
    antares.send(projectName, deviceNameSensor);
    delay(2000);
  }

}
