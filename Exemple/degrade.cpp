#include <Adafruit_Protomatter.h>

// Brochage HUB75 du Matrix Portal S3
uint8_t rgbPins[]  = {42, 41, 40, 38, 39, 37};
uint8_t addrPins[] = {45, 36, 48, 35};  // 4 pins d'adresse pour du 32 lignes
const uint8_t clockPin = 2;
const uint8_t latchPin = 47;
const uint8_t oePin    = 14;

// largeur 32, profondeur de bits 4, 1 chaîne, double buffer
Adafruit_Protomatter matrix(32, 4, 1, rgbPins, 4, addrPins,
                            clockPin, latchPin, oePin, true);

uint16_t wheel(uint8_t pos) {
  if (pos < 85)  return matrix.color565(255 - pos * 3, pos * 3, 0);
  if (pos < 170) { pos -= 85;  return matrix.color565(0, 255 - pos * 3, pos * 3); }
  pos -= 170;
  return matrix.color565(pos * 3, 0, 255 - pos * 3);
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.print("Starting .....\n");
  ProtomatterStatus status = matrix.begin();
  Serial.printf("Protomatter status: %d\n", (int)status);
  if (status != PROTOMATTER_OK) for (;;);
}

void loop() {
  static uint8_t offset = 0;
  for (int y = 0; y < 32; y++)
    for (int x = 0; x < 32; x++)
      matrix.drawPixel(x, y, wheel((x + y) * 4 + offset));
  matrix.show();
  offset++;
  Serial.print("x");
  delay(20);
}