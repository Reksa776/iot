#include <esp_now.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

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
 
LiquidCrystal_I2C lcd(0x27,16,2);

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
  lcd.begin(16,2);
  lcd.init();
  // lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Parkir Karyawan");
  lcd.setCursor(0,1);
  lcd.print("Tempelkan Kartu!");
  data_ku.nilai = 3;
}
 
void loop() {
  lcd.setCursor(0,1);
  lcd.print("Tempelkan Kartu!");
  if(data_ku.nilai == 1){
    lcd.setCursor(0,1);
    lcd.print("Silahkan Masuk!        ");
    delay(2000);
    lcd.setCursor(0,1);
    lcd.print("Tempelkan Kartu!");
    data_ku.nilai = 3;
  }else if (data_ku.nilai == 0){
    lcd.setCursor(0,1);
    lcd.print("Akses Ditolak        ");
    delay(2000);
    lcd.setCursor(0,1);
    lcd.print("Tempelkan Kartu!");
    data_ku.nilai = 3;
  }else{
    lcd.setCursor(0,1);
    lcd.print("Tempelkan Kartu!");
  }
}
