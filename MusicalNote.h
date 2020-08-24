#pragma once

#include <Adafruit_NeoPixel.h>

#define MinOctave 2
#define MaxOctave 4
#define MaxIndex 11
#define MaxAccumulation 5


class Note
{
public:
  Note():
    m_octave(0),
    m_index(0),
    m_accumulation(0),
    m_nextOctave(0),
    m_nextIndex(0),
    m_nextAccumulation(0)
  {}

  Note(uint8_t index, uint8_t octave):
    m_octave(octave),
    m_index(index),
    m_accumulation(3),
    m_nextOctave(0),
    m_nextIndex(0),
    m_nextAccumulation(0)
  {}

  Note(float frequency):
    m_octave(0),
    m_index(0),
    m_accumulation(0),
    m_nextOctave(0),
    m_nextIndex(0),
    m_nextAccumulation(0)
  {
    setFrequency(frequency);
  }

  void setFrequency(float frequency)
  {
    // Find octave range based on the guess
    int multiplier = 1;
    uint8_t newOctave = 0;
    while (newOctave <= MaxOctave && (frequency < Frequencies[0] * multiplier - 7 || frequency > Frequencies[11] * multiplier + 7))
    {
      ++newOctave;
      multiplier *= 2;
    }

    if (newOctave < MinOctave || newOctave > MaxOctave)
    {
      decrement();
      return;
    }

    // Find the closest note
    auto minValue = 10000000;
    uint8_t newIndex = 0;
    for (auto i = 0u; i <= MaxIndex; ++i)
    {
      if (minValue > fabs(frequency - Frequencies[i] * multiplier))
      {
        minValue = fabs(frequency - Frequencies[i] * multiplier);
        newIndex = i;
      }
    }

    if (newOctave == m_octave && newIndex == m_index)
    {
      if (m_accumulation + 3 <= MaxAccumulation)
        m_accumulation += 3;
      else
        m_accumulation = MaxAccumulation;
    }
    else
    {
      decrement();
      if (newOctave == m_nextOctave && newIndex == m_nextIndex)
        ++m_nextAccumulation;
      else
      {
        m_nextOctave = newOctave;
        m_nextIndex = newIndex;
        m_nextAccumulation = 0;
      }
      if (m_nextAccumulation > m_accumulation)
      {
        m_octave = newOctave;
        m_index = newIndex;
        m_accumulation = 1;
      }
    }

    //Print the note
    //Serial.print("I think you played ");
    Serial.print(Names[newIndex]);
    Serial.println(newOctave);
    //Serial.print(", accumulated ");
    //Serial.print(m_accumulation);
    //Serial.print(" on ");
    //Serial.print(name());
    //Serial.println(m_octave);
  }

  void decrement()
  {
    if (m_accumulation > 0)
      --m_accumulation;
  }

  bool active() const
  {
    return m_accumulation > 3;
  }

  // Get English note name (with optional #)
  const char* name() const
  {
    return Names[m_index];
  }

  // Get octave number (0-8)
  uint8_t octave() const
  {
    return m_octave; 
  }

  uint32_t color() const
  {
    if (m_index == 0 && m_octave == 4)
      return Adafruit_NeoPixel::Color(255, 255, 255);
    return Colors[m_index];
  };

private:
  // Note names
  const char* const Names[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
  // Note frequencies
  const float Frequencies[12] = {16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87};
  // Note colors
  const uint32_t Colors[12] = {
    Adafruit_NeoPixel::Color(255, 0, 0), Adafruit_NeoPixel::Color(225, 85, 85),
    Adafruit_NeoPixel::Color(255, 120, 0), Adafruit_NeoPixel::Color(255, 192, 0),
    Adafruit_NeoPixel::Color(255, 255, 0),
    Adafruit_NeoPixel::Color(0, 178, 0), Adafruit_NeoPixel::Color(37, 214, 0),
    Adafruit_NeoPixel::Color(0, 255, 156), Adafruit_NeoPixel::Color(37, 214, 218),
    Adafruit_NeoPixel::Color(0, 145, 181), Adafruit_NeoPixel::Color(0, 72, 181),
    Adafruit_NeoPixel::Color(0, 0, 181) };

private:
    uint8_t m_octave;
    uint8_t m_index;
    uint8_t m_accumulation;
    uint8_t m_nextOctave;
    uint8_t m_nextIndex;
    uint8_t m_nextAccumulation;
};
