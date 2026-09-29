#include <Arduino.h>
#include <Adafruit_Protomatter.h>

// Adafruit Matrix Portal S3 HUB75 pins.
uint8_t rgbPins[] = {42, 41, 40, 38, 39, 37};
uint8_t addrPins[] = {17, 18, 21, 16, 36};

constexpr uint8_t CLK_PIN = 34;
constexpr uint8_t LAT_PIN = 33;
constexpr uint8_t OE_PIN = 35;
constexpr uint16_t MATRIX_WIDTH = 64;
constexpr uint16_t MATRIX_HEIGHT = 32;

Protomatter matrix(
    6,
    1,
    rgbPins,
    5,
    addrPins,
    CLK_PIN,
    LAT_PIN,
    OE_PIN,
    true);

uint16_t hue = 0;

void drawDemo() {
  matrix.fillScreen(0);

  for (int16_t x = 0; x < MATRIX_WIDTH; x++) {
    const uint16_t color = matrix.color565(
        (x * 4 + hue) & 0xFF,
        80,
        255 - ((x * 4 + hue) & 0xFF));
    matrix.drawFastVLine(x, 0, MATRIX_HEIGHT, color);
  }

  matrix.setTextColor(matrix.color565(255, 255, 255));
  matrix.setTextSize(1);
  matrix.setCursor(2, 4);
  matrix.print("Matrix Portal S3");
  matrix.setCursor(2, 18);
  matrix.print("HUB75 / Arduino");
  matrix.show();
}

void setup() {
  Serial.begin(115200);
  delay(250);

  const auto status = matrix.begin();
  if (status != PROTOMATTER_OK) {
    Serial.print("Protomatter init failed: ");
    Serial.println(status);
    while (true) {
      delay(1000);
    }
  }

  matrix.setTextWrap(false);
  drawDemo();
  Serial.println("Matrix Portal S3 ready");
}

void loop() {
  drawDemo();
  hue++;
  delay(40);
}
