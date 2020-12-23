#pragma once

#define OFFSETSAMPLES 40  //used for calibrating purposes

#include "util.h"

class Calibrator
{
public:
  Calibrator() : m_avgOffset(0)
  {}

  void calibrate()
  {
    SerialPrintStr("Keep silent! Calibrating...");
    uint32_t sumOffset = 0;
    uint16_t offset[OFFSETSAMPLES]; //create offset vector
    for (uint8_t i = 0u; i < OFFSETSAMPLES; ++i)
    {
      offset[i] = analogRead(0); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
      //Serial.println(offset[i]); //use this to adjust the sound detection module to approximately half or 512 when no sound is played.
      sumOffset += offset[i];
    }
    m_avgOffset = sumOffset / OFFSETSAMPLES;
    SerialPrintLnStr("Done");
  }

  uint8_t offset() const { return m_avgOffset; }

private:
  uint8_t m_avgOffset; //create offset vector
};
