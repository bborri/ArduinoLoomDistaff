// Modified from Clyde A. Lettsome's code, whose licence follows

/*
  File/Sketch Name: MusicalNoteDetector

  Version No.: v1.0 Created 7 June, 2020

  Original Author: Clyde A. Lettsome, PhD, PE, MEM

  Description:  This code/sketch displays the approximate frequency as well as the musical note played on an electronic keyboard or piano app. For this project, the analog output from the
  sound module detector is sent to the A0 analog input of the Arduino Uno. The analog signal is sampled and quantized (digitized). Autocorrelation, weighting and tuning code is used to
  find fundamental frequency using the first 3 periods. The approximate fundamental frequency is then compared to frequencies in octaves 3, 4, and 5 range to determine the closest musical
  note frequency. Finally the guessed note for the closest frequency is printed to the screen.

  License: This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License (GPL) version 3, or any later
  version of your choice, as published by the Free Software Foundation.

  Notes: Copyright (c) 2020 by C. A. Lettsome Services, LLC
  For more information visit https://clydelettsome.com/blog/2020/06/07/my-weekend-project-musical-note-detector-using-an-arduino/

*/

#pragma once

#include "util.h"

#define TUNER 0    //Adjust until C3 is 130.50
#define SAMPLES 128             //Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 9615 //Fs = Based on Nyquist, must be 2 times the highest expected frequency.
#define SAMPLING_PERIOD (1.0 / SAMPLING_FREQUENCY) //Period in microseconds


class AutoCorrelate
{
public:
  AutoCorrelate()
  {}

  void readData(int pin)
  {
    //*****************************************************************
    //Collect SAMPLES samples from A0 with sample period of samplingPeriod
    //*****************************************************************
    for (auto i = 0u; i < SAMPLES; i++)
    {
      unsigned long microSeconds = micros();    //Returns the number of microseconds since the Arduino board began running the current script.
      m_samples[i] = analogRead(pin); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
      
      /*remaining wait time between samples if necessary in seconds */
      while (micros() < (microSeconds + (SAMPLING_PERIOD * 1000000)))
      {
        //do nothing just wait
      }
    }
  }

  // Apply a Hann window to the signal. Array must be SAMPLES long
  template<class T>
  void window(T* array)
  {
    for (uint8_t i = 0; i < SAMPLES; ++i)
      array[i] = 0.5 * (1 - cos(6.283 * array[i] / SAMPLES));
  }

  float correlate(int offset) const
  {
    float signalFrequency = 0;
    float signalFrequency2 = 0;
    float signalFrequency3 = 0;
    int periodBegin = 0;
    int periodEnd = 0;
    float maxValue = 0;
    int samplesPerPeriod = 0;
    int adjuster = 0;
    int numOfCycles = 0;
    int thresh = 0;
    float autoCorr[SAMPLES]; //create vector of size SAMPLES to hold imaginary values
    byte state_machine = 0;

    for (auto i = 0u; i < SAMPLES; i++) //i=delay
    {
      //Serial.print(m_samples[i] - offset);
      //Serial.print(", ");
      long sum = 0;
      for (auto k = 0u; k < SAMPLES - i; k++) //Match signal with delayed signal
      {
        sum = sum + (((m_samples[k]) - offset) * ((m_samples[k + i]) - offset)); // m_samples[k] is the signal and m_samples[k+i] is the delayed version
      }
      autoCorr[i] = sum / SAMPLES;
  
      // First Peak Detect State Machine
      if (state_machine==0 && i == 0)
      {
        thresh = autoCorr[i] * 0.5;
        state_machine = 1;
      }
      else if (state_machine == 1 && i>0 && thresh < autoCorr[i] && (autoCorr[i]-autoCorr[i-1])>0) //state_machine=1, find 1 period for using first cycle
      {
        maxValue = autoCorr[i];
      }
      else if (state_machine == 1&& i>0 && thresh < autoCorr[i-1] && maxValue == autoCorr[i-1] && (autoCorr[i]-autoCorr[i-1])<=0)
      {
        periodBegin = i - 1;
        state_machine = 2;
        numOfCycles = 1;
        samplesPerPeriod = (periodBegin - 0);
        adjuster = TUNER + (50.04 * exp(-0.102 * samplesPerPeriod));
        signalFrequency = ((SAMPLING_FREQUENCY) / (samplesPerPeriod))-adjuster; // f = fs/N
      }
      else if (state_machine == 2 && i>0 && thresh < autoCorr[i] && (autoCorr[i]-autoCorr[i-1])>0) //state_machine=2, find 2 periods for 1st and 2nd cycle
      {
        maxValue = autoCorr[i];
      }
      else if (state_machine == 2&& i>0 && thresh < autoCorr[i-1] && maxValue == autoCorr[i-1] && (autoCorr[i]-autoCorr[i-1])<=0)
      {
        periodEnd = i-1;
        state_machine = 3;
        numOfCycles = 2;
        samplesPerPeriod = (periodEnd - 0);
        signalFrequency2 = ((numOfCycles*SAMPLING_FREQUENCY) / (samplesPerPeriod))-adjuster; // f = (2*fs)/(2*N)
        maxValue = 0;
      }
      else if (state_machine == 3 && i>0 && thresh < autoCorr[i] && (autoCorr[i]-autoCorr[i-1])>0) //state_machine=3, find 3 periods for 1st, 2nd and 3rd cycle
      {
        maxValue = autoCorr[i]; 
      }
      else if (state_machine == 3&& i>0 && thresh < autoCorr[i-1] && maxValue == autoCorr[i-1] && (autoCorr[i]-autoCorr[i-1])<=0)
      {
        periodEnd = i-1;
        state_machine = 4;
        numOfCycles = 3;
        samplesPerPeriod = (periodEnd - 0);
        signalFrequency3 = ((numOfCycles*SAMPLING_FREQUENCY) / (samplesPerPeriod))-adjuster; // f = (3*fs)/(3*N)
      }
    }

    SerialPrintLnStr("Result analysis");

    //*****************************************************************
    //Result Analysis
    //*****************************************************************
    if (samplesPerPeriod > 0)
    { 
      //prepare the weighting function
      float total = 0.0f;
      if (signalFrequency != 0)
      {
        total = 1.0f;
      }
      if (signalFrequency2 != 0)
      {
        total = total + 2.0f;
      }
      if (signalFrequency3 != 0)
      {
        total = total + 3.0f;
      }
  
      //calculate the frequency using the weighting function
      const float signalFrequencyGuess = ((1/total) * signalFrequency) + ((2/total) * signalFrequency2) + ((3/total) * signalFrequency3); //find a weighted frequency
      SerialPrintStr("Detected frequency ");
      Serial.print(signalFrequencyGuess);     //Print the frequency guess.
      SerialPrintLnStr(" Hz.");
      return signalFrequencyGuess;
    }

    return 0;
  }

private:
  int m_samples[SAMPLES];
};
