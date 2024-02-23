#include <SPI.h>
#include <MFRC522.h>
 #include <ESP32Servo.h> 
 #include <Wire.h> 
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <lorawan.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include <Ultrasonic.h>
#include <AntaresESPHTTP.h>     // Include the AntaresESP32HTTP library

#define ACCESSKEY "03fd039bd6554101:c3be4620d72ab8bb"       // Replace with your Antares account access key
#define WIFISSID "Bandits of Bandwidth"         // Replace with your Wi-Fi SSID
#define PASSWORD "qwerty123"     // Replace with your Wi-Fi password

#define projectName "smartGateParking"          // Antares project name
#define deviceNameSensor "sensorDIgital"   // Name of the device sending sensor data

AntaresESPHTTP antares(ACCESSKEY);   // Initialize AntaresESP32HTTP with the access key




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

const unsigned long interval = 5000;     // Interval to send sensor data (5 seconds)
const unsigned long interval2 = 10000;   // Interval to get data from Antares (10 seconds)
unsigned long previousMillis = 0;        // Store the last time sensor data was sent
unsigned long previousMillis2 = 0;       // Store the last time data was received from Antares


unsigned int counter = 0;     // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;
int channel;
#define SS_PIN 16
#define RST_PIN 0
int pos = 5;

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

/*
   Pass as a parameter the trigger and echo pin, respectively,
   or only the signal pin (for sensors 3 pins), like:
   Ultrasonic ultrasonic(13);
*/

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/26, /*tx =*/25);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif
DFRobotDFPlayerMini myDFPlayer;


// Ultrasonic ultrasonic(26,25);
MFRC522 mfrc522(SS_PIN, RST_PIN); 
LiquidCrystal_I2C lcd(0x27,16,2);
Servo myservo;
int distance;

String testData;            // String to hold received data from Antares
String lastData="";         // String to store last received data

int echo = 33;
int trig = 32;
long jarak,durasi;
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
  Serial.begin(9600);
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

  #if (defined ESP32)
  FPSerial.begin(9600, SERIAL_8N1, /*rx =*/26, /*tx =*/25);
#else
  FPSerial.begin(9600);
#endif

  antares.setDebug(true);    // Enable Antares library debug mode
  antares.wifiConnection(WIFISSID, PASSWORD);  // Connect to WiFi using provided SSID and password
  // put your setup code here, to run once:
    // mengatur esp ke mode station
  WiFi.mode(WIFI_STA);
  SPI.begin();          // Initiate  SPI bus
  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  myDFPlayer.play(3);
  myDFPlayer.volume(100);
  mfrc522.PCD_Init();
  Serial.println("Put your card to the reader...");
  Serial.println();
  myservo.attach(17);
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Parkir Karyawan");
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  lcd.setCursor(0,1);
  lcd.print("Tempelkan Kartu!");
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  myservo.write(178);
  

}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0,1);
  lcd.print("Tempelkan Kartu!");
    // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
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

  digitalWrite(trig, LOW); 
  delayMicroseconds(8);
  digitalWrite(trig, HIGH); 
  delayMicroseconds(8); 
  digitalWrite(trig, LOW); 
  delayMicroseconds(8);
  durasi= pulseIn(echo, HIGH);
  jarak= (durasi / 2) / 29.1;
  delay(5);
  
  if(jarak <= 5 ){
      if (content.substring(1) == "53 41 03 AA"){
          antares.add("user", "reksa");                    // Add temperature data to 
          antares.send(projectName, deviceNameSensor);
          
          myDFPlayer.play(2);
          lcd.setCursor(0,1);
          lcd.print("Silahkan Masuk!        ");
            for(pos = 178; pos >= 98; pos -= 1)  //fungsi perulangan yang akan dijadikan PWM dengan kenaikan 1
            {
              myservo.write(pos); //prosedur penulisan data PWM ke motor servo
              delay(15); //waktu tunda 15 ms                 
            } 
          delay(2000);
          antares.add("user", "none");                    // Add temperature data to 
          antares.send(projectName, deviceNameSensor);
          while(jarak >= 5){
            myservo.write(pos);
          }
          for(pos = 98; pos <= 178; pos+=1)  //fungsi perulangan yang akan dijadikan PWM dengan penurunan 1
        {                              
          myservo.write(pos);                 
          delay(15);                        
        }
        lcd.setCursor(0,1);
        lcd.print("Tempelkan Kartu!");
        }else if (content.substring(1) == "53 77 8A 0D"){
          antares.add("user", "Agus");                    // Add temperature data to 
          antares.send(projectName, deviceNameSensor);
          
          myDFPlayer.play(2);
          lcd.setCursor(0,1);
          lcd.print("Silahkan Masuk!        ");
            for(pos = 178; pos >= 98; pos -= 1)  //fungsi perulangan yang akan dijadikan PWM dengan kenaikan 1
            {
              myservo.write(pos); //prosedur penulisan data PWM ke motor servo
              delay(15); //waktu tunda 15 ms                 
            } 
          delay(2000);
          antares.add("user", "none");                    // Add temperature data to 
          antares.send(projectName, deviceNameSensor);
          do{
            myservo.write(pos);
          }while((jarak <= 5))
          for(pos = 98; pos <= 178; pos+=1)  //fungsi perulangan yang akan dijadikan PWM dengan penurunan 1
        {                              
          myservo.write(pos);                 
          delay(15);                        
        }
        lcd.setCursor(0,1);
        lcd.print("Tempelkan Kartu!");
        }else if (content.substring(1) == "93 A9 2E 10"){
          antares.add("user", "Algi");                    // Add temperature data to 
          antares.send(projectName, deviceNameSensor);
          
          myDFPlayer.play(2);
          lcd.setCursor(0,1);
          lcd.print("Silahkan Masuk!        ");
            for(pos = 178; pos >= 98; pos -= 1)  //fungsi perulangan yang akan dijadikan PWM dengan kenaikan 1
            {
              myservo.write(pos); //prosedur penulisan data PWM ke motor servo
              delay(15); //waktu tunda 15 ms                 
            } 
          delay(2000);
          antares.add("user", "none");                    // Add temperature data to 
          antares.send(projectName, deviceNameSensor);
          while(jarak >= 5){
            myservo.write(pos);
          }
          for(pos = 98; pos <= 178; pos+=1)  //fungsi perulangan yang akan dijadikan PWM dengan penurunan 1
        {                              
          myservo.write(pos);                 
          delay(15);                        
        }
        lcd.setCursor(0,1);
        lcd.print("Tempelkan Kartu!");
        }
  
    else   {
      antares.add("user", "Unknow");                    // Add temperature data to 
      antares.send(projectName, deviceNameSensor);
      
      myDFPlayer.play(1);
      lcd.setCursor(0,1);
      lcd.print("Akses Ditolak!      ");
      delay(2000);
      antares.add("user", "none");                    // Add temperature data to 
      antares.send(projectName, deviceNameSensor);
    }
  }else{
    lcd.setCursor(0,1);
    lcd.print("Tempelkan Kartu!");
  }
  

}
