# ArduinoLoomDistaff
Arduino sketch for a prop of the distaff from Lucasarts' Loom game.

## Details
The idea is for the distaff to behave like it does in the game.
It should:
- detect a note being played
- play the corresponding note in unison
- light up with the appropriate color for the given note
Optionally, it could also:
- detect a 4-note draft and play the "successful casting" sound

## Targeted hardware
- Arduino Uno
- Amplified Microphone
- RGB LED strip
- Arduino Wave Shield
- Speaker

## How to
- Install and run the Arduino IDE
- Install Adafruit_NeoPixel plugin
- Install WaveHC plugin
- Prepare your circuit and plug your Arduino Uno using USB
- Compile and upload the code

## Circuit schema
- Plug your Wave Shield on top of your Arduino Uno
- Plug the microphone on analog pin 0
- Plug RGB LED strip to digital pin 6
- Properly plug the 5V and Ground on the microphone and LED strip
- Add the speaker to the Wave Shield to hear audio

## To do
- Stabilize pitch detection