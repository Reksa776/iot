#include <lorawan.h>
#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>


// mac address tujuan
uint8_t mac_addr_tujuan[] = {0xc8, 0xc9, 0xa3, 0xeb, 0xce, 0xac};

// struktur data yang dikirim
typedef struct struct_message {
  int nilai;
} struct_message;
struct_message data_ku;

// variabel peerInfo
esp_now_peer_info_t peerInfo;

// callback Jika data terkirim
void cb_terkirim(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nStatus Pengiriman Paket Terakhir:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Pengiriman Sukses" : "Pengiriman Gagal");
}


//ABP Credentials
/*
  Notes:
  - select ABP Activation on ANTARES
  - select inherit to generate your keys
  - nwkSKey: 32 digit hex, you can put 16 first digits by first 16 digits your access key and add 16 digits with 0 (ex : abcdef01234567890000000000000000)
  - appSKey: 32 digit hex, put 16 first digits by 0 and put last 16 digits by last 16 digit your access key (ex : 0000000000000000abcdef0123456789)
*/
const char *devAddr = "Lora-Device-Address"; // Replace with the Device Address that you have in the Antares console
const char *nwkSKey = "Network-Session-Key"; // Replace with the Network Session Key that you have in the Antares console
const char *appSKey = "Application-Session-Key"; // Replace with the Application Session Key that you have in the Antares console

const unsigned long interval = 10000;    // 10 s interval to send message
unsigned long previousMillis = 0;  // will store last time message sent
unsigned int counter = 0;     // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;
int channel;
#define SS_PIN 16
#define RST_PIN 0

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

MFRC522 mfrc522(SS_PIN, RST_PIN); 

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
  // mengatur esp ke mode station
  WiFi.mode(WIFI_STA);
    SPI.begin();          // Initiate  SPI bus
  mfrc522.PCD_Init();
  Serial.println("Put your card to the reader...");
  Serial.println();

  // inisialisasi espnow
  if (esp_now_init() != ESP_OK) {
    Serial.println("Gagal Inisialisasi espnow");
    return;
  }

  // mendaftarkan fungsi callback  
  esp_now_register_send_cb(cb_terkirim);
  
  // Mendata Peer / Kawan
  memcpy(peerInfo.peer_addr, mac_addr_tujuan, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Menambahkan Peer / Kawan
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Gagal Menambah Peer");
    return;
  }
}

void loop() {
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
  // map<char, char> UID{ { "53 41 03 AA", "53 41 03 AA" },
  //           { "93 A9 2E 10", "93 A9 2E 10" } };
            

  // mengisi nilai ke variabel
    if (content.substring(1) == "53 41 03 AA") //TUKAR UID INI SESUAI RFIA TAG KALIAN
      {
        data_ku.nilai = 1;
      }else   {
     data_ku.nilai = 0;
      }
  
  // mengirim data
  esp_err_t result = esp_now_send(mac_addr_tujuan, (uint8_t *) &data_ku, sizeof(data_ku));

  // status pengiriman
  if (result == ESP_OK) {
    Serial.println("Berhasil");
  }
  else {
    Serial.println("Gagal");
  }
  delay(2000);

}
