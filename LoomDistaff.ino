
#include "util.h"
#include "RGBStrip.h"
#include "MusicalNote.h"
#include "Calibrator.h"
//#include "SoundPlayer.h"
//#include "AutoCorrelate-lettsome.h"
//#include "AutoCorrelate-vonk.h"
#include "FFT.h"

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
//  Serial.println(calibrator.offset());

  // Init LED strip
  rgb.init();
}

void playNote(const Note& note)
{
  // Play the proper sound
//  player.playLoomNote(note.name());

  // Light the LEDs with the proper color
  rgb.shimmerAroundColor(note.color());
}

void stopPlaying()
{
  // Stop playing sound
//  player.stop();
  
  // Turn off the lights
  rgb.clear();
}

void loop()
{
  //rgb.wave(Adafruit_NeoPixel::Color(0, 150, 255), 10000);

  float signalFrequencyGuess = 0.0f;

  {
      //AutoCorrelate autoCorrelate;

    // Read from pin 0
/*    autoCorrelate.readData(0);
*/
    #define SAMPLES 128
    double samples[SAMPLES];
    for (auto i = 0u; i < SAMPLES; i++)
    {
      unsigned long microSeconds = micros();    //Returns the number of microseconds since the Arduino board began running the current script.
      double value = analogRead(0); // Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
      double gain = analogRead(1); // Read envelope value
      samples[i] = (abs(gain) > 20) ? value : 0;
      
      /*remaining wait time between samples if necessary in seconds */
      while (micros() < (microSeconds + ((1.0/9615) * 1000000)))
      {
        //do nothing just wait
      }
    }

    //signalFrequencyGuess = autoCorrelate.correlate(samples, 0/*calibrator.offset()*/); // Lettsome
    //signalFrequencyGuess = autoCorrelate(); // Vonk
    FFTFrequencyDetector fft;
    signalFrequencyGuess = fft.detectFrequency(samples, SAMPLES); // FFT
  }

  //SerialPrintStr("Frequency ");
  //Serial.println(signalFrequencyGuess);
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

/*    player.playLoomNote("C");
    player.playLoomNote("D");
    player.playLoomNote("E");
    player.playLoomNote("F");
    player.playLoomNote("G");
    player.playLoomNote("A");
    player.playLoomNote("B");
    player.playLoomNote("CC");*/

}
