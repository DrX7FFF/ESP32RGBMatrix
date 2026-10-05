#include <Adafruit_Protomatter.h>
#include <Arduino.h>
#include <vector>

// Adafruit Matrix Portal S3 HUB75 pins.
uint8_t rgbPins[] = {42, 41, 40, 38, 39, 37};
uint8_t addrPins[] = {17, 18, 21, 16};

constexpr uint8_t CLK_PIN = 34;
constexpr uint8_t LAT_PIN = 33;
constexpr uint8_t OE_PIN = 35;
constexpr uint16_t MATRIX_WIDTH = 32;
constexpr uint16_t MATRIX_HEIGHT = 32;

Adafruit_Protomatter protomatterMatrix(MATRIX_WIDTH,
										6,
										1,
										rgbPins,
										4,
										addrPins,
										CLK_PIN,
										LAT_PIN,
										OE_PIN,
										true);

Adafruit_Protomatter* matrix = &protomatterMatrix;

#define VPANEL_W MATRIX_WIDTH
#define VPANEL_H MATRIX_HEIGHT

#include "AuroraProtomatterCompat.h"

ProtomatterOutput output;
ProtomatterOutput* virtualDisp = &output;

#include <cstdlib>
#define free(pointer) ::free(pointer)
#include "aurora/EffectsLayer.hpp"
#undef free
EffectsLayer effects(MATRIX_WIDTH, MATRIX_HEIGHT);

#include "aurora/Drawable.hpp"
#include "aurora/Geometry.hpp"
#include "aurora/Patterns.hpp"

Patterns patterns;

constexpr uint32_t PATTERN_DURATION_MS = 30000;
constexpr uint32_t PALETTE_DURATION_MS = 10000;
uint32_t lastPatternChange = 0;
uint32_t lastPaletteChange = 0;
uint32_t nextFrameAt = 0;
bool autoAdvance = true;

void changePattern(int8_t step) {
	patterns.move(step);
	lastPatternChange = millis();
	nextFrameAt = lastPatternChange;
}

void handleSerialInput() {
	if (Serial.available() == 0) {
		return;
	}

	const char command = static_cast<char>(Serial.read());
	switch (command) {
		case 'n':
			changePattern(1);
			break;
		case 'p':
			changePattern(-1);
			break;
		case 'a':
			autoAdvance = !autoAdvance;
			Serial.println(autoAdvance ? "Auto advance ON" : "Auto advance OFF");
			lastPatternChange = millis();
			break;
		default:
			break;
	}
}

void setup() {
	Serial.begin(115200);
	delay(250);

	const auto status = matrix->begin();
	if (status != PROTOMATTER_OK) {
		Serial.print("Protomatter init failed: ");
		Serial.println(status);
		while (true) {
			delay(1000);
		}
	}

	patterns.listPatterns();
	lastPatternChange = millis();
	lastPaletteChange = lastPatternChange;
	Serial.print("Starting Aurora pattern: ");
	Serial.println(patterns.getCurrentPatternName());
}

void loop() {
	handleSerialInput();
	const uint32_t now = millis();

	if (now - lastPaletteChange >= PALETTE_DURATION_MS) {
		effects.RandomPalette();
		lastPaletteChange = now;
	}

	if (autoAdvance && now - lastPatternChange >= PATTERN_DURATION_MS) {
		changePattern(1);
	}

	if (static_cast<int32_t>(now - nextFrameAt) >= 0) {
		nextFrameAt = now + patterns.drawFrame();
	}
}
