// #include "1_ColorMusic.ino"
// #include "2_Math.ino"

void volumeAnimate() {
  const float index = (float)255 / HALF_LED;
  byte counter = 0;

  for (int i = HALF_LED - 1; i > HALF_LED - 1 - Rlenght; i--) {
    switch (volumePalette) {
      case 0: leds[i] = ColorFromPalette(redtogreen_p, (counter * index)); break;
      case 1: leds[i] = ColorFromPalette(RainbowColors_p, (counter * index)); break;
      case 2: leds[i] = ColorFromPalette(ForestColors_p, (counter * index)); break;
      case 3: leds[i] = ColorFromPalette(CloudColors_p, (counter * index)); break;
      case 4: leds[i] = ColorFromPalette(LavaColors_p, (counter * index)); break;
      case 5: leds[i] = ColorFromPalette(OceanColors_p, (counter * index)); break;
      default: break;
    }
    counter++;
  }
  counter = 0;

  for (int i = HALF_LED; i < HALF_LED + Llenght; i++) {
    switch (volumePalette) {
      case 0: leds[i] = ColorFromPalette(redtogreen_p, (counter * index)); break;
      case 1: leds[i] = ColorFromPalette(RainbowColors_p, (counter * index)); break;
      case 2: leds[i] = ColorFromPalette(ForestColors_p, (counter * index)); break;
      case 3: leds[i] = ColorFromPalette(CloudColors_p, (counter * index)); break;
      case 4: leds[i] = ColorFromPalette(LavaColors_p, (counter * index)); break;
      case 5: leds[i] = ColorFromPalette(OceanColors_p, (counter * index)); break;
      default: break;
    }
    counter++;
  }
  counter = 0;

  if (emptyBright > 0) {
    CHSV empt = CHSV(emptyColor, 255, emptyBright);

    for (int i = HALF_LED - 1 - Rlenght; i > 0; i--)
      leds[i] = empt;

    for (int i = HALF_LED + Llenght; i < NUM_LEDS; i++)
      leds[i] = empt;
  }
}

void runningFreqAnimate() {
  static uint32_t running_timer;

  if (running_flag[2]) leds[HALF_LED] = CHSV(freqColors[2], 255, thisBright[2]);
  else if (running_flag[1]) leds[HALF_LED] = CHSV(freqColors[1], 255, thisBright[1]);
  else if (running_flag[0]) leds[HALF_LED] = CHSV(freqColors[0], 255, thisBright[0]);
  else leds[HALF_LED] = CHSV(emptyColor, 255, emptyBright);

  leds[HALF_LED - 1] = leds[HALF_LED];

  if (millis() - running_timer > runningFreqSpeed) {
    running_timer = millis();
    
    for (int i = 0; i < HALF_LED - 1; i++) {
      leds[i] = leds[i + 1];
      Serial.println(i);
      leds[HALF_LED - 1] = leds[i];
      FastLED.show();
    }
  } 
}

void strobe1FreqAnimate() {
  if (colorMusicFlash[2]) fill_solid(leds, NUM_LEDS, CHSV(freqColors[2], 255, thisBright[2]));
  else if (colorMusicFlash[1]) fill_solid(leds, NUM_LEDS, CHSV(freqColors[1], 255, thisBright[1]));
  else if (colorMusicFlash[0]) fill_solid(leds, NUM_LEDS, CHSV(freqColors[0], 255, thisBright[0]));
  else fill_solid(leds, NUM_LEDS, CHSV(emptyColor, 255, emptyBright));
}

void strobe6FreqAnimate() {
  const int freq6Leds = NUM_LEDS / 6;
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < freq6Leds)          leds[i] = CHSV(freqColors[2], 255, thisBright[2]);
    else if (i < freq6Leds * 2) leds[i] = CHSV(freqColors[1], 255, thisBright[1]);
    else if (i < freq6Leds * 3) leds[i] = CHSV(freqColors[0], 255, thisBright[0]);
    else if (i < freq6Leds * 4) leds[i] = CHSV(freqColors[1], 255, thisBright[1]);
    else if (i < freq6Leds * 5) leds[i] = CHSV(freqColors[2], 255, thisBright[2]);
  }
}

void freq20Animate() {
  byte HUEindex = hueStart;

  for (int i = 0; i < HALF_LED; i++) {
    const float numLedsFreq = NUM_LEDS / (float)40;
    const int j = floor((HALF_LED - i) / numLedsFreq);
    byte this_bright = map(freq_min[j], 0, freq_max_f, 0, 255);

    leds[i] = CHSV(HUEindex, 255, this_bright);
    leds[NUM_LEDS - i - 1] = leds[i];

    HUEindex += hueStep;
    if (HUEindex > 254) HUEindex = 0;
  } 
}

void animation() {
  switch (mode) {
    case 0: volumeAnimate(); break;
    case 1: runningFreqAnimate(); break;
    case 2: strobe1FreqAnimate(); break;
    case 3: strobe6FreqAnimate(); break;
    case 4: freq20Animate(); break;
  }
  if (mode != 1) FastLED.show();
}