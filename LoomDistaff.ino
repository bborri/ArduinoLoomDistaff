
#include "RGBStrip.h"
#include "MusicalNote.h"
#include "Calibrator.h"
#include "AutoCorrelate-lettsome.h"

RGBStrip rgb;
Note note;
Calibrator calibrator;

void setup()
{
  Serial.begin(115200); //115200 Baud rate for the Serial Monitor

  // if analog input pin 3 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(3));

  //*****************************************************************
  //Calibration Section
  //*****************************************************************
  calibrator.calibrate();

  // Init LED strip
  rgb.init();
}

void playNote(const Note& note)
{
  // Play the proper sound
  // TODO

  // Light the LEDs with the proper color
  rgb.shimmerAroundColor(note.color());
}

void stopPlaying()
{
  // Stop playing sound
  // TODO
  
  // Turn off the lights
  rgb.clear();
}

void loop()
{
  AutoCorrelate autoCorrelate;

  // Read from pin 0
  autoCorrelate.readData(0);

  float signalFrequencyGuess = autoCorrelate.correlate(calibrator.offset());
  if (signalFrequencyGuess > 0)
  {
    note.setFrequency(signalFrequencyGuess);
  }
  else
    note.decrement();

  if (note.active())
    playNote(note);
  else
    stopPlaying();
}
