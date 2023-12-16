/* Edge Impulse Arduino examples
 * Copyright (c) 2021 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Includes ---------------------------------------------------------------- */
#include <laura_skirt_inferencing.h>
#include <Arduino_LSM9DS1.h>

#include <Adafruit_NeoPixel.h>
#define LED_COUNT 30
Adafruit_NeoPixel strip1(LED_COUNT, 2, NEO_GRB + NEO_KHZ800);
/*Adafruit_NeoPixel strip2(LED_COUNT, 3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3(LED_COUNT, 4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4(LED_COUNT, 5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip5(LED_COUNT, 6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip6(LED_COUNT, 9, NEO_GRB + NEO_KHZ800);
*/
void run_animation();
enum State {none = 0, jump = 1, spin=2}; 
enum State detect = none;


/* Constant defines -------------------------------------------------------- */
#define CONVERT_G_TO_MS2    9.80665f

/* Private variables ------------------------------------------------------- */
static bool debug_nn = true; // Set this to true to see e.g. features generated from the raw signal
static uint32_t run_inference_every_ms = 200;
static rtos::Thread inference_thread(osPriorityLow);
static float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };
static float inference_buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

/* Forward declaration */
void run_inference_background();

/**
* @brief      Arduino setup function
*/
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Edge Impulse Inferencing Demo");
    strip1.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip1.show();            // Turn OFF all pixels ASAP
    strip1.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    /*
    strip2.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip2.show();            // Turn OFF all pixels ASAP
    strip2.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    strip3.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip3.show();            // Turn OFF all pixels ASAP
    strip3.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    strip4.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip4.show();            // Turn OFF all pixels ASAP
    strip4.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    strip5.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip5.show();            // Turn OFF all pixels ASAP
    strip5.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    strip6.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip6.show();            // Turn OFF all pixels ASAP
    strip6.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    */

    colorWipe(strip1.Color(255,   0,   0), 10); // Red

    while (!IMU.begin()) {
        ei_printf("Failed to initialize IMU!\r\n");
        delay(1000);
    }


    if (EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 3) {
        ei_printf("ERR: EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME should be equal to 3 (the 3 sensor axes)\n");
        return;
    }

    inference_thread.start(mbed::callback(&run_inference_background));
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip1.numPixels(); i++) { // For each pixel in strip...
    strip1.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    /*
    strip2.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip2.show();                          //  Update strip to match
    strip3.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip3.show();                          //  Update strip to match
    strip4.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip4.show();                          //  Update strip to match
    strip5.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip5.show();                          //  Update strip to match
    strip6.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip6.show();                          //  Update strip to match
    */
  }
    strip1.show();                          //  Update strip to match
}

/**
* @brief      Printf function uses vsnprintf and output using Arduino Serial
*
* @param[in]  format     Variable argument list
*/
void ei_printf(const char *format, ...) {
   static char print_buf[1024] = { 0 };

   va_list args;
   va_start(args, format);
   int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
   va_end(args);

   if (r > 0) {
       Serial.write(print_buf);
   }
}

/**
 * @brief      Run inferencing in the background.
 */
void run_inference_background()
{
    // wait until we have a full buffer
    delay((EI_CLASSIFIER_INTERVAL_MS * EI_CLASSIFIER_RAW_SAMPLE_COUNT) + 100);

    // This is a structure that smoothens the output result
    // With the default settings 70% of readings should be the same before classifying.
    ei_classifier_smooth_t smooth;
    ei_classifier_smooth_init(&smooth, 5 /* no. of readings */, 2 /* min. readings the same */, 0.8 /* min. confidence */, 0.3 /* max anomaly */);

    while (1) {
        // copy the buffer
        memcpy(inference_buffer, buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE * sizeof(float));

        // Turn the raw buffer in a signal which we can the classify
        signal_t signal;
        int err = numpy::signal_from_buffer(inference_buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
        if (err != 0) {
            ei_printf("Failed to create signal from buffer (%d)\n", err);
            return;
        }

        // Run the classifier
        ei_impulse_result_t result = { 0 };

        err = run_classifier(&signal, &result, debug_nn);
        if (err != EI_IMPULSE_OK) {
            ei_printf("ERR: Failed to run classifier (%d)\n", err);
            return;
        }

        // print the predictions
        ei_printf("Predictions ");
        ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
            result.timing.dsp, result.timing.classification, result.timing.anomaly);
        ei_printf(": ");

        // ei_classifier_smooth_update yields the predicted label
        const char *prediction = ei_classifier_smooth_update(&smooth, &result);

        if(strcmp(prediction,"jump") == 0)
            detect = jump;
        else if(strcmp(prediction, "spin")== 0)
            detect = spin;
        else
            detect = none;

        ei_printf("%s ", prediction);
        // print the cumulative results
        ei_printf(" [ ");
        for (size_t ix = 0; ix < smooth.count_size; ix++) {
            ei_printf("%u", smooth.count[ix]);
            if (ix != smooth.count_size + 1) {
                ei_printf(", ");
            }
            else {
              ei_printf(" ");
            }
        }
        ei_printf("]\n");

        delay(run_inference_every_ms);
    }

    ei_classifier_smooth_free(&smooth);
}

/**
* @brief      Get data and run inferencing
*
* @param[in]  debug  Get debug info if true
*/
void loop()
{
    while (1) {
        // Determine the next tick (and then sleep later)
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        // roll the buffer -3 points so we can overwrite the last one
        numpy::roll(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, -3);

        // read to the end of the buffer
        IMU.readAcceleration(
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 3],
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 2],
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 1]
        );

        buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 3] *= CONVERT_G_TO_MS2;
        buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 2] *= CONVERT_G_TO_MS2;
        buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 1] *= CONVERT_G_TO_MS2;


        // and wait for next tick
        uint64_t time_to_wait = next_tick - micros();
        delay((int)floor((float)time_to_wait / 1000.0f));
        delayMicroseconds(time_to_wait % 1000);

        if(detect == jump)
            colorWipe(strip1.Color(0,   255,   0), 0);
        else if(detect == spin)
            colorWipe(strip1.Color(0,   0,   255), 0);
        else
            colorWipe(strip1.Color(0,   0,   0), 0);

    }
}

void run_animation()
{
    static int pix = 0;

    // all off
    for(int i = 0; i < LED_COUNT; i ++)
        strip1.setPixelColor(i, 0);

    // red colour
    uint32_t red = strip1.Color(255,   0,   0); // Red

    // incrmeent led number
    if(pix++ == LED_COUNT)
        pix = 0;

    // set that one colour
    strip1.setPixelColor(pix, red);         //  Set pixel's color (in RAM)

    // update strip
    strip1.show();                          //  Update strip to match
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_ACCELEROMETER
#error "Invalid model for current sensor"
#endif
