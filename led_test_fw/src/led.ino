#include <Adafruit_NeoPixel.h>
#define LED_COUNT 30
Adafruit_NeoPixel strip(LED_COUNT, 2, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(115200);
    Serial.println("Started");
    strip.setBrightness(100); // Set BRIGHTNESS to about 1/5 (max = 255)
    // colorWipe(strip.Color(0xff,   0x00,   0xeb), 0); // pink
    //colorWipe(strip.Color(0xff,   0x60,   0x00), 0); // orange
    //colorWipe(strip.Color(0xfb, 0xef, 0x00), 0); // yellow
    colorWipe(strip.Color(0x00, 0xff, 0x00), 0); // green
    strip.show();            // Turn OFF all pixels ASAP
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  }
}

void rainbow(int wait) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 3*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void loop() {
    rainbow(10);
}
/*
    float x, y, z;
    if (millis() > last_interval_ms + INTERVAL_MS) {
        last_interval_ms = millis();
        IMU.readAcceleration(x, y, z);
        Serial.print(x * CONVERT_G_TO_MS2);
        Serial.print('\t');
        Serial.print(y * CONVERT_G_TO_MS2);
        Serial.print('\t');
        Serial.println(z * CONVERT_G_TO_MS2);
        digitalWrite(LED, led); 
        led = !led;
    }
}

void run_animation()
{
    static int pix = 0;

    // all off
    for(int i = 0; i < LED_COUNT; i ++)
        strip.setPixelColor(i, 0);

    // red colour
    uint32_t red = strip.Color(255,   0,   0); // Red

    // incrmeent led number
    if(pix++ == LED_COUNT)
        pix = 0;

    // set that one colour
    strip.setPixelColor(pix, red);         //  Set pixel's color (in RAM)

    // update strip
    strip.show();                          //  Update strip to match
}
*/
