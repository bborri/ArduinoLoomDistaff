#pragma once

#include "arduinoFFT.h"


class FFTFrequencyDetector
{
public:
  FFTFrequencyDetector()
  {}

  double detectFrequency(double* samples, size_t sampleCount)
  {
    double* real = samples;
    double imaginary[sampleCount];
    ::memset(imaginary, 0, sizeof(double) * sampleCount);

    // Create FFT object
    arduinoFFT FFT { real, imaginary, sampleCount, 9165 };

    // Remove DC bias
    //FFT.DCRemoval();

    // Weigh data
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);

    // Compute FFT
    FFT.Compute(FFT_FORWARD);

    // Compute magnitudes 
    FFT.ComplexToMagnitude();

    // Calculate major peak
    return FFT.MajorPeak();
  }

private:
  template<class T>
  static void printArray(T* values, size_t sampleCount)
  {
    for (auto i = 0u; i < sampleCount; ++i)
    {
      Serial.print(values[i]);
      Serial.print(" ");
    }
    Serial.println("");
  }
};
