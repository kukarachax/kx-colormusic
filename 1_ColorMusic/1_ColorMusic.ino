#define FFT_SIZE 64 //размер выборки x2
#define NUM_LEDS 87 //количество светодиодов

#define LED_PIN 22
#define LED_CHIP WS2812
#define LED_COLOR GRB

#define SOUND_R 25 //порт звука
#define EXP 1.4 //степень усиления сигнала для более резкой работы

#include "FastLED.h"
#include "FFT_C.h"
#include "WiFi.h"

CRGB leds[NUM_LEDS];

#define SMOOTH_STEP 20  
#define LIGHT_SMOOTH 2

byte emptyBright = 50;
byte bright = 255;
byte emptyColor = 192;
float SmoothFreq = 0.8;
float MaxCoefFreq = 1.5; 
float VolumeSmooth = 0.3;

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<LED_CHIP, LED_PIN, LED_COLOR>(leds, NUM_LEDS);
}
 

void loop() {
  ADC_FFT();
  filterFFT();
  filterVolume();
  //animation();
}

void animation() {
  FastLED.show();
}
