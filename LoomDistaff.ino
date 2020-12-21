
#include "util.h"
#include "RGBStrip.h"
#include "MusicalNote.h"
#include "Calibrator.h"
//#include "SoundPlayer.h"
#include "AutoCorrelate-lettsome.h"

RGBStrip rgb(6);
Note note;
Calibrator calibrator;
//SoundPlayer player;


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
  Serial.println(calibrator.offset());

  // Init LED strip
//  rgb.init();
}
/*
void playNote(const Note& note)
{
  // Play the proper sound
  player.playLoomNote(note.name());

  // Light the LEDs with the proper color
  rgb.shimmerAroundColor(note.color());
}

void stopPlaying()
{
  // Stop playing sound
  player.stop();
  
  // Turn off the lights
  rgb.clear();
}
*/
void loop()
{
  //rgb.wave(Adafruit_NeoPixel::Color(0, 150, 255), 10000);

  AutoCorrelate autoCorrelate;

  // Read from pin 0
  autoCorrelate.readData(0);

  float signalFrequencyGuess = autoCorrelate.correlate(calibrator.offset());
  SerialPrintStr("Frequency ");
  Serial.println(signalFrequencyGuess);
  if (signalFrequencyGuess > 0)
  {
    note.setFrequency(signalFrequencyGuess);
  }
  else
    note.decrement();
/*
  if (note.active())
    playNote(note);
  else
    stopPlaying();
*/
/*    player.playLoomNote("C");
    player.playLoomNote("D");
    player.playLoomNote("E");
    player.playLoomNote("F");
    player.playLoomNote("G");
    player.playLoomNote("A");
    player.playLoomNote("B");
    player.playLoomNote("CC");*/
}
