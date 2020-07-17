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
- Arduino Nano
- Amplified Microphone
- RGB LED strip
- Arduino Wave Shield

## How to
- Install and run the Arduino IDE
- Install Adafruit_NeoPixel plugin
- Prepare your circuit and plug your Arduino Nano using USB
- Compile and upload the code


## To do
- Sound playing is not written yet
- Pitch detection must be improved