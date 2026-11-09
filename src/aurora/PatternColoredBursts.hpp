// Colored bursts - Yaroslaw Turbin (ldirko), 23-03-2021
// https://editor.soulmatelights.com/gallery/819-colored-bursts

#ifndef PatternColoredBursts_H
#define PatternColoredBursts_H

class PatternColoredBursts : public Drawable {
  private:
    uint8_t hue = 0;
    uint8_t numLines = 10;
    uint32_t lastHue = 0;
    uint32_t lastLines = 0;

    void add(int x, int y, CRGB color) {
      if (x >= 0 && x < VPANEL_W && y >= 0 && y < VPANEL_H) effects.leds[XY16(x, y)] += color;
    }

    void drawLine(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, CRGB color) {
      uint8_t xsteps = abs8(x - x1) + 1;
      uint8_t ysteps = abs8(y - y1) + 1;
      uint8_t steps = xsteps >= ysteps ? xsteps : ysteps;
      for (uint8_t i = 1; i <= steps; i++) {
        uint8_t dx = lerp8by8(x, x1, i * 255 / steps);
        uint8_t dy = lerp8by8(y, y1, i * 255 / steps);
        CRGB c = color;
        c %= (i * 255 / steps); // ligne en dégradé
        add(dx, dy, c);
      }
    }

  public:
    PatternColoredBursts() {
      name = (char *)"Colored Bursts";
    }

    unsigned int drawFrame() {
      const int8_t speed = -5; // vitesse globale des points
      uint32_t now = millis();
      if (now - lastHue >= 30) { lastHue = now; hue++; }
      if (now - lastLines >= 1000) {
        lastLines = now;
        if (random8(2)) { if (++numLines > 15) numLines = 15; }
        else { if (--numLines < 5) numLines = 5; }
      }

      fadeToBlackBy(effects.leds, effects.num_leds, 40);
      for (uint8_t i = 0; i < numLines; i++) {
        uint8_t x1 = beatsin8(12 + speed, 0, VPANEL_W - 1);
        uint8_t x2 = beatsin8(10 + speed, 0, VPANEL_W - 1);
        uint8_t y1 = beatsin8(25 + speed, 0, VPANEL_H - 1, 0, i * 24);
        uint8_t y2 = beatsin8(20 + speed, 0, VPANEL_H - 1, 0, i * 48 + 64);
        // l'original passe (x1, x2, y1, y2) : conservé tel quel pour le même rendu
        drawLine(x1, x2, y1, y2, CHSV(i * 255 / numLines + hue, 255, 255));
      }
      effects.Blur2d(8);

      effects.ShowFrame();
      return 0;
    }
};

#endif
