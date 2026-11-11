/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Original Copyright (c) 2014 Jason Coon
 * 
 * Modified by Codetastic 2024
 * 
*/

#ifndef Patterns_H
#define Patterns_H

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#include "Vector2.hpp"
#include "Boid.hpp"
#include "Attractor.hpp"

/* 
 *  Note from mrfaptastic:
 *  
 *  Commented out patterns are due to the fact they either didn't work properly with a non-square display,
 *  or from my personal opinion, are crap. 
 */
#include "PatternStarfield.hpp" // new 2024
#include "PatternAttract.hpp"
#include "PatternElectricMandala.hpp"
#include "PatternFlock.hpp"
#include "PatternFlowField.hpp"
#include "PatternIncrementalDrift.hpp"
// #include "PatternInfinity.hpp"
#include "PatternMaze.hpp" // ??
// #include "PatternMunch.hpp"
#include "PatternNoiseSmearing.hpp"
#include "PatternPendulumWave.hpp"
#include "PatternPlasma.hpp"
#include "PatternRadar.hpp"
#include "PatternSimplexNoise.hpp"
#include "PatternSnake.hpp"
#include "PatternSpiral.hpp"
#include "PatternSpiro.hpp"
#include "PatternWave.hpp" 
#include "PatternRain.hpp"
#include "PatternJuliaSetFractal.hpp"
#include "PatternFireworks.hpp"
#include "PatternDistorsionWaves.hpp"
#include "PatternColoredBursts.hpp"
#include "PatternFireOldSchool.hpp"
#include "PatternBounce.hpp"
#include "PatternCube.hpp"
#include "PatternFireKoz.hpp"
// #include "PatternGreenScroll.hpp"
#include "PatternIncrementalDrift2.hpp"
// #include "PatternSpin.hpp"



class Patterns {
  private:

    PatternStarfield starfield;
    PatternAttract attract;
    PatternElectricMandala electricMandala;
    PatternFlock flock;
    PatternFlowField flowField;
    PatternIncrementalDrift incrementalDrift;
    // PatternInfinity infinity;
    PatternMaze maze;    
    // PatternMunch munch;
    PatternPendulumWave pendwave;
    PatternPlasma plasma;
    PatternRadar radar;
    PatternSimplexNoise simpnoise;
    PatternSnake snake;
    PatternSpiral spiral;
    PatternSpiro spiro;
    PatternWave wave;
    PatternJuliaSet juliaSet;
    
    PatternRain rain;
    PatternFirework fireworks;
    PatternDistorsionWaves distorsionWaves;
    PatternColoredBursts coloredBursts;
    PatternFireOldSchool fireOldSchool;
    PatternBounce bounce;
    PatternCube cube;
    PatternFireKoz fireKoz;
    // PatternTheMatrix theMatrix;
    PatternIncrementalDrift2 incrementalDrift2;
    // PatternSpin spin;
    // PatternMultipleStream multipleStream;
    PatternMultipleStream2 multipleStream2;
    PatternMultipleStream3 multipleStream3;
    PatternMultipleStream4 multipleStream4;
    // PatternMultipleStream5 multipleStream5;
    PatternMultipleStream8 multipleStream8;
    // PatternPaletteSmear paletteSmear;
    // PatternRainbowFlag rainbowFlag;
    

    std::vector<Drawable*> availablePatterns = {
      &juliaSet,
      &starfield,
      &attract,
      &electricMandala,
      &flock,
      &spiro,
      &radar,
      &flowField,
      &incrementalDrift,
      // &infinity,
      &maze,
      // &munch,
      &pendwave,
      &plasma,
      &simpnoise,
      &wave,
      &rain,
      &fireworks,
      &distorsionWaves,
      &coloredBursts,
      &fireOldSchool,
      &snake,
      &spiral,
      &bounce,
      &cube,
      &fireKoz,
      // &theMatrix,
      &incrementalDrift2,
      // &spin,
      // &multipleStream,
      &multipleStream2,
      &multipleStream3,
      &multipleStream4,
      // &multipleStream5,
      &multipleStream8,
      // &paletteSmear,
      // &rainbowFlag
    };    

    int currentIndex = 0;
    Drawable* currentItem;

    int getCurrentIndex() {
      return currentIndex;
    }


  public:
    Patterns() {
      this->currentItem = availablePatterns[0];
      this->currentItem->start();
    }

    void stop() {
      if (currentItem)
        currentItem->stop();
    }

    void start() {
      if (currentItem)
        currentItem->start();
    }

    void moveTo(int index) 
    {
      index = ((index >= availablePatterns.size()) || (index < 0)) ? 0 : index;

      if (currentItem)
        currentItem->stop();

      currentIndex = index;
      currentItem = availablePatterns[currentIndex];

      if (currentItem)
        currentItem->start();

    } // index   


    void move(int step) {

      const int patternCount = static_cast<int>(availablePatterns.size());
      currentIndex = (currentIndex + step) % patternCount;
      if (currentIndex < 0) currentIndex += patternCount;

      moveTo(currentIndex);
    }

    void moveRandom(int step) {
      int rand_index =  random(0, availablePatterns.size()-1);
      moveTo(rand_index);
    }

    unsigned int drawFrame() {
      return currentItem->drawFrame();
    }

    char * getCurrentPatternName()
    {
      return currentItem->name;      
    }

    size_t getPatternCount() const {
      return availablePatterns.size();
    }

    const char * getPatternName(size_t index) const {
      return index < availablePatterns.size() ? availablePatterns[index]->name : nullptr;
    }
/*
    bool setPattern(String name) {

      for (size_t i = 0; i < availablePatterns.size(); i++) {
        if (name.compareTo(availablePatterns[i]->name) == 0) {
          moveTo(i);
          return true;
        }
      }

      return false;
    }

*/
    bool setPattern(int index) {
      if (index >= availablePatterns.size() || index < 0)
        return false;

      moveTo(index);

      return true;
    }
};

#endif