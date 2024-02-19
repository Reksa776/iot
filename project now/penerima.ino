#include <esp_now.h>
#include <WiFi.h>
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

// struktur data yang diterima
typedef struct struct_message {
    int nilai;
} struct_message;
struct_message data_ku;

// callback jika menerima data
void cb_terima(const uint8_t *mac_addr, const uint8_t *dataDiterima, int panjang) {
  memcpy(&data_ku, dataDiterima, sizeof(data_ku));
  Serial.print("Bytes diterima: ");
  Serial.println(panjang);
  Serial.print("Int: ");
  Serial.println(data_ku.nilai);

  Serial.println();
}

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/26, /*tx =*/25);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  #if (defined ESP32)
  FPSerial.begin(9600, SERIAL_8N1, /*rx =*/26, /*tx =*/25);
#else
  FPSerial.begin(9600);
#endif
  Serial.begin(115200);
  
  // mengatur esp ke mode station
  WiFi.mode(WIFI_STA);

  // inisialisasi espnow
  if (esp_now_init() != ESP_OK) {
    Serial.println("Gagal Inisialisasi espnow");
    return;
  }

  // mendaftarkan fungsi callback
  esp_now_register_recv_cb(cb_terima);
  data_ku.nilai = 3;
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
}
 
void loop() {
  if(data_ku.nilai == 1){
    myDFPlayer.play(2);
    data_ku.nilai = 3;
  }else if (data_ku.nilai == 0){
    myDFPlayer.play(1);
    data_ku.nilai = 3;
  }
}
