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
 
SoftwareSerial mySoftwareSerial(16, 17);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
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
    if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
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
