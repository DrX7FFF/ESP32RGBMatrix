#pragma once

#include <Adafruit_GFX.h>
#include <FastLED.h>

class GFX : public Adafruit_GFX {
public:
	GFX(int16_t width, int16_t height) : Adafruit_GFX(width, height) {}
	virtual void drawPixel(int16_t x, int16_t y, CRGB color) = 0;
	void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, CRGB color) {
		Adafruit_GFX::drawLine(x0, y0, x1, y1, matrix->color565(color.r, color.g, color.b));
	}
	void drawTriangle(int16_t x0, int16_t y0,
					  int16_t x1, int16_t y1,
					  int16_t x2, int16_t y2, CRGB color) {
		Adafruit_GFX::drawTriangle(x0, y0, x1, y1, x2, y2,
									matrix->color565(color.r, color.g, color.b));
	}
};

class ProtomatterOutput {
public:
	void drawPixelRGB888(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
		matrix->drawPixel(x, y, matrix->color565(red, green, blue));
		if (x == MATRIX_WIDTH - 1 && y == MATRIX_HEIGHT - 1) {
			matrix->show();
		}
	}
};