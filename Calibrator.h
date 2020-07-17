#pragma once

#define OFFSETSAMPLES 40  //used for calibrating purposes

class Calibrator
{
public:
  void calibrate()
  {
    Serial.println("Calibrating. Please do not play any notes during calibration.");
    int sumOffset = 0;
    int offset[OFFSETSAMPLES]; //create offset vector
    for (auto i = 0u; i < OFFSETSAMPLES; i++)
    {
      offset[i] = analogRead(0); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
      Serial.println(offset[i]); //use this to adjust the sound detection module to approximately half or 512 when no sound is played.
      sumOffset += offset[i];
    }
    m_avgOffset = round(sumOffset / OFFSETSAMPLES);
  }

  int offset() const { return m_avgOffset; }

private:
  int m_avgOffset; //create offset vector
};
