#define FFT_SIZE 64 //размер выборки x2
#define NUM_LEDS 87 //количество светодиодов

#define LED_PIN 22
#define LED_CHIP WS2812
#define LED_COLOR GRB

#define SOUND_R_1 25 //порт звука 1
#define SOUND_R_2 26 //порт звука 2
#define EXP 1.4 //степень усиления сигнала для более резкой работы

#include "FastLED.h"
#include "FFT_C.h"
#include "WiFi.h"

CRGB leds[NUM_LEDS];

#define SMOOTH_STEP 20  
#define LIGHT_SMOOTH 2

byte portAux = 0;
const byte soundRList[2] = {SOUND_R_1, SOUND_R_2};

byte mode = 1;
byte emptyBright = 50;
byte bright = 150;
byte emptyColor = 192;

float volumeSmooth = 0.3;
byte volumePalette = 0; 

int runningFreqSpeed = 10;

byte hueStart = 0;
byte hueStep = 5;

byte freqColors[3] = {0, 64, 96};
byte addNoiseLowPassVolume = 10;
byte addNoiseLowPassSpectr = 10;
float SmoothFreq = 0.8;
float MaxCoefFreq = 1.5;

const int HALF_LED = NUM_LEDS / 2;
DEFINE_GRADIENT_PALETTE(soundlevel_gp) {
  0,    0,    255,  0,  // green
  100,  255,  255,  0,  // yellow
  150,  255,  100,  0,  // orange
  200,  255,  50,   0,  // red
  255,  255,  0,    0   // red
};
CRGBPalette32 redtogreen_p = soundlevel_gp;


void setup() {
  Serial.begin(115200);

  pinMode(SOUND_R_1, INPUT);
  pinMode(SOUND_R_2, INPUT);

  FastLED.addLeds<LED_CHIP, LED_PIN, LED_COLOR>(leds, NUM_LEDS);

  updateLowPass();
}
 
void loop() { 
  computeSound();
  animation();
}