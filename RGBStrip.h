#pragma once

#include <Adafruit_NeoPixel.h>

static const uint16_t LedCount = 60u;

#define Max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define Min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


class RGBStrip
{
public:
  // Pin 0 by default
  RGBStrip(uint16_t controlPin = 6): m_strip(LedCount, controlPin, NEO_GRB + NEO_KHZ800)
  {}

  void init()
  {
    m_strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    m_strip.show();            // Turn OFF all pixels ASAP
    m_strip.setBrightness(55); // Set BRIGHTNESS to about 1/5 (max = 255)
  }

  void clear()
  {
    m_strip.clear();
    m_strip.show();
  }

  void brightness(uint8_t brightness)
  {
    m_strip.setBrightness(brightness);
    m_brightness = brightness;
  }

  uint8_t brightness() const
  {
    return m_brightness;
  }

  void wave(uint32_t color, uint32_t fullDurationMs)
  {
    const auto constexpr KernelSize = 11;
    const float Kernel[KernelSize] = { 1.0f, 0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f, 0.0f };

    m_strip.clear();
    // Increase wave
    for (auto i = 0u; i < m_strip.numPixels() + KernelSize; ++i) // For each pixel in strip...
    {
      brightnessGradient<KernelSize>(color, Kernel, (int)i - KernelSize, i + 1);
      m_strip.show();
      delay(fullDurationMs / (2 * m_strip.numPixels()));
    }
    // Decrease wave
    for (auto i = m_strip.numPixels() + KernelSize - 1; i > 1; --i) // For each pixel in strip...
    {
      brightnessGradient<KernelSize>(color, Kernel, (int)i - KernelSize, i + 1);
      m_strip.show();
      delay(fullDurationMs / (2 * m_strip.numPixels()));
    }
    clear();
  }

  template<int KernelSize>
  void brightnessGradient(uint32_t color, const float kernel[KernelSize], int16_t rangeStart = 0, uint16_t rangeCount = LedCount)
  {
    for (auto i = rangeStart; i < int(m_strip.numPixels()) && i < int(rangeCount); ++i)
    {
      if (i >= 0)
      {
        auto brightnessRatio = kernel[Max(0, Min(i - rangeStart, KernelSize - 1))];
        Serial.print(i);
        Serial.print(", ");
        Serial.println(brightnessRatio);
        m_strip.setPixelColor(i, setColorBrightness(color, brightnessRatio));
      }
    }
    m_strip.show();
    Serial.println();
  }

  void colorOnRange(uint32_t color, int16_t rangeStart = 0, uint16_t rangeCount = LedCount)
  {
    for (auto i = rangeStart; i < int(m_strip.numPixels()) && i < int(rangeCount); ++i)
    {
      if (i >= 0)
        m_strip.setPixelColor(i, color);
    }
    m_strip.show();
  }

  void shimmerAroundColor(uint32_t color)
  {
    for (auto i = 0u; i < m_strip.numPixels(); ++i) // For each pixel in strip...
    {
      uint8_t brightness = random(50) + 150;
      m_strip.setPixelColor(i, setColorBrightness(color, brightness));
    }
    m_strip.show();
  }

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait)
{
  for (auto i = 0u; i < m_strip.numPixels(); ++i) // For each pixel in strip...
  {
    m_strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    m_strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait)
{
  for(int a=0; a<10; a++)  // Repeat 10 times...
  {
    for(int b=0; b<3; b++) //  'b' counts from 0 to 2...
    {
      m_strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c < (int)m_strip.numPixels(); c += 3)
      {
        m_strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      m_strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait)
{
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256)
  {
    for(int i=0; i < (int)m_strip.numPixels(); i++) // For each pixel in strip...
    {
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / m_strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      m_strip.setPixelColor(i, m_strip.gamma32(m_strip.ColorHSV(pixelHue)));
    }
    m_strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait)
{
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for (auto a = 0u; a < 30; ++a)  // Repeat 30 times...
  {
    for (auto b = 0u; b < 3; ++b) //  'b' counts from 0 to 2...
    {
      m_strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(auto c = b; c < m_strip.numPixels(); c += 3)
      {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / m_strip.numPixels();
        uint32_t color = m_strip.gamma32(m_strip.ColorHSV(hue)); // hue -> RGB
        m_strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      m_strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

private:
  uint8_t red(uint32_t color)
  {
    return uint8_t((color >> 16) & 0xff);
  }

  uint8_t green(uint32_t color)
  {
    return uint8_t((color >> 8) & 0xff);
  }

  uint8_t blue(uint32_t color)
  {
    return uint8_t(color & 0xff);
  }

  uint32_t setColorBrightness(uint32_t color, uint8_t brightness)
  {
    return Adafruit_NeoPixel::Color(red(color) * brightness / 255, green(color) * brightness / 255, blue(color) * brightness / 255);
  }

  uint32_t setColorBrightness(uint32_t color, float ratio)
  {
    return Adafruit_NeoPixel::Color(uint8_t(red(color) * ratio), uint8_t(green(color) * ratio), uint8_t(blue(color) * ratio));
  }

  void clearPixel(uint16_t index)
  {
    // Set to black to switch off
    m_strip.setPixelColor(index, 0);
  }

private:
    Adafruit_NeoPixel m_strip;
    uint8_t m_brightness;
};
