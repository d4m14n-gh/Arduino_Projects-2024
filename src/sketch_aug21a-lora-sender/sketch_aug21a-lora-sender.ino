#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6)) {  // Ustawienie częstotliwości
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

int counter = 0;
void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // Rozpoczęcie nadawania
  LoRa.beginPacket();
  LoRa.print("Hello ");
  LoRa.print(String(counter));
  LoRa.endPacket();

  counter++;

  delay(1000);
}
