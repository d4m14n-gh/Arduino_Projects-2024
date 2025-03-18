#include <SPI.h>
#include <LoRa.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.home();
  lcd.print("LoRa Receiver");

  if (!LoRa.begin(433E6)) {  // Ustawienie częstotliwości
    lcd.print(" Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  // Sprawdzanie, czy jest dostępna nowa wiadomość
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Odczytywanie pakietu
    lcd.clear();
    lcd.home();
    lcd.print("Received packet '");
    lcd.setCursor(0, 1);
    while (LoRa.available()) {
      lcd.print((char)LoRa.read());
    }
    lcd.print("' with RSSI ");
    lcd.print(LoRa.packetRssi());
  }
}
