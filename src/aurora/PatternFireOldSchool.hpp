// Fire Old School Demo Scene effect - Yaroslaw Turbin (ldirko), 12-06-2021
// https://editor.soulmatelights.com/gallery/1098-fire-oldschooldemoeffect

#ifndef PatternFireOldSchool_H
#define PatternFireOldSchool_H

DEFINE_GRADIENT_PALETTE(fireOldSchoolPal) {
  0,   0,   0,   0,   // noir
  50,  0,   0,   0,
  64,  255, 0,   0,   // rouge
  100, 255, 255, 0,   // jaune
  255, 255, 255, 255  // blanc
};

class PatternFireOldSchool : public Drawable {
  private:
    // tampon avec une bordure d'un pixel autour de la matrice
    uint8_t buff[(VPANEL_W + 2) * (VPANEL_H + 2)] = {0};
    CRGBPalette256 pal = fireOldSchoolPal;

    uint16_t buffXY(int x, int y) { return y * (VPANEL_W + 2) + x; }

  public:
    PatternFireOldSchool() {
      name = (char *)"Fire Old School";
    }

    unsigned int drawFrame() {
      // ligne aléatoire sous l'écran
      for (int i = 0; i < VPANEL_W + 2; i++)
        buff[buffXY(i, VPANEL_H + 1)] = !random8(2) ? random8(65, 200) : 0;

      // remontée + moyenne
      for (int x = 1; x < VPANEL_W + 1; x++)
        for (int y = 0; y < VPANEL_H + 1; y++) {
          uint16_t sum = buff[buffXY(x, y)] + buff[buffXY(x - 1, y + 1)]
                       + buff[buffXY(x, y + 1)] + buff[buffXY(x + 1, y + 1)];
          buff[buffXY(x, y)] = sum / 4;
        }

      uint16_t buffIndex = 1;
      for (int y = 0; y < VPANEL_H; y++) {
        for (int x = 0; x < VPANEL_W; x++)
          effects.leds[XY16(x, y)] = ColorFromPalette(pal, buff[buffIndex++], 255);
        buffIndex += 2;
      }

      effects.ShowFrame();
      return 40;
    }
};

#endif
